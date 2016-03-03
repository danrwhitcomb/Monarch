//
//  monarch_util.hpp
//  sources_for_indexing
//
//  Created by Dan Whitcomb on 12/27/15.
//
//

#ifndef monarch_util_h
#define monarch_util_h

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

#include "base/strings/string16.h"
#include "base/files/file_path.h"

namespace monarch_app {

//Annoying conversions between UTF-8 and UTF-16
#if defined(OS_MACOSX)
std::string String16ToString(base::string16& s);
base::string16 StringToString16(std::string& s);
#endif

//Util functions to replace data in extension template.
//Should work with any file and key/val pairs though
bool ReplaceKeysInFile(std::map<std::string, std::string> map, base::FilePath& file);
std::string ReplaceKeysWithWordsInFile(std::ifstream& in, std::map<std::string, std::string> map);

//Standard path getters related to monarch
base::FilePath GetTempAppDirectory(base::FilePath& profile_path);
base::FilePath GetTempExtDirectory(base::FilePath& profile_path);
base::FilePath GetBaseExtPath();
base::FilePath GetProfilePath();

base::FilePath GetParentPath(const base::FilePath& path);

}

#endif /* monarch_util_h */
