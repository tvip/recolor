#pragma once

#define GLM_FORCE_RADIANS 1
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <array>

class ColorTransition {
  friend struct RecolorSimpleTest;
  
public:
  typedef glm::vec3 rgb;
  typedef glm::vec4 rgba;
  typedef std::pair<rgb,rgb> transition;
  typedef std::vector<std::vector<rgba> > image;
  
  ColorTransition( const std::vector<transition> &transition );
  
  image fromImage( const image &img );
  glm::vec3 fromColor( const rgb &color );
  
private:
  typedef std::array<unsigned,4> tetr;
  static const float _accuracy = 0.001f;
  
  const std::vector<transition> m_transition;
  std::vector<tetr> m_fill_tetrs;
  
  std::vector<std::vector<unsigned> > subset(unsigned n, unsigned offset);
  
};
