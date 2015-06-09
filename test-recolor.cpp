#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE Recolor
#include <boost/test/unit_test.hpp>

#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

BOOST_AUTO_TEST_CASE ( RecolorSubsetTest )
{
  std::vector<T::Transition > transition;
  transition.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3(0,0,1)) );
  transition.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( T::Transition(glm::vec3(0,1,1),glm::vec3(0,1,1)) );
  transition.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( T::Transition(glm::vec3(1,0,1),glm::vec3(1,0,1)) );
  transition.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( T::Transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  
  T recolor(transition);
  auto subset = recolor.subset(4, 0, transition.size());
  BOOST_LOG_TRIVIAL(info) << boost::format("SUBSET(4,%s): ") %subset.size();
  BOOST_LOG_TRIVIAL(info) << boost::format("COMBINATION(4,%s): ") %combination(transition.size(), 4);
  BOOST_CHECK_EQUAL(combination(transition.size(), 4), subset.size() );
  BOOST_CHECK_EQUAL(combination(transition.size(), 4), recolor.all_tetrs().size() );
  
  for ( T::Tetr &t : recolor.all_tetrs() ) {
    std::string str;
    for( T::Transition &e : t ) {
      str = (boost::format("%s %s") %str %glm::to_string(e.first)).str();
    }
    BOOST_LOG_TRIVIAL(trace) << str;
  }
  
}

BOOST_AUTO_TEST_CASE ( RecolorPleqCrossTest )
{
  glm::vec4 p1 = T::pleq(glm::vec3(-1.1,-1.2,1.25), glm::vec3(3,2,0.1), glm::vec3(1,1.3,0.2));
  BOOST_CHECK_LT(glm::length(p1 - glm::vec4(-0.485, 1.89, 3.53, -2.678)), T::_accuracy);
  BOOST_LOG_TRIVIAL(info) << "PLEQ : " << glm::to_string(p1);
  
  BOOST_CHECK_LT(glm::length(T::pleq(glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0)) - glm::vec4(0, 0, 1, 0)), T::_accuracy);
  BOOST_LOG_TRIVIAL(trace) << glm::length(T::pleq(glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0)) - glm::vec4(0, 0, 1, 0));
  
  glm::vec3 c1;
  T::cross(c1, glm::vec3(2.1,1.2,-0.1), glm::vec3(-1.3,-3,2.4), p1);
  BOOST_CHECK_LT(glm::length(c1 - glm::vec3(-0.288,-1.75,1.656)), T::_accuracy );
  BOOST_LOG_TRIVIAL(info) << "CROSS : " << glm::to_string(c1);
}

BOOST_AUTO_TEST_CASE ( RecolorAsSummTest )
{
  glm::vec3 s1;
  T::as_summ(s1, glm::vec3(0.9,0.9,0.9), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1));
  BOOST_CHECK_LT(glm::length(s1 - glm::vec3(0.9,0.9,0.9)), T::_accuracy );
  BOOST_LOG_TRIVIAL(info) << "AS SUMM : " << glm::to_string(s1);
  
  glm::vec3 s2;
  T::as_summ(s2, glm::vec3(-1.9,5.4,1.3), glm::vec3(1.7,0.34,-0.45), glm::vec3(1.2,-1.54,0.1), glm::vec3(-0.11,0.2,2.2));
  BOOST_CHECK_LT(glm::length(s2 - glm::vec3(1.152,-3.126,0.969)), T::_accuracy );
  BOOST_LOG_TRIVIAL(info) << "AS SUMM : " << glm::to_string(s2);
}

BOOST_AUTO_TEST_CASE ( RecolorInsideTest )
{
  BOOST_CHECK( !T::inside(glm::vec3(0.9,0.9,0.9), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( !T::inside(glm::vec3(0.5,0.5,0.5), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( !T::inside(glm::vec3(-1,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0.5,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0,0.5,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0,0,0.5), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(1,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0,1,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
  BOOST_CHECK( T::inside(glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(0,0,0)) );
}

BOOST_AUTO_TEST_CASE ( RecolorCrossingTest )
{
  glm::vec3 c;
  
  BOOST_CHECK(T::is_crossing(c, glm::vec3(0,0,1), glm::vec3(1,1,0),
                             glm::vec3(0,0,0), glm::vec3(1,0,1), glm::vec3(0,1,1)));
  
  BOOST_CHECK(!T::is_crossing(c, glm::vec3(0,0,-1), glm::vec3(1,1,-1),
                             glm::vec3(0,0,0), glm::vec3(1,0,1), glm::vec3(0,1,1)));
  
  BOOST_CHECK(!T::is_crossing(c, glm::vec3(0,0,0), glm::vec3(1,0,1),
                              glm::vec3(0,0,0), glm::vec3(1,0,1), glm::vec3(0,1,1)));
}

BOOST_AUTO_TEST_CASE ( RecolorIntersectionTest )
{
  
}
