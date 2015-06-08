#include "filesystem.hpp"

#include "recolor.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();
  
  std::string cmd = "pwd";
  BOOST_LOG_TRIVIAL(info) << boost::format("%s : %s") %cmd %exec(cmd);
  BOOST_LOG_TRIVIAL(info) << "app data : " << getAppDataPath();
  BOOST_LOG_TRIVIAL(info) << "resources : " << getBasePath();
  
  ILuint img_id = ilGenImage(); ilBindImage(img_id);
  BOOST_LOG_TRIVIAL(trace) << boost::format("il img id : %s") %img_id;
  
  std::string img_name = "lowres.png";
  if ( ilLoadImage( (getBasePath()+img_name).c_str() ) ) {
    BOOST_LOG_TRIVIAL(info) << "img loaded";
  }
  else {
    ILenum error;
    while ((error = ilGetError()) != IL_NO_ERROR) {
      BOOST_LOG_TRIVIAL(error) << "img not loaded : " << iluErrorString( error );
    }
    return 1;
  }
  
  ILinfo img_info;
  iluGetImageInfo(&img_info);
  
  BOOST_LOG_TRIVIAL(info) << boost::format("  dimensions : %sx%sx%s ") %img_info.Width %img_info.Height %img_info.Depth;
  BOOST_LOG_TRIVIAL(info) << boost::format("  bbp: %s") %(unsigned)img_info.Bpp;
  BOOST_LOG_TRIVIAL(info) << boost::format("  size: %s") %img_info.SizeOfData;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  format: %#06x") %img_info.Format;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  type: %#06x") %img_info.Type;
  BOOST_LOG_TRIVIAL(trace) << boost::format("  origin: %#06x") %img_info.Origin;
  
  std::vector< std::vector<glm::vec4> > colors(img_info.Height, std::vector<glm::vec4>(img_info.Width));
  for (ILuint j=0; j<img_info.Height; ++j) {
    for (ILuint i=0; i<img_info.Width; ++i) {
      ILuint offset = (j*img_info.Width+i)*img_info.Bpp;
      colors[j][i] = glm::vec4(img_info.Data[offset+0]/256.0f,
                               img_info.Data[offset+1]/256.0f,
                               img_info.Data[offset+2]/256.0f,
                               img_info.Data[offset+3]/256.0f);
      BOOST_LOG_TRIVIAL(trace) << boost::format("%s[%s:%s] %s") %(unsigned)offset %j %i %glm::to_string( colors[j][i] );
      img_info.Data[offset+0] = 255;
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
  
  return 0;
}
