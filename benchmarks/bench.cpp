#include <boost/random.hpp>
#include <boost/timer/timer.hpp>

#include "HashSet.hpp"

int main(int argc, char *argv[])
{
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> dist(0,4000000); // TODO use asymmetrical distribution

    HashSet< HashSetTraits<32> > set;

    {
        boost::timer::auto_cpu_timer t;
        for (int n=0; n<2000000; ++n) {
            int r = dist(rng);
            set += r;
        }
        // TODO iterate over result and compute sum
    }

}

