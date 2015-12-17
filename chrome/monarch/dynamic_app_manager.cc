// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/common/manifest_handlers/file_handler_info.h"
#include "chrome/common/chrome_paths_internal.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "content/public/browser/web_contents.h"
#include "base/memory/scoped_ptr.h"
#include "base/files/file_util.h"
#include "base/files/file.h"

#include "chrome/monarch/dynamic_app_manager.h"

namespace monarch_app {

using namespace content;

DynamicAppManager* instance_ = NULL;

void BuildDynamicApp(content::WebContents* contents){
  DynamicAppManager* manager = DynamicAppManager::GetDynamicAppManager();
  manager->BuildAppFromTab(contents);
}

DynamicAppManager* DynamicAppManager::GetDynamicAppManager(){
  if(!instance_){
    instance_ = new DynamicAppManager;
  }
  return instance_;
}

DynamicAppManager::DynamicAppManager(){}

//Need to add in deletion of shims when the app closes
DynamicAppManager::~DynamicAppManager(){}

bool DynamicAppManager::BuildAppFromTab(content::WebContents* web_contents){
  
  scoped_ptr<web_app::ShortcutInfo> shortcut_info = web_app::GetShortcutInfoForTab(web_contents);
  shortcut_info->profile_name = "dyn app";
  
  //Build new app directory
  //TODO: This needs to be updated to make random temporary directories
  //where users can't access install files easily
  base::FilePath user_data_dir;
  if(!chrome::GetUserDataDirectoryForBrowserBundle(chrome::OuterAppBundle(), &user_data_dir))
    return false;
  
  
  base::FilePath temp_app_home = FilePath("/Default/Web Applications/Temporary Applications/");

  std::string app_num = std::to_string(GetOpenAppNumber() + 1);
  base::FilePath temp_app_dir = user_data_dir.Append(temp_app_home.Append(FilePath(app_num)));
  
  File::Error error = File::Error::FILE_OK;
  if(!base::CreateDirectoryAndGetError(temp_app_dir, &error))
    return false;
  
  //Get FileHandlersInfo
  extensions::FileHandlersInfo file_handlers_info;
  
  scoped_ptr<web_app::WebAppShortcutCreator> shortcut_creator = CreateShortcutBuilder(temp_app_dir, shortcut_info.get(), file_handlers_info);
  
  web_app::ShortcutLocations shortcut_location;
  shortcut_location.on_desktop = false;
  shortcut_location.applications_menu_location = web_app::ApplicationsMenuLocation::APP_MENU_LOCATION_HIDDEN;
  shortcut_location.in_quick_launch_bar = true;
  
  
  shortcut_creator.get()->CreateShortcuts(web_app::ShortcutCreationReason::SHORTCUT_CREATION_AUTOMATED, shortcut_location);
  
  count_++;
  return true;
}

scoped_ptr<web_app::WebAppShortcutCreator> DynamicAppManager::CreateShortcutBuilder(const base::FilePath& app_data_dir,
                                                               const ShortcutInfo* shortcut_info,
                                                               const extensions::FileHandlersInfo& file_handlers_info){
    
    scoped_ptr<web_app::WebAppShortcutCreator> shortcut_builder(new web_app::WebAppShortcutCreator(app_data_dir, shortcut_info, file_handlers_info));
//    shortcut_builders_.push_back(shortcut_builder);
//    
    return std::move(shortcut_builder);
}

int DynamicAppManager::GetOpenAppNumber(){
  return count_;
}

} //namespace monarch




