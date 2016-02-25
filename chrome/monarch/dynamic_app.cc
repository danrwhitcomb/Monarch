//
//  dynamic_app.cpp
//  sources_for_indexing
//
//  Created by Dan Whitcomb on 12/20/15.
//
//
#include <string>
#include <map>
#include "base/files/file_path.h"
#include "base/files/file.h"
#include "base/files/file_util.h"
#include "content/public/browser/browser_thread.h"
#include "components/crx_file/id_util.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/monarch/dynamic_app_service.h"
#include "chrome/monarch/monarch_util.h"

#include "chrome/monarch/dynamic_app.h"

namespace monarch_app {

const std::string kAppName("%APP_NAME%");
const std::string kAppIcon16("%APP_ICON_16%");
const std::string kAppIcon128("%APP_ICON_128%");
const std::string kAppURL("%APP_URL%");
const std::string kAppID("%APP_ID%");


scoped_refptr<DynamicApp> DynamicApp::Create(scoped_ptr<ShortcutInfo> info, base::FilePath profile_path){
    return scoped_refptr<DynamicApp>(new DynamicApp(std::move(info), profile_path));
}

DynamicApp::DynamicApp(scoped_ptr<ShortcutInfo> info, base::FilePath profile_path):
    shortcut_info_(std::move(info)),
    profile_path_(profile_path)
{
  std::string name = String16ToString(shortcut_info_->title);
  extension_dir_ = crx_file::id_util::GenerateId(name);
  
  base::FilePath ext_dir = GetExtensionPath();
  shortcut_info_->extension_id = crx_file::id_util::GenerateIdForPath(ext_dir.Append("1.0_0"));
}

DynamicApp::~DynamicApp(){}

std::string DynamicApp::GetExtensionID(){
  return shortcut_info_->extension_id;
}

std::string DynamicApp::GetAppName(){
  return String16ToString(shortcut_info_->title);
}

base::FilePath DynamicApp::GetExtensionPath(){
  base::FilePath ext_path = GetTempExtDirectory(profile_path_);
  return ext_path.Append(extension_dir_);
}

base::FilePath DynamicApp::GetAppBundlePath(){
  return GetTempAppDirectory(profile_path_).Append("_crx_" + GetExtensionID());
}

std::string DynamicApp::GetPlainAppURL(){
  return shortcut_info_->url.spec();
}

GURL DynamicApp::GetURL(){
  return shortcut_info_->url;
}

web_app::ShortcutInfo* DynamicApp::GetShortcutInfo(){
  return shortcut_info_.get();
}

void DynamicApp::SetExtensionID(std::string extension_id){
  shortcut_info_->extension_id = extension_id;
}

bool DynamicApp::CopyBaseExtension(){
  base::FilePath ext_dir = GetExtensionPath();
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
  base::FilePath path = GetExtensionPath().Append("1.0_0").Append("background.js");
  if(!ReplaceKeysInFile(injections, path))
    return false;

  return true;
}

bool DynamicApp::ReplaceManifestData(){
  std::map<std::string, std::string> map;
  map[kAppName] = GetAppName();
  map[kAppURL] = GetPlainAppURL();
  
  base::FilePath path = GetExtensionPath().Append("1.0_0").Append("manifest.json");
  
  if(!ReplaceKeysInFile(map, path))
    return false;
  
  return true;
}

bool DynamicApp::ReplaceHTMLData(){
  std::map<std::string, std::string> map;
  map[kAppName] = GetAppName();
  map[kAppURL] = GetPlainAppURL();
  base::FilePath html_path = GetExtensionPath().Append("1.0_0").Append("main.html");
  
  if(!ReplaceKeysInFile(map, html_path))
    return false;
  
  return true;
}

bool DynamicApp::CreateShortcut(){

  extensions::FileHandlersInfo file_handlers_info;
  
  web_app::WebAppShortcutCreator shortcut_builder(GetAppBundlePath(), shortcut_info_.get(), file_handlers_info);
  
  web_app::ShortcutLocations shortcut_location;
  shortcut_location.on_desktop = false;
  shortcut_location.applications_menu_location = web_app::ApplicationsMenuLocation::APP_MENU_LOCATION_HIDDEN;
  shortcut_location.in_quick_launch_bar = true;
  
  if(!shortcut_builder.CreateShortcuts(web_app::ShortcutCreationReason::SHORTCUT_CREATION_AUTOMATED, shortcut_location))
    return false;
  
  return true;
}

}
