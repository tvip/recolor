#include "filesystem.hpp"

#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

void process_image( std::string img_name )
{
  ILuint img_id = ilGenImage(); ilBindImage(img_id);
  BOOST_LOG_TRIVIAL(trace) << boost::format("il img id : %s") %img_id;
  
  if ( ilLoadImage( (getBasePath()+img_name).c_str() ) ) {
    BOOST_LOG_TRIVIAL(info) << "img loaded";
  }
  else {
    ILenum error;
    while ((error = ilGetError()) != IL_NO_ERROR) {
      BOOST_LOG_TRIVIAL(error) << "img not loaded : " << iluErrorString( error );
    }
    return;
  }
  
  ILinfo img_info;
  iluGetImageInfo(&img_info);
  
  BOOST_LOG_TRIVIAL(info) << boost::format("  dimensions : %sx%sx%s ") %img_info.Width %img_info.Height %img_info.Depth;
  BOOST_LOG_TRIVIAL(info) << boost::format("  bbp: %s") %(unsigned)img_info.Bpp;
  BOOST_LOG_TRIVIAL(info) << boost::format("  size: %s") %img_info.SizeOfData;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  format: %#06x") %img_info.Format;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  type: %#06x") %img_info.Type;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  origin: %#06x") %img_info.Origin;
  
  ColorTransition::Image orig_img(img_info.Height, std::vector<glm::vec4>(img_info.Width));
  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      orig_img[j][i] = glm::vec4(img_info.Data[offset+0]/255.0f,
                                 img_info.Data[offset+1]/255.0f,
                                 img_info.Data[offset+2]/255.0f,
                                 img_info.Data[offset+3]/255.0f);
#if 0
      BOOST_LOG_TRIVIAL(trace) << boost::format("%s[%s:%s] %s") %(unsigned)offset %j %i %glm::to_string( colors[j][i] );
#endif
    }
  }
  
  std::vector<ColorTransition::Transition > transition;
  transition.push_back( ColorTransition::Transition(glm::vec3(0.33,0.8,0.95),glm::vec3(1,0.5,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(0,0.5,1),glm::vec3(1,0.5,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(0,0,0),glm::vec3(0,0,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(0,0,1),glm::vec3(1,0.5,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(0,1,0),glm::vec3(0,1,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(0,1,1),glm::vec3(0,0.3,0.3)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(1,0,0),glm::vec3(1,0,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(1,0,1),glm::vec3(1,0,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(1,1,0),glm::vec3(1,1,0)) );
  transition.push_back( ColorTransition::Transition(glm::vec3(1,1,1),glm::vec3(1,1,1)) );
  ColorTransition recolor(transition);
  
  ColorTransition::Image res_img = recolor.fromImage(orig_img);
  
  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      img_info.Data[offset+0] = 255.f * res_img[j][i].r;
      img_info.Data[offset+1] = 255.f * res_img[j][i].g;
      img_info.Data[offset+2] = 255.f * res_img[j][i].b;
      img_info.Data[offset+3] = 255.f * res_img[j][i].a;
#if 0
      BOOST_LOG_TRIVIAL(trace) << boost::format("%s[%s:%s] %s") %(unsigned)offset %j %i %glm::to_string( colors[j][i] );
#endif
    }
  }
  
  std::string output_fname = getAppDataPath() + "recolor/" + img_name;
  std::remove( output_fname.c_str() );
  if ( ilSaveImage(output_fname.c_str()) ) {
    BOOST_LOG_TRIVIAL(info) << "img saved : " << output_fname;
  }
  else {
    ILenum error;
    while ((error = ilGetError()) != IL_NO_ERROR) {
      BOOST_LOG_TRIVIAL(error) << boost::format("img not saved : %s : %s") %output_fname %iluErrorString( error );
    }
  }
  
  ilDeleteImage(img_id);
}

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();
  
  std::string cmd = "pwd";
  BOOST_LOG_TRIVIAL(info) << boost::format("%s : %s") %cmd %exec(cmd);
  BOOST_LOG_TRIVIAL(info) << "app data : " << getAppDataPath();
  BOOST_LOG_TRIVIAL(info) << "resources : " << getBasePath();
  
  process_image( "icons.png" );
  process_image( "icons1.bmp" );
  process_image( "lowres.png" );
  process_image( "small.png" );
  
  return 0;
}
