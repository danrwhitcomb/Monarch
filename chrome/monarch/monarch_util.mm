//
//  monarch_util.cpp
//  sources_for_indexing
//
//  Created by Dan Whitcomb on 12/27/15.
//
//

#import <Foundation/Foundation.h>

#include "chrome/monarch/monarch_util.h"
#include "base/strings/string16.h"
#include "base/strings/sys_string_conversions.h"
#include "base/mac/foundation_util.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths_internal.h"

namespace monarch_app {

std::string String16ToString(base::string16& s){
  CFStringRef cf = base::SysUTF16ToCFStringRef(s);
  std::string string = base::SysCFStringRefToUTF8(cf);
  return string;
}

base::string16 StringToString16(std::string& s){
  CFStringRef cf = base::SysUTF8ToCFStringRef(s);
  base::string16 string = base::SysCFStringRefToUTF16(cf);
  return string;
}

bool ReplaceKeysInFile(std::map<std::string, std::string> map, base::FilePath& file){
  
  typedef std::map<std::string, std::string>::iterator it_type;
  for(it_type iterator = map.begin(); iterator != map.end(); iterator++){
    std::ifstream in(file.MaybeAsASCII().c_str());
    std::ofstream out(file.MaybeAsASCII().c_str(), std::ofstream::in | std::ofstream::out);
    ReplaceKeyWithWordInFile(in, out, iterator->first, iterator->second);
    in.close();
    out.close();
  }
  
  return true;
}

bool ReplaceKeyWithWordInFile(std::ifstream& in, std::ofstream& out, std::string wordToReplace, std::string wordToReplaceWith){
  
  if (in.fail())
  {
    return false;
  }
  
  if (out.fail())
  {
    return false;
  }
  
  std::string line;
  size_t len = wordToReplace.length();
  while (std::getline(in, line))
  {
    while (true)
    {
      size_t pos = line.find(wordToReplace);
      if (pos != std::string::npos)
        line.replace(pos, len, wordToReplaceWith);
      else
        break;
    }
    
    out << line << '\n';
  }
  
  return true;
}

base::FilePath GetTempAppDirectory(base::FilePath& profile_path){
  if(profile_path.empty())
    return base::FilePath();
  
  return profile_path.Append(chrome::kWebAppDirname);
}

base::FilePath GetTempExtDirectory(base::FilePath& profile_path){
  if(profile_path.empty())
    return base::FilePath();
  //TODO: Fix the hardcoding of this path.... Couldn't find the
  //right method/constant
  return profile_path.Append("Extensions/");
}

base::FilePath GetBaseExtPath(){
  return base::mac::PathForFrameworkBundleResource(
        base::mac::NSToCFCast([NSString stringWithUTF8String:chrome::kBaseExtName]));
}

}
