#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

const float ColorTransition::_accuracy = 0.001f;

ColorTransition::ColorTransition( const std::vector<Transition> &transition )
: m_transition(transition)
{
  //transition[0].first.r = 0;
  
  
}

std::vector<std::vector<unsigned> > ColorTransition::subset(unsigned n, unsigned offset, unsigned size)
{
  std::vector<std::vector<unsigned> > res;
  
  if (n == 1) {
    for (unsigned i=0; i<size-offset; ++i) {
      std::vector<unsigned> set;
      set.push_back(i+offset);
      res.push_back(set);
    }
  }
  else {
    for (const auto &s : subset(n-1, offset+1, size)) {
      std::vector<unsigned> set;
      set.push_back(offset);
      set.insert(set.end(), s.begin(), s.end());
      res.push_back(set);
    }
    
    if (n + offset < size) {
      for (const auto &s : subset(n, offset+1, size)) {
        res.push_back(s);
      }
    }
    
  }
  
  return res;
}

std::vector<ColorTransition::Tetr> ColorTransition::all_tetrs()
{
  return combos(4, m_transition);
}

std::vector<glm::vec3> ColorTransition::intersection(const Tetr &A, const Tetr &B) const
{
  std::vector< std::vector<Transition> > Aedge = combos(2, A);
  std::vector< std::vector<Transition> > Aface = combos(3, A);
  std::vector< std::vector<Transition> > Bedge = combos(2, B);
  std::vector< std::vector<Transition> > Bface = combos(3, B);
  
  std::vector<glm::vec3> res;
  glm::vec3 c;
  
  for (const Edge &edge : Aedge) {
    for (const Face &face : Bface) {
      if ( is_crossing(c, edge[0].first, edge[1].first, face[0].first, face[1].first, face[2].first) ) {
        res.push_back(c);
      }
    }
  }
  
  for (const Edge &edge : Bedge) {
    for (const Face &face : Aface) {
      if ( is_crossing(c, edge[0].first, edge[1].first, face[0].first, face[1].first, face[2].first) ) {
        res.push_back(c);
      }
    }
  }
}

bool ColorTransition::is_crossing(glm::vec3 &c, const glm::vec3 &e0, const glm::vec3 &e1, const glm::vec3 &f0, const glm::vec3 &f1, const glm::vec3 &f2)
{
#if 0
  const glm::vec3 &e0 = m_transition[edge[0]].first;
  const glm::vec3 &e1 = m_transition[edge[1]].first;
  const glm::vec3 &f0 = m_transition[face[0]].first;
  const glm::vec3 &f1 = m_transition[face[1]].first;
  const glm::vec3 &f2 = m_transition[face[2]].first;
#endif
  
  glm::vec4 eq = pleq(f0, f1, f2);
  
  if (!cross(c, e0, e1, eq))
    return false;
  
  glm::vec3 p = glm::vec3(eq.x,eq.y,eq.z);
  glm::vec3 k;
  
  return
  as_summ( k, c - f0, f1 - f0, f2 - f0, p ) &&
  glm::length(e0 - c) < glm::length(e0 - e1) + _accuracy &&
  glm::length(e1 - c) < glm::length(e0 - e1) + _accuracy &&
  k.x > -_accuracy && k.y > -_accuracy &&
  k.x + k.y + glm::abs(k.z) < 1.f + _accuracy;
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

bool ColorTransition::cross(glm::vec3 &C, const glm::vec3 &M, const glm::vec3 &N, const glm::vec4 &P)
{
  bool ok;
  const float denom = M.x*P.x + M.y*P.y + M.z*P.z - N.x*P.x - N.y*P.y - N.z*P.z;
  
  if (denom != 0) {
    ok = true;
    C = glm::vec3
    (
     -( M.x*P.w - N.x*P.w + M.x*N.y*P.y - M.y*N.x*P.y + M.x*N.z*P.z - M.z*N.x*P.z ) / denom,
     -( M.y*P.w - N.y*P.w - M.x*N.y*P.x + M.y*N.x*P.x + M.y*N.z*P.z - M.z*N.y*P.z ) / denom,
     -( M.z*P.w - N.z*P.w - M.x*N.z*P.x + M.z*N.x*P.x - M.y*N.z*P.y + M.z*N.y*P.y ) / denom
     );
  }
  else {
    ok = false;
  }
  
  return ok;
}

bool ColorTransition::as_summ(glm::vec3 &K, const glm::vec3 &D, const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C)
{
  bool ok;
  const float denom = A.x*B.y*C.z - A.x*B.z*C.y - A.y*B.x*C.z + A.y*B.z*C.x + A.z*B.x*C.y - A.z*B.y*C.x;
  
  if (denom != 0) {
    ok = true;
    K = glm::vec3
    (
     ( B.x*C.y*D.z - B.x*C.z*D.y - B.y*C.x*D.z + B.y*C.z*D.x + B.z*C.x*D.y - B.z*C.y*D.x ) / denom,
     -( A.x*C.y*D.z - A.x*C.z*D.y - A.y*C.x*D.z + A.y*C.z*D.x + A.z*C.x*D.y - A.z*C.y*D.x ) / denom,
     ( A.x*B.y*D.z - A.x*B.z*D.y - A.y*B.x*D.z + A.y*B.z*D.x + A.z*B.x*D.y - A.z*B.y*D.x ) / denom
     );
  }
  else {
    ok = false;
  }
  
  return ok;
}

bool ColorTransition::inside(const glm::vec3 &O, const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C, const glm::vec3 &D)
{
  glm::vec3 k;
//  BOOST_LOG_TRIVIAL(trace) << "INSIDE : " << glm::to_string(O-A);
//  BOOST_LOG_TRIVIAL(trace) << "INSIDE : " << glm::to_string(B-A);
//  BOOST_LOG_TRIVIAL(trace) << "INSIDE : " << glm::to_string(C-A);
//  BOOST_LOG_TRIVIAL(trace) << "INSIDE : " << glm::to_string(D-A);
//  BOOST_LOG_TRIVIAL(trace) << "INSIDE : " << glm::to_string(k);
  
  return
  as_summ(k, O-A, B-A, C-A, D-A) &&
  k.x > -_accuracy &&
  k.y > -_accuracy &&
  k.z > -_accuracy &&
  k.x + k.y + k.z < 1.f + _accuracy;
}

ColorTransition::Image ColorTransition::fromImage( const Image &img )
{
  
}

ColorTransition::rgb ColorTransition::fromColor( const rgb &color )
{
  
}
