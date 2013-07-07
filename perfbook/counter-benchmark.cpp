// TODO use boost preprocessor programming for dispatch
// TODO different program_option groups:
// - template arguments
// - variable arguments
// - benchmark related arguments
// TODO allow configuration file
// TODO use a benchmarking library (crono?)
// TODO code reuse for other benchmark programs
// TODO ifdef userspace-rcu supported

#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <map>

#include "UnsynchronizedCounter.hpp"
#include "AtomicCounter.hpp"
#include "UrcuAtomicCounter.hpp"
#include "ThreadLocalCounter.hpp"

namespace po = boost::program_options;

struct WorkerSpecification {
  std::string algorithm;
  int bitWidth;
  WorkerSpecification(const std::string & algorithm, int bitWidth)
    : algorithm(algorithm)
    , bitWidth(bitWidth)
  {}
  WorkerSpecification(const po::variables_map & vm)
    : algorithm(vm["algorithm"].as<std::string>())
    , bitWidth(vm["bit-width"].as<unsigned int>())
  {}
  bool operator<(const WorkerSpecification& r) const {
    return bitWidth < r.bitWidth || (bitWidth == r.bitWidth && algorithm < r.algorithm);
  }
};

struct WorkerArguments {
  long iterationCount;
  WorkerArguments(const po::variables_map & vm)
    : iterationCount(vm["iteration-count"].as<unsigned long>())
  {}
};

template<typename T>
int dispatchWork(const WorkerSpecification & spec, const WorkerArguments & args) {
}

class BaseWorker {
public:
  virtual void work(const WorkerArguments&) = 0;
  virtual long read() = 0;
};

template<class CounterType>
class Worker : public BaseWorker {

  CounterType counter;

public:
  virtual void work(const WorkerArguments& args) {
    counter.registerThread();
    for (unsigned long i=0; i<args.iterationCount; ++i) {
      if (i % 2)
        counter.inc();
    }
    counter.unregisterThread();
  }
  virtual long read() {
    return counter;
  }
};

#define NEW_WORKER(type, nBits) \
  { \
    WorkerSpecification spec = WorkerSpecification(#type, (nBits)); \
    specMap[spec] = new Worker<type<boost::uint_t<nBits>::least>>(); \
  }

int main(int argc, char *argv[])
{
  std::map<WorkerSpecification, BaseWorker*> specMap; // TODO destroy
  NEW_WORKER(UnsynchronizedCounter, 64)
  NEW_WORKER(AtomicCounter, 64)
  NEW_WORKER(UrcuAtomicCounter, 64)
  NEW_WORKER(ThreadLocalCounter, 64)

  NEW_WORKER(UnsynchronizedCounter, 32)
  NEW_WORKER(AtomicCounter, 32)
  NEW_WORKER(UrcuAtomicCounter, 32)
  NEW_WORKER(ThreadLocalCounter, 32)

  unsigned int nCpus = std::thread::hardware_concurrency();

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "show help message")
    ("iteration-count", po::value<unsigned long>()->default_value(1000000), "set number of iterations per thread")
    ("algorithm", po::value<std::string>()->default_value("AtomicCounter"), "the algorithm (one of: UnsynchronizedCounter AtomicCounter UrcuAtomicCounter ThreadLocalCounter)") // TODO automatically deduce alternatives
    ("number-of-threads", po::value<unsigned int>()->default_value(nCpus), "number of threads")
    ("bit-width", po::value<unsigned int>()->default_value(64), "number of bits (one of: 32 64)")  // TODO detect default from architecture
    ;

  try {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
    }

    unsigned int nThreads = vm["number-of-threads"].as<unsigned int>();
    std::vector<std::thread> threads;
    BaseWorker* worker = specMap[WorkerSpecification(vm)];
    if (worker == nullptr) { // TODO or do we need to call contains?
      std::cerr << "implementation not found!" << std::endl;
      return 1;
    }
    for (int i=0; i<nThreads; ++i) {
      threads.push_back(std::thread([worker, &vm] () {worker->work(WorkerArguments(vm));}));
    }
    for (std::thread& t : threads)
      t.join();
    // TODO verify counter, return 1 on mismatch
    long expectedCount = nThreads * (vm["iteration-count"].as<unsigned long>() / 2);
    long actualCount = worker->read();
    if (expectedCount != actualCount) {
      std::cerr << "invalid count, expected " << expectedCount << std::endl;
      std::cerr << "                but was " << actualCount << std::endl;
      return 1;
    }
  }
  catch ( const boost::program_options::error & e ) {
    std::cerr << e.what() << std::endl;
    std::cout << desc << "\n";
    return 1;
  }
  return 0;
}
