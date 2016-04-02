// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <map>

#include "base/files/file_path.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/monarch/dynamic_app_service.h"
#include "chrome/monarch/monarch_util.h"
#include "components/crx_file/id_util.h"
#include "content/common/frame_messages.h"
#include "content/public/browser/browser_thread.h"

#include "chrome/monarch/dynamic_app.h"

namespace monarch_app {

const std::string kAppName("%APP_NAME%");
const std::string kAppIcon16("%APP_ICON_16%");
const std::string kAppIcon128("%APP_ICON_128%");
const std::string kAppURL("%APP_URL%");
const std::string kAppID("%APP_ID%");

DynamicApp::DynamicAppParams::DynamicAppParams(){}
DynamicApp::DynamicAppParams::~DynamicAppParams(){}

scoped_refptr<DynamicApp> DynamicApp::Create(const DynamicAppParams& params){
    return scoped_refptr<DynamicApp>(new DynamicApp(params));
}

DynamicApp::DynamicApp(const DynamicAppParams& params):
  app_name_(params.app_name),
  contents_(params.contents),
  url_(params.url),
  profile_path_(params.profile_path){
  
    FilePath dir = GetTempExtDirectory(profile_path_).Append(crx_file::id_util::GenerateId(app_name_));
    extension_path_ = dir.Append("1.0_0");
    
    extension_id_ = crx_file::id_util::GenerateIdForPath(extension_path_);
    
    //Setup observer
    Observe(contents_);
}
  

DynamicApp::~DynamicApp(){
  if(contents_){
    delete contents_;
  }
}

void DynamicApp::SetMenu(scoped_ptr<DynamicAppMenu> menu){
  menu_.reset(menu.release());
  menu->NotifyMenuChange();
}

void DynamicApp::OnUpdateMDAMenu(const MDAMenuItem& menu){
  scoped_ptr<DynamicAppMenu> new_menu = DynamicAppMenu::CreateWithMenu(menu);
  SetMenu(std::move(new_menu));
}


//Getters
std::string DynamicApp::GetExtensionID(){ return extension_id_; }
std::string DynamicApp::GetAppName(){ return app_name_; }
content::WebContents* DynamicApp::GetWebContents(){ return contents_; }
GURL DynamicApp::GetURL(){ return url_; }


std::string DynamicApp::GetPlainAppURL(){ return url_.spec(); }


//Returns the path of where the app bundle should be
//it is not guarenteed to exist
base::FilePath DynamicApp::GetAppBundlePath(){
  return GetTempAppDirectory(profile_path_).Append("_crx_" + GetExtensionID()).Append(GetAppName() + ".app");
}

//Returns path of the installed extension
base::FilePath DynamicApp::GetExtensionPath(){
  return extension_path_;
}

void DynamicApp::SetExtensionID(std::string extension_id){ extension_id_ = extension_id; }

bool DynamicApp::CopyBaseExtension(){
  base::FilePath ext_dir = GetParentPath(GetExtensionPath());
  if(!base::DirectoryExists(ext_dir)){
    File::Error err = File::Error::FILE_OK;
    base::CreateDirectoryAndGetError(ext_dir, &err);
    
    if(err != File::Error::FILE_OK){
      return false;
    }
  }
  
  if(!base::CopyDirectory(GetBaseExtPath().Append("1.0_0"), ext_dir, true)){
    return false;
  }
  
  return true;
}

void DynamicApp::SetupMockExtension(){

  CopyBaseExtension();

  ReplaceHTMLData();
  ReplaceManifestData();
  ReplaceBackgroundJSData();
}


bool DynamicApp::ReplaceBackgroundJSData(){
  std::map<std::string, std::string> injections;
  injections[kAppID]  = GetExtensionID();
  base::FilePath path = GetExtensionPath().Append("background.js");
  if(!ReplaceKeysInFile(injections, path))
    return false;

  return true;
}

bool DynamicApp::ReplaceManifestData(){
  std::map<std::string, std::string> map;
  map[kAppName] = GetAppName();
  map[kAppURL] = GetPlainAppURL();
  
  base::FilePath path = GetExtensionPath().Append("manifest.json");
  
  if(!ReplaceKeysInFile(map, path))
    return false;
  
  return true;
}

bool DynamicApp::ReplaceHTMLData(){
  std::map<std::string, std::string> map;
  map[kAppName] = GetAppName();
  map[kAppURL] = GetPlainAppURL();
  base::FilePath html_path = GetExtensionPath().Append("main.html");
  
  if(!ReplaceKeysInFile(map, html_path))
    return false;
  
  return true;
}

}
