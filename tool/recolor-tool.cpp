#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <pugixml.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

static ColorTransition *s_recolor;

void process_image( const std::string &img_fname, const std::string &output_fname )
{
  ILuint img_id = ilGenImage(); ilBindImage(img_id);

  if ( ilLoadImage( (img_fname).c_str() ) ) {
    std::cout << "img loaded" << std::endl;
  }
  else {
    ILenum error;
    while ((error = ilGetError()) != IL_NO_ERROR) {
      std::cerr << "img not loaded : " << iluErrorString( error ) << std::endl;
    }
    return;
  }

  ILinfo img_info;
  iluGetImageInfo(&img_info);

  std::cout << "  dimensions : " << img_info.Width << "x" << img_info.Height << "x" << img_info.Depth << std::endl;
  std::cout << "  size: " << img_info.SizeOfData << "  bbp: " << (unsigned)img_info.Bpp << std::endl;

  ColorTransition::Image orig_img(img_info.Height, std::vector<glm::vec4>(img_info.Width));
  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      orig_img[j][i] = glm::vec4(img_info.Data[offset+0]/255.0f,
                                 img_info.Data[offset+1]/255.0f,
                                 img_info.Data[offset+2]/255.0f,
                                 img_info.Data[offset+3]/255.0f);
    }
  }

  ColorTransition::Image res_img = s_recolor->fromImage(orig_img);

  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      img_info.Data[offset+0] = 255.f * res_img[j][i].r;
      img_info.Data[offset+1] = 255.f * res_img[j][i].g;
      img_info.Data[offset+2] = 255.f * res_img[j][i].b;
      img_info.Data[offset+3] = 255.f * res_img[j][i].a;
    }
  }

#if 1
  std::remove( output_fname.c_str() );
  if ( ilSaveImage(output_fname.c_str()) ) {
    std::cout << "img saved : " << output_fname << std::endl;
  }
  else {
    ILenum error;
    while ((error = ilGetError()) != IL_NO_ERROR) {
      std::cerr << "img not saved : " << output_fname << " : " << iluErrorString( error ) << std::endl;
    }
  }
#endif

  ilDeleteImage(img_id);
}

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

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file( m_fname.c_str() );
    if ( result.status != pugi::status_ok ) {
      std::cerr << "Couldn't load xml \"" << m_fname << "\" : " << result.description() << std::endl;
      return;
    }

    pugi::xpath_node_set image_nodes = doc.select_nodes( "//image[@file]" );
    std::cout << image_nodes.size() << " images" << std::endl << std::endl;

    for ( const pugi::xpath_node &xnode : image_nodes ) {
      std::string img_fname = xnode.node().attribute("file").value();
      std::cout << img_fname << std::endl;
      process_image( input_dir() + "/" + img_fname, output_dir() + "/" + img_fname );
      std::cout << std::endl;
    }
  }
};

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();

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

  std::cout << "Transition: \"" << transition_fname << "\"" << std::endl;
  std::fstream fTransition( transition_fname, std::ios_base::in );
  std::vector<ColorTransition::Transition > transition;
  ColorTransition::Transition t;

  while (fTransition
         >> t.first.r >> t.first.g >> t.first.b
         >> t.second.r >> t.second.g >> t.second.b)
  {
    transition.push_back( t );
  }

  s_recolor = new ColorTransition(transition);

  for ( Process *p : proc_queue ) {
    p->make();
  }

  delete  s_recolor;

  return 0;
}
