#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE Recolor
#include <boost/test/unit_test.hpp>

#include "recolor.hpp"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

typedef ColorTransition T;

static unsigned factorial(unsigned n) {
  unsigned res = 1;
  for (unsigned i=1; i<=n; ++i)
    res *= i;
  return res;
}

BOOST_AUTO_TEST_CASE ( FactorialTest )
{
  BOOST_CHECK_EQUAL(factorial(1), 1);
  BOOST_CHECK_EQUAL(factorial(2), 2);
  BOOST_CHECK_EQUAL(factorial(3), 6);
  BOOST_CHECK_EQUAL(factorial(4), 24);
  BOOST_CHECK_EQUAL(factorial(5), 120);
}

static unsigned combination(unsigned n, unsigned k) {
  return factorial(n) / ( factorial(k) * factorial(n - k) );
}

BOOST_AUTO_TEST_CASE ( CombinationTest )
{
  BOOST_CHECK_EQUAL(combination(1, 1), 1);
  BOOST_CHECK_EQUAL(combination(3, 1), 3);
  BOOST_CHECK_EQUAL(combination(5, 5), 1);
  BOOST_CHECK_EQUAL(combination(8, 4), 70);
}

BOOST_AUTO_TEST_CASE ( RecolorSimpleTest )
{
  std::vector<T::transition > transition;
  transition.push_back( T::transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( T::transition(glm::vec3(0,0,1),glm::vec3(0,0,1)) );
  transition.push_back( T::transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( T::transition(glm::vec3(0,1,1),glm::vec3(0,1,1)) );
  transition.push_back( T::transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( T::transition(glm::vec3(1,0,1),glm::vec3(1,0,1)) );
  transition.push_back( T::transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( T::transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  
  T recolor(transition);
  auto subset = recolor.subset(4, 0);
  BOOST_LOG_TRIVIAL(info) << boost::format("SUBSET(4,%s): ") %subset.size();
  BOOST_LOG_TRIVIAL(info) << boost::format("COMBINATION(4,%s): ") %combination(transition.size(), 4);
  BOOST_CHECK_EQUAL(subset.size(), combination(transition.size(), 4));
  
  for ( auto &t : subset ) {
    std::string str;
    for( auto &e : t ) {
      str = (boost::format("%s %s") %str %e).str();
    }
    BOOST_LOG_TRIVIAL(trace) << str;
  }
  
  
  
  
}
