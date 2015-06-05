#include "filesystem.hpp"

#include <Foundation/Foundation.h>

std::string getBasePath(void)
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  
  /* this returns the exedir for non-bundled  and the resourceDir for bundled apps */
  const char *base = [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
  std::string retval(base);
  
  [pool release];
  return retval + "/";
}

std::string getAppDataPath()
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  std::string path;
  
  NSArray *dirs = [[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory
                                                         inDomains:NSUserDomainMask];
  
  if (dirs.count > 0) {
    path = std::string( [[dirs.firstObject path] fileSystemRepresentation] );
  }
  
  [pool release];
  return path;
}

std::string exec(std::string cmd)
{
  FILE* pipe = popen(cmd.c_str(), "r");
  
  if (!pipe)
    return "ERROR";
  
  char buffer[128];
  std::string result = "";
  
  while(!feof(pipe)) {
    if(fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }
  
  pclose(pipe);
  
  return result;
}
