#define BOOST_TEST_MODULE test
#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include <set>
#include <vector>

#include "HashSet.hpp"

BOOST_AUTO_TEST_CASE( hashSet_simple_int32 )
{
    HashSet< HashSetTraits<32> > set;
    BOOST_CHECK_EQUAL(set.size(), 0);
    set += 42;
    BOOST_CHECK(set[42]);
    BOOST_CHECK_EQUAL(set.size(), 1);
    set += 42;
    BOOST_CHECK(set[42]);
    BOOST_CHECK_EQUAL(set.size(), 1);
}

BOOST_AUTO_TEST_CASE( resizes )
{
    HashSet< HashSetTraits<32> > set;
    BOOST_CHECK_EQUAL(set.capacity(), 1<<5);
    for (int i=0; i<0.74*256; ++i)
    {
        set += i;
//        std::cout << "size=" << set.size() << " capacity=" << set.capacity() << std::endl;
    }
}

typedef boost::mpl::list<int,long,unsigned char> test_types;

typedef boost::mpl::list<int,long,unsigned char> test_types;

BOOST_AUTO_TEST_CASE( hashSet_int8_full_random_symmetric )
{
    std::cout << "rand" << std::endl;
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> dist(0,255);

    HashSet< HashSetTraits<8> > set;

    std::vector<int> requests;
    for (int i=0; i<0.70*255; ++i) {
        requests.push_back(dist(rng));
    }

    std::set<int> seen;
    for (int r : requests) {
        set += r;
        seen.insert(r);
    }
    for (int i=0; i<256; ++i) {
        BOOST_CHECK_EQUAL(set[i], seen.count(i));
    }
}

BOOST_AUTO_TEST_CASE( hashSet_random_symmetric )
{

}
