#pragma once

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

class Recolor {
  friend struct RecolorSimpleTest;
  
public:
  typedef const std::pair<glm::vec3,glm::vec3> transition;
  
  Recolor( const std::vector<transition> &transition );
  
private:
  const std::vector<const std::pair<glm::vec3,glm::vec3> > m_transition;
  
};
