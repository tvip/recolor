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

BOOST_AUTO_TEST_CASE( RecolorVoloumeTest )
{
  T::Tetr t;
  t.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  t.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  t.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3()) );
  t.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3()) );
  
  BOOST_CHECK_LT(glm::abs(T::volume(t) - 0.167), T::_accuracy);
  BOOST_LOG_TRIVIAL(trace) << "VOLUME : " << T::volume(t);
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
  BOOST_LOG_TRIVIAL(info) << boost::format("ALL TETRS : %s") %recolor.all_tetrs().size();
  BOOST_CHECK_EQUAL(combination(transition.size(), 4), subset.size() );
  BOOST_CHECK_EQUAL(recolor.all_tetrs().size(), 58 );
  
  for ( T::Tetr &t : recolor.all_tetrs() ) {
    std::string str;
    for( T::Transition &e : t ) {
      str = (boost::format("%s %s") %str %glm::to_string(e.first)).str();
    }
    BOOST_LOG_TRIVIAL(trace) << str;
  }
  
  float V = 0;
  for ( const std::vector< std::vector<T::Transition>::const_iterator > &t : recolor.m_fill_tetrs ) {
    T::Tetr tetr;
    tetr.push_back( T::Transition( *t[0] ) );
    tetr.push_back( T::Transition( *t[1] ) );
    tetr.push_back( T::Transition( *t[2] ) );
    tetr.push_back( T::Transition( *t[3] ) );
    V += T::volume( tetr );
  }
  
  BOOST_CHECK_LT(glm::abs(V - 1), T::_accuracy);
  BOOST_LOG_TRIVIAL(info) << "VOLUME : " << V;
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
  
  BOOST_CHECK(T::is_crossing(c, glm::vec3(0,0,1), glm::vec3(1,1,0),
                             glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,1,1)));
  BOOST_CHECK_LT(glm::length(c - glm::vec3(0.5,0.5,0.5)), T::_accuracy );
  BOOST_LOG_TRIVIAL(info) << "IS CROSSING : " << glm::to_string(c);
  
  BOOST_CHECK(T::is_crossing(c, glm::vec3(0,0,0), glm::vec3(1,0,1),
                             glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1)));
  BOOST_CHECK_LT(glm::length(c - glm::vec3(0.5,0,0.5)), T::_accuracy );
  BOOST_LOG_TRIVIAL(info) << "IS CROSSING : " << glm::to_string(c);
}

BOOST_AUTO_TEST_CASE ( RecolorIntersectionTest )
{
#if 1
  T::Tetr A;
  A.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  A.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  A.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3()) );
  A.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3()) );
  
  T::Tetr B;
  B.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  B.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  B.push_back( T::Transition(glm::vec3(1,1,1),glm::vec3()) );
  B.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3()) );
  
  T::Tetr C;
  C.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  C.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  C.push_back( T::Transition(glm::vec3(1,1,-1),glm::vec3()) );
  C.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3()) );
  
  std::vector<glm::vec3> I = T::intersection(A, B);
  BOOST_CHECK_EQUAL(I.size(), 28);
  BOOST_LOG_TRIVIAL(info) << "INTERSECTION : " << I.size();
  
  std::vector<glm::vec3> R = T::reduce(I);
  BOOST_CHECK_EQUAL(R.size(), 4);
  BOOST_LOG_TRIVIAL(info) << "REDUCE : " << R.size();
//  for ( glm::vec3 i : R )
//    BOOST_LOG_TRIVIAL(trace) << "INTERSECTION : " << glm::to_string(i);
  
  BOOST_CHECK(!T::competitable(A, B));
  BOOST_CHECK(!T::competitable(A, A));
  BOOST_CHECK(T::competitable(B, C));
  BOOST_CHECK(T::competitable(A, C));
#endif
  
#if 0
  T::Tetr D;
  D.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  D.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3()) );
  D.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3()) );
  D.push_back( T::Transition(glm::vec3(0,1,1),glm::vec3()) );
  
  T::Tetr E;
  E.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  E.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3()) );
  E.push_back( T::Transition(glm::vec3(1,0,1),glm::vec3()) );
  E.push_back( T::Transition(glm::vec3(1,1,1),glm::vec3()) );
  
  BOOST_CHECK(T::competitable(D, E));
#endif
  
  T::Tetr M;
  M.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  M.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3()) );
  M.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3()) );
  M.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3()) );
  
  T::Tetr N;
  N.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3()) );
  N.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3()) );
  N.push_back( T::Transition(glm::vec3(0,1,1),glm::vec3()) );
  N.push_back( T::Transition(glm::vec3(1,0,1),glm::vec3()) );
  
  std::vector<glm::vec3> I2 = T::intersection(M, N);
  BOOST_CHECK( !T::competitable(M, N) );
}

