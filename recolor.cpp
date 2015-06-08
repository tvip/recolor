#include "recolor.hpp"

#include <glm/glm.hpp>

const float ColorTransition::_accuracy = 0.001f;

ColorTransition::ColorTransition( const std::vector<transition> &transition )
: m_transition(transition)
{
  //transition[0].first.r = 0;
  
  
}

std::vector<std::vector<unsigned> > ColorTransition::subset(unsigned n, unsigned offset)
{
  std::vector<std::vector<unsigned> > res;
  
  if (n == 1) {
    for (unsigned i=0; i<m_transition.size()-offset; ++i) {
      std::vector<unsigned> set;
      set.push_back(i+offset);
      res.push_back(set);
    }
  }
  else {
    for (const auto &s : subset(n-1, offset+1)) {
      std::vector<unsigned> set;
      set.push_back(offset);
      set.insert(set.end(), s.begin(), s.end());
      res.push_back(set);
    }
    
    if (n + offset < m_transition.size()) {
      for (const auto &s : subset(n, offset+1)) {
        res.push_back(s);
      }
    }
    
  }
  
  return res;
}

std::vector<ColorTransition::tetr> ColorTransition::all_tetrs()
{
  std::vector<ColorTransition::tetr> res;
  for ( auto &v : subset(4,0) ) {
    tetr t = {{v[0],v[1],v[2],v[3]}};
    res.push_back(t);
  }
  return res;
}

void ColorTransition::intersection(ColorTransition::tetr A, ColorTransition::tetr B)
{
  auto Aedge = subset(2, 0);
  auto Aface = subset(3, 0);
  auto Bedge = subset(2, 0);
  auto Bface = subset(3, 0);
  
  glm::vec4 vec(0,0,0,0);
//  vec.d;
}

glm::vec4 ColorTransition::pleq(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C)
{
  return glm::vec4
  (
   A.y*B.z - A.z*B.y - A.y*C.z + A.z*C.y + B.y*C.z - B.z*C.y,
   A.z*B.x - A.x*B.z + A.x*C.z - A.z*C.x - B.x*C.z + B.z*C.x,
   A.x*B.y - A.y*B.x - A.x*C.y + A.y*C.x + B.x*C.y - B.y*C.x,
   A.x*B.z*C.y - A.x*B.y*C.z + A.y*B.x*C.z - A.y*B.z*C.x - A.z*B.x*C.y + A.z*B.y*C.x
   );
}

glm::vec3 ColorTransition::cross(const glm::vec3 &M, const glm::vec3 &N, const glm::vec4 &P)
{
  return glm::vec3
  (
   -(M.x*P.w - N.x*P.w + M.x*N.y*P.y - M.y*N.x*P.y + M.x*N.z*P.z - M.z*N.x*P.z)
   / (M.x*P.x + M.y*P.y + M.z*P.z - N.x*P.x - N.y*P.y - N.z*P.z),
   
   -(M.y*P.w - N.y*P.w - M.x*N.y*P.x + M.y*N.x*P.x + M.y*N.z*P.z - M.z*N.y*P.z)
   / (M.x*P.x + M.y*P.y + M.z*P.z - N.x*P.x - N.y*P.y - N.z*P.z),
   
   -(M.z*P.w - N.z*P.w - M.x*N.z*P.x + M.z*N.x*P.x - M.y*N.z*P.y + M.z*N.y*P.y)
   / (M.x*P.x + M.y*P.y + M.z*P.z - N.x*P.x - N.y*P.y - N.z*P.z)
   );
}

ColorTransition::image ColorTransition::fromImage( const image &img )
{
  
}

ColorTransition::rgb ColorTransition::fromColor( const rgb &color )
{
  
}
