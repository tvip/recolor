#include "filesystem.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

int main (int argc, char *argv[])
{
  ilInit();
  iluInit();

  std::string cmd = "pwd";
  BOOST_LOG_TRIVIAL(info) << boost::format("%s : %s") %cmd %exec(cmd);
  BOOST_LOG_TRIVIAL(info) << boost::format("app data : %s") %getAppDataPath();
  BOOST_LOG_TRIVIAL(info) << boost::format("resources : %s") %getBasePath();

  if ( ilLoadImage( (getBasePath()+"icons.png").c_str() ) ) {
    BOOST_LOG_TRIVIAL(info) << "img loaded";
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "img not loaded";
    return 1;
  }

  return 0;
}
