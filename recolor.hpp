#pragma once

#define GLM_FORCE_CXX98 1
#define GLM_FORCE_RADIANS 1
#include <glm/fwd.hpp>

#include <vector>
#include <array>

class ColorTransition {
  friend struct RecolorSubsetTest;
  friend struct RecolorPleqCrossTest;
  
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
  static const float _accuracy;
  
  const std::vector<transition> m_transition;
  std::vector<tetr> m_fill_tetrs;
  
  std::vector<std::vector<unsigned> > subset(unsigned n, unsigned offset);
  std::vector<tetr> all_tetrs();
  
  static glm::vec4 pleq(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C);
  static glm::vec3 cross(const glm::vec3 &M, const glm::vec3 &N, const glm::vec4 &P);
  
  void intersection(tetr A, tetr B);
  
};