BOOST_AUTO_TEST_CASE ( RecolorFillTest )
{
  std::vector<T::Transition > transition;
  transition.push_back( T::Transition(glm::vec3(0.33,0.8,0.95),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0.5,1),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( T::Transition(glm::vec3(0,1,1),glm::vec3(0,0.3,0.3)) );
  transition.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( T::Transition(glm::vec3(1,0,1),glm::vec3(1,0,0)) );
  transition.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( T::Transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  
  T recolor(transition);
  BOOST_LOG_TRIVIAL(info) << "FILL : " << recolor.fill_tetrs().size();
  
  float V = 0;
  for ( T::Tetr &t : recolor.fill_tetrs() ) {
    V += T::volume(t);
  }
  
  BOOST_CHECK_LT(glm::abs(V - 1), T::_accuracy);
  BOOST_LOG_TRIVIAL(info) << "VOLUME : " << V;
  
  for ( T::Tetr &t : recolor.fill_tetrs() ) {
    std::string str;
    for( T::Transition &e : t ) {
      str = (boost::format("%s %s") %str %glm::to_string(e.first)).str();
    }
    BOOST_LOG_TRIVIAL(trace) << boost::format("%s %s") %T::volume(t) %str;
  }
}

BOOST_AUTO_TEST_CASE ( RecolorRastrTest )
{
  {
    glm::vec4 K = T::rastr(T::rgb(0.57,0.61,0.52), T::rgb(0,0,1), T::rgb(0,1,0), T::rgb(1,0,0), T::rgb(1,1,1));
    float S = K[0] + K[1] + K[2] + K[3];
    BOOST_CHECK_LE(glm::length(K - glm::vec4(0.17,0.26,0.22,0.35)), T::_accuracy);
    BOOST_CHECK_LE(glm::abs( S - 1.f ), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("RASRT %s : %s") %S %glm::to_string(K);
  }
  {
    glm::vec4 K = T::rastr(T::rgb(0.5,0.5,0.5), T::rgb(0,0,1), T::rgb(0,1,0), T::rgb(0,1,1), T::rgb(1,0,0));
    float S = K[0] + K[1] + K[2] + K[3];
    BOOST_CHECK_LE(glm::length(K - glm::vec4(0,0,0.5,0.5)), T::_accuracy);
    BOOST_CHECK_LE(glm::abs( S - 1.f ), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("RASRT %s : %s") %S %glm::to_string(K);
  }
}

BOOST_AUTO_TEST_CASE ( RecolorTest )
{
  std::vector<T::Transition > transition;
  transition.push_back( T::Transition(glm::vec3(0.33,0.8,0.95),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0.5,1),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( T::Transition(glm::vec3(0,0,1),glm::vec3(1,0.5,0)) );
  transition.push_back( T::Transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( T::Transition(glm::vec3(0,1,1),glm::vec3(0,0.3,0.3)) );
  transition.push_back( T::Transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( T::Transition(glm::vec3(1,0,1),glm::vec3(1,0,0)) );
  transition.push_back( T::Transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( T::Transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  T recolor(transition);
  
  {
    T::rgb origin = T::rgb(0,0,0);
    T::rgb result = recolor.fromColor(origin);
    
    BOOST_CHECK_LE(glm::length(result - glm::vec3(0,0,0)), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("TRANSITION : %s -> %s") %glm::to_string(origin) %glm::to_string(result);
  }
  
  {
    T::rgb origin = T::rgb(0,0.9,1);
    T::rgb result = recolor.fromColor(origin);
    
    BOOST_CHECK_LE(glm::length(result - glm::vec3(0.2,0.34,0.24)), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("TRANSITION : %s -> %s") %glm::to_string(origin) %glm::to_string(result);
  }
  
  {
    T::rgb origin = T::rgb(0.33,0.8,0.95);
    T::rgb result = recolor.fromColor(origin);
    
    BOOST_CHECK_LE(glm::length(result - glm::vec3(1,0.5,0)), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("TRANSITION : %s -> %s") %glm::to_string(origin) %glm::to_string(result);
  }
  
  {
    T::rgb origin = T::rgb(1,1,1);
    T::rgb result = recolor.fromColor(origin);
    
    BOOST_CHECK_LE(glm::length(result - glm::vec3(1,1,1)), T::_accuracy);
    BOOST_LOG_TRIVIAL(info) << boost::format("TRANSITION : %s -> %s") %glm::to_string(origin) %glm::to_string(result);
  }
}
