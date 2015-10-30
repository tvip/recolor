#pragma once

#include <glm/fwd.hpp>

#include <vector>

class ColorTransition {
  friend struct RecolorVoloumeTest;
  friend struct RecolorIntersectionTest;
  friend struct RecolorSubsetTest;
  friend struct RecolorPleqCrossTest;
  friend struct RecolorAsSummTest;
  friend struct RecolorInsideTest;
  friend struct RecolorCrossingTest;
  friend struct RecolorFillTest;
  friend struct RecolorRastrTest;
  
  
public:
  typedef glm::vec3 rgb;
  typedef glm::vec4 rgba;
  typedef std::pair<rgb,rgb> Transition;
  typedef std::vector<std::vector<rgba> > Image;
  
  static const float _accuracy;
  
  ColorTransition( const std::vector<Transition> &transition );
  
  Image fromImage( const Image &img ) const;
  glm::vec3 fromColor( const rgb &color ) const;
  
private:
  typedef std::vector<Transition> Tetr;
  typedef std::vector<Transition> Face;
  typedef std::vector<Transition> Edge;
  
  const std::vector<Transition> m_transition;
  std::vector< std::vector< std::vector<Transition>::const_iterator > > m_fill_tetrs;
  
  static std::vector<std::vector<unsigned> > subset(unsigned n, unsigned offset, unsigned size);
  std::vector<Tetr> all_tetrs() const;
  std::vector<Tetr> fill_tetrs() const;
  
  static glm::vec4 rastr(const rgb &color, const rgb &A, const rgb &B, const rgb &C, const rgb &D);
  static bool competitable(const Tetr &A, const Tetr &B);
  static std::vector<glm::vec3> intersection(const Tetr &A, const Tetr &B);
  static std::vector<glm::vec3> reduce(const std::vector<glm::vec3> &V);
  static float volume(const Tetr &t);
  
  /// вычисление уравнения плоскости по трём точкам
  static glm::vec4 pleq(const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C);
  
  /// пересечение прямой заданной двумя точками и плоскости
  static bool cross(glm::vec3 &C, const glm::vec3 &M, const glm::vec3 &N, const glm::vec4 &P);
  
  /// Разложение вектора на сумму трёх векторов
  static bool as_summ(glm::vec3 &K, const glm::vec3 &D, const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C);
  
  /// Проверка на то, что точка внутри тетраэдра, включая грани и вершины
  static bool inside(const glm::vec3 &O, const glm::vec3 &A, const glm::vec3 &B, const glm::vec3 &C, const glm::vec3 &D);
  
  /// Проверка на то, что отрезок пересекает треугольник, но не лежит в его плоскости
  static bool is_crossing(glm::vec3 &c, const glm::vec3 &e0, const glm::vec3 &e1, const glm::vec3 &f0, const glm::vec3 &f1, const glm::vec3 &f2);
  
  template <typename T>
  static std::vector< std::vector<T> > combos(const unsigned n, const std::vector<T> &k)
  {
    std::vector< std::vector<T> > res;
    
    for ( std::vector<unsigned> s : subset(n,0,k.size()) ) {
      std::vector<T> t;
      for ( unsigned i = 0; i < n; ++i )
        t.push_back( k[s[i]] );
      res.push_back(t);
    }
    
    return res;
  }
  
};
