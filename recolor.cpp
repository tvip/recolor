#include "recolor.hpp"

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

ColorTransition::image ColorTransition::fromImage( const image &img )
{
  
}

ColorTransition::rgb ColorTransition::fromColor( const rgb &color )
{
  
}
