#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <pugixml.hpp>

#include <chrono>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include "../recolor.hpp"


static ColorTransition *s_recolor;
static std::chrono::nanoseconds s_nanoseconds;
static unsigned long s_pixels;

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

  std::cout << "  dimensions : " << img_info.Width << "x" << img_info.Height << std::endl;
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

  auto t1 = std::chrono::high_resolution_clock::now();
  ColorTransition::Image res_img = s_recolor->fromImage(orig_img);
  std::chrono::nanoseconds t = std::chrono::high_resolution_clock::now() - t1;
  std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds> ( t ).count() << "ms" << std::endl;
  s_nanoseconds += t;
  s_pixels += img_info.Width * img_info.Height;

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

struct Process {

  std::string transition;
  std::string input_dir;
  std::string output_dir;

  virtual void make() {
    std::cout << "Input dir \"" << input_dir << "\"; Output dir \"" << output_dir << "\";" << std::endl;
  }
  
};

struct ProcessIMG : public Process {

  std::string input_file;
  std::string output_file;

  virtual void make() {
    std::cout << "Process IMG \"" << input_file << "\" -> \"" << output_file << "\"; ";
    Process::make();

    process_image(input_file, output_file);
  }

};

struct ProcessXML : public Process {
  std::string fname;
  std::string xpath, attr;

  ProcessXML( std::string fname ) : fname(fname) { }
  virtual void make() {
    std::cout << "Process XML \"" << fname << "\"; ";
    Process::make();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file( fname.c_str() );
    if ( result.status != pugi::status_ok ) {
      std::cerr << "Couldn't load xml \"" << fname << "\" : " << result.description() << std::endl;
      return;
    }

    pugi::xpath_node_set image_nodes = doc.select_nodes( xpath.c_str() );
    std::cout << image_nodes.size() << " images" << std::endl << std::endl;

    for ( const pugi::xpath_node &xnode : image_nodes ) {
      std::string img_fname = xnode.node().attribute( attr.c_str() ).value();
      std::cout << img_fname << std::endl;
      process_image( input_dir + "/" + img_fname, output_dir + "/" + img_fname );
      std::cout << std::endl;
    }
  }
};

ColorTransition *make_transition( const std::string &transition_fname )
{
  std::cerr << "Transition: \"" << transition_fname << "\"" << std::endl;
  
  std::fstream fTransition( transition_fname, std::ios_base::in );
  std::vector<ColorTransition::Transition > transition;
  ColorTransition::Transition t;

  while (fTransition
         >> t.first.r >> t.first.g >> t.first.b
         >> t.second.r >> t.second.g >> t.second.b)
  {
    transition.push_back( t );
  }

  try {
    return new ColorTransition(transition);
  }
  catch (const std::exception& e) {
    return nullptr;
  }
}

template<typename T>
inline T lexical_cast(const std::string& str) {
  T var;
  std::istringstream iss;
  iss.str(str);
  iss >> var;
  return var;
}

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();

  for ( int i = 0; i < argc; ++i ) {
    std::cerr << argv[i] << std::endl;
  }
  
  std::vector<Process *> proc_queue;
  std::string transition_fname;
  std::string input_dir, output_dir;
  std::string xml_xpath, xml_atrib;
  std::list<std::string> color_components;
  
  int c = 0;
  if ( ++c < argc ) {
    transition_fname = argv[c];
  } else {
    std::cerr << "usage examples :" << std::endl
    << "recolor-tool /Users/iKoznov/Developer/recolor/app/res/orange.txt -in /Users/iKoznov/Developer/tvip/tvip/themes/tvip_light/resources -out /Users/iKoznov/Desktop/res -xpath //image[@file] -xattr file -xml /Users/iKoznov/Developer/tvip/tvip/themes/tvip_light/resources.xml" << std::endl
    << "recolor-tool matrix.txt 0.7 0.1 0.9" << std::endl;
  }
  
  while ( ++c < argc ) {
    std::cerr << "ARG: " << argv[c] << std::endl;
    if ( std::strcmp( argv[c], "-img" ) == 0 ) {
      ProcessIMG *procIMG = new ProcessIMG();
      procIMG->input_file = argv[++c];
      procIMG->output_file = argv[++c];
      proc_queue.push_back( procIMG );
    }
    else if ( std::strcmp( argv[c], "-xml" ) == 0 ) {
      ProcessXML *procXML = new ProcessXML( argv[++c] );
      procXML->transition = transition_fname;
      procXML->input_dir = input_dir;
      procXML->output_dir = output_dir;
      procXML->xpath = xml_xpath;
      procXML->attr = xml_atrib;
      proc_queue.push_back( procXML );
    }
    else if ( std::strcmp( argv[c], "-xpath") == 0 ) {
      xml_xpath = argv[++c];
    }
    else if ( std::strcmp( argv[c], "-xattr") == 0 ) {
      xml_atrib = argv[++c];
    }
    else if ( std::strcmp( argv[c], "-in") == 0 ) {
      input_dir = argv[++c];
    }
    else if ( std::strcmp( argv[c], "-out") == 0 ) {
      output_dir = argv[++c];
    }
    else {
      color_components.push_back(std::string(argv[c]));
    }
  }

  s_recolor = make_transition( transition_fname );

  for ( Process *p : proc_queue ) {
    p->make();
  }
  
  if ( color_components.size() >= 3 ) {
    ColorTransition::rgb color;
    auto component = color_components.begin();
    for (int i = 0; i < 3; ++i) {
      color[i] = lexical_cast<float>(*component);
      component = ++component;
    }
    std::cerr << "color components: " << glm::to_string(color) << std::endl;
    
    ColorTransition::rgb result = s_recolor->fromColor(color);
    std::cout << result.r << " " << result.g << " " << result.b << std::endl;
  }

  delete  s_recolor;

  if ( !proc_queue.empty() ) {
    std::cout << "Total pixels: " << s_pixels << std::endl;
    std::cout << "Seconds: " << ( s_nanoseconds.count() / 1000000000.0 ) << std::endl;
    std::cout << "Avarage pixels per second: " << (double)s_pixels / ( (double)s_nanoseconds.count() / 1000000000.0 ) << std::endl;
  }
  
  return 0;
}
