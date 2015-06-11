#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Process {
  
public:
  virtual void make() {
    std::cout << "Input dir \"" << input_dir() << "\"; Output dir \"" << output_dir() << "\";" << std::endl;
  }
  
private:
  std::string m_transition;
public:
  void set_transition( std::string fname ) { m_transition = fname; }
  std::string transition() { return m_transition; }
  
private:
  std::string m_input_dir;
public:
  void set_input_dir( std::string dir ) { m_input_dir = dir; }
  std::string input_dir() { return m_input_dir; }
  
private:
  std::string m_output_dir;
public:
  void set_output_dir( std::string dir ) { m_output_dir = dir; }
  std::string output_dir() { return m_output_dir; }
  
};

class ProcessXML : public Process {
  std::string m_fname;
public:
  ProcessXML( std::string fname ) : m_fname(fname) { }
  virtual void make() {
    std::cout << "Process XML \"" << m_fname << "\"; ";
    Process::make();
  }
};

int main (int argc, char *argv[])
{
  for ( int i = 0; i < argc; ++i ) {
    std::cout << argv[i] << std::endl;
  }
  
  std::vector<Process *> proc_queue;
  std::string transition_fname;
  std::string input_dir;
  std::string output_dir;
  
  int c = 0;
  if ( ++c < argc ) {
    transition_fname = argv[c];
  }
  
  while ( ++c < argc ) {
    std::cout << "ARG: " << argv[c] << std::endl;
    if ( std::strcmp( argv[c], "-xml" ) == 0 ) {
      ProcessXML *procXML = new ProcessXML( argv[++c] );
      procXML->set_transition( transition_fname );
      procXML->set_input_dir( input_dir );
      procXML->set_output_dir( output_dir );
      proc_queue.push_back( procXML );
    }
    else if ( std::strcmp( argv[c], "-in") == 0 ) {
      input_dir = argv[++c];
    }
    else if ( std::strcmp( argv[c], "-out") == 0 ) {
      output_dir = argv[++c];
    }
  }
  
  for ( Process *p : proc_queue ) {
    p->make();
  }
  
  return 0;
}
