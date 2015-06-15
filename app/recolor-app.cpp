#include "filesystem.hpp"

#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <iostream>
#include <fstream>

void process_image( const ColorTransition &recolor, const std::string &img_name )
{
  ILuint img_id = ilGenImage(); ilBindImage(img_id);
  std::cout << "il img id : " << img_id << std::endl;
  
  if ( ilLoadImage( (getBasePath()+img_name).c_str() ) ) {
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
  std::cout << "  bbp: " << (unsigned)img_info.Bpp << std::endl;
  std::cout << "  size: " << img_info.SizeOfData << std::endl;
  std::cout << "  format: " << img_info.Format << std::endl;
  std::cout << "  type: " << img_info.Type << std::endl;
  std::cout << "  origin: " << img_info.Origin << std::endl;
  
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
  
  ColorTransition::Image res_img = recolor.fromImage(orig_img);
  
  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      img_info.Data[offset+0] = 255.f * res_img[j][i].r;
      img_info.Data[offset+1] = 255.f * res_img[j][i].g;
      img_info.Data[offset+2] = 255.f * res_img[j][i].b;
      img_info.Data[offset+3] = 255.f * res_img[j][i].a;
    }
  }
  
  std::string output_fname = getAppDataPath() + "recolor/" + img_name;
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
  
  ilDeleteImage(img_id);
}

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();
  
  std::string pwd = "pwd";
  std::cout << pwd << " : " << exec(pwd);
  std::cout << "app data : " << getAppDataPath();
  std::cout << "resources : " << getBasePath();
  
  std::fstream fTransition( getBasePath() + "orange.txt", std::ios_base::in );
  std::vector<ColorTransition::Transition > transition;
  ColorTransition::Transition t;
  
  while (fTransition
         >> t.first.r >> t.first.g >> t.first.b
         >> t.second.r >> t.second.g >> t.second.b)
  {
    transition.push_back( t );
  }
  
  ColorTransition recolor = ColorTransition(transition);
  
  process_image( recolor, "icons.png" );
  process_image( recolor, "icons1.bmp" );
  process_image( recolor, "lowres.png" );
  process_image( recolor, "small.png" );
  
  return 0;
}
