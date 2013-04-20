#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include <numeric>
#include <iostream>

#include "HashSet.hpp"

int main(int argc, char *argv[])
{
    const int valueBitWidth = 32;
    const int indexBitWidth = 64;

    boost::random::mt19937 rng; //
    boost::uint_t<valueBitWidth>::least maxValue = std::numeric_limits<boost::int_t<valueBitWidth>::least>::max(); // TODO replace by 1L<<valueBitWidth
    std::cout << "maxValue=" << maxValue << std::endl;
    boost::random::uniform_int_distribution<> dist(0,maxValue);

    int numberOfItems = 16000000;

    HashSet< HashSetTraits<valueBitWidth, indexBitWidth> > set;

    {
        boost::timer::auto_cpu_timer t;
        for (int n=0; n<numberOfItems; ++n) {
            int r = dist(rng);
            set += r;
        }
        // TODO iterate over result and compute sum
    }

}

