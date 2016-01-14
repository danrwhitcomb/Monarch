 // Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#import <Foundation/Foundation.h>

#include "extensions/common/manifest_handlers/file_handler_info.h"
#include "chrome/monarch/dynamic_app.h"
#include "chrome/monarch/monarch_util.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/unpacked_installer.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/extensions/launch_util.h"
#include "chrome/browser/ui/extensions/application_launch.h"
#include "chrome/browser/ui/extensions/app_launch_params.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/browser_thread.h"
#include "base/memory/scoped_ptr.h"
#include "base/files/file_util.h"
#include "base/files/file.h"
#include "base/mac/foundation_util.h"
#include "components/crx_file/id_util.h"

#include "chrome/monarch/dynamic_app_manager.h"



namespace monarch_app {

using namespace content;
using content::BrowserThread;

DynamicAppManager* instance_ = NULL;

void DynamicAppManagerDeleteTraits::Destruct(const DynamicAppManager* x){
  DynamicAppManager::Destruct(x);
}

void BuildDynamicApp(scoped_ptr<web_app::ShortcutInfo> shortcut_info, ExtensionService* ext_ser){
  DynamicAppManager* manager = DynamicAppManager::GetDynamicAppManager(ext_ser);
  manager->BuildAppFromTab(std::move(shortcut_info), ext_ser);
}

DynamicAppManager* DynamicAppManager::GetDynamicAppManager(ExtensionService* extension_service){
  if(!instance_){
    instance_ = new DynamicAppManager(extension_service);
  }
  return instance_;
}

DynamicAppManager::DynamicAppManager(ExtensionService* extension_service):
  extension_service_(extension_service),
  browser_context_(extension_service->GetBrowserContext()){}

void DynamicAppManager::Destruct(const DynamicAppManager* x){
  delete x;
}

DynamicAppManager::~DynamicAppManager(){}

void DynamicAppManager::DoUnpackedExtensionLoad(const base::FilePath& ext_path){
  scoped_refptr<extensions::UnpackedInstaller> installer = extensions::UnpackedInstaller::Create(extension_service_);
  installer->set_completion_callback(base::Bind(&monarch_app::DynamicAppManager::LaunchDynamicApp, this));
  installer->Load(ext_path);
}

void DynamicAppManager::LaunchDynamicApp(const extensions::Extension* extension,
                        const base::FilePath& file_path,
                        const std::string& error){
  
  extensions::LaunchContainer launch_container = GetLaunchContainer(extensions::ExtensionPrefs::Get(browser_context_), extension);
  
  OpenApplication(AppLaunchParams(Profile::FromBrowserContext(browser_context_), extension, launch_container,
      NEW_FOREGROUND_TAB, extensions::SOURCE_MANAGEMENT_API));
}

bool DynamicAppManager::BuildAppFromTab(scoped_ptr<web_app::ShortcutInfo> shortcut_info, ExtensionService* ext_ser){
  
  //1. Get shortcut data to use in extension base
  shortcut_info->profile_name = "First User";
  std::string name = web_app::GenerateApplicationNameFromURL(shortcut_info->url);
  shortcut_info->title = StringToString16(name);
  
  //2. Build new app
  scoped_ptr<DynamicApp> app_ptr(new DynamicApp(std::move(shortcut_info)));
  
  //7. Copy base app directory to proper location
  base::FilePath ext_dir = app_ptr->GetExtensionPath();
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
  
  //8. Setup extension data
  app_ptr->SetExtensionID(crx_file::id_util::GenerateIdForPath(ext_dir.Append("1.0_0")));
  if(!app_ptr->SetupMockExtension()){
    return false;
  }
  
  base::FilePath app_ext_dir = ext_dir.Append("1.0_0");
  std::string ext_id = app_ptr->GetExtensionID();
  LoadExtension(app_ext_dir);

  //9. Store away for later
  apps_.insert(std::pair<std::string, scoped_ptr<DynamicApp>>(app_ptr->GetAppName(), std::move(app_ptr)));
  
  return true;
}

bool DynamicAppManager::LoadExtension(base::FilePath& ext_path){
  
  content::BrowserThread::PostTask(content::BrowserThread::UI,
                          FROM_HERE,
                          base::Bind(&monarch_app::DynamicAppManager::DoUnpackedExtensionLoad, this, ext_path));
  return true;
}


} //namespace monarch
