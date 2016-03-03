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
  
    std::ifstream in(file.MaybeAsASCII().c_str());
    std::string result = ReplaceKeysWithWordsInFile(in, map);
    in.close();
  
    if(!result.empty()){
      std::ofstream out(file.MaybeAsASCII().c_str());
      
      if(out.fail())
        return false;
      
      out << result;
      out.close();
      
    } else {
      return false;
    }
  
  return true;
}

std::string ReplaceKeysWithWordsInFile(std::ifstream& in, std::map<std::string, std::string> map){
  
  if (in.fail())
  {
    return std::string();
  }
  
  std::string line;
  std::string result;
  typedef std::map<std::string, std::string>::iterator it_type;
  
  while (std::getline(in, line)){
  
    for(it_type iterator = map.begin(); iterator != map.end(); iterator++){
    
      //Run through all keys and see if they're in the line
      std::string wordToReplace = iterator->first;
      std::string wordToReplaceWith = iterator->second;
      size_t len = wordToReplace.length();
      
      //Replace every instance of this key in this line with its val
      while(true){
        size_t pos = line.find(wordToReplace);
        if (pos != std::string::npos)
          line.replace(pos, len, wordToReplaceWith);
        else
          break;
      }

    }
    
    result += line + "\n";
  }
  
  return result;
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

base::FilePath GetParentPath(const base::FilePath& path){
  base::FilePath new_path("/");
  std::vector<std::string> components;
  path.GetComponents(&components);
  
  for(size_t i=1; i < components.size() - 1; i++){
    new_path = new_path.Append(components[i]);
  }
  
  return new_path;
}

}
