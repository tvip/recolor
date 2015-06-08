#define BOOST_TEST_DYN_LINK 1
#define BOOST_TEST_MODULE Recolor
#include <boost/test/unit_test.hpp>

#include "recolor.hpp"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

BOOST_AUTO_TEST_CASE ( RecolorSimpleTest )
{
  std::vector<Recolor::transition > transition;
  transition.push_back( Recolor::transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( Recolor::transition(glm::vec3(0,0,1),glm::vec3(0,0,1)) );
  transition.push_back( Recolor::transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( Recolor::transition(glm::vec3(0,1,1),glm::vec3(0,1,1)) );
  transition.push_back( Recolor::transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( Recolor::transition(glm::vec3(1,0,1),glm::vec3(1,0,1)) );
  transition.push_back( Recolor::transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( Recolor::transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  
  Recolor recolor(transition);
}
