 // Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#import <Foundation/Foundation.h>

#include "apps/app_lifetime_monitor.h"
#include "apps/app_lifetime_monitor_factory.h"

#include "extensions/common/manifest_handlers/file_handler_info.h"
#include "chrome/monarch/dynamic_app.h"
#include "chrome/monarch/monarch_util.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/unpacked_installer.h"
#include "extensions/browser/extension_system.h"
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

#include "chrome/monarch/dynamic_app_service.h"



namespace monarch_app {

using namespace content;
using content::BrowserThread;


DynamicAppService::DynamicAppService(BrowserContext* context):
  extension_service_(extensions::ExtensionSystem::Get(context)->extension_service()),
  browser_context_(context){
  
  apps::AppLifetimeMonitor* monitor =
    apps::AppLifetimeMonitorFactory::
    GetForProfile(Profile::FromBrowserContext(context));
    
  monitor->AddObserver(this);
}

void DynamicAppService::ShutdownOnUIThread(){}

DynamicAppService::~DynamicAppService(){}

void DynamicAppService::DoUnpackedExtensionLoad(const base::FilePath& ext_path){
  scoped_refptr<extensions::UnpackedInstaller> installer =
    extensions::UnpackedInstaller::Create(extension_service_);
  
  installer->set_completion_callback(base::Bind(&monarch_app::DynamicAppService::LaunchDynamicApp, this));
  installer->Load(ext_path);
}

void DynamicAppService::LaunchDynamicApp(const extensions::Extension* extension,
                                         const base::FilePath& file_path,
                                         const std::string& error){
  
  extensions::LaunchContainer launch_container =
    GetLaunchContainer(extensions::ExtensionPrefs::Get(browser_context_), extension);
  
  OpenApplication(AppLaunchParams(Profile::FromBrowserContext(browser_context_),
                                  extension, launch_container,
                                  NEW_FOREGROUND_TAB, extensions::SOURCE_DYNAMIC_LAUNCH));
}

bool DynamicAppService::BuildAppFromTab(scoped_ptr<web_app::ShortcutInfo> shortcut_info){
  
  //1. Get shortcut data to use in extension base
  shortcut_info->profile_name = "First User";
  std::string name = web_app::GenerateApplicationNameFromURL(shortcut_info->url);
  shortcut_info->title = StringToString16(name);
  
  //2. Build new app
  scoped_refptr<DynamicApp> app_ptr =
    DynamicApp::Create(std::move(shortcut_info), browser_context_->GetPath());
  
  base::FilePath app_ext_dir = app_ptr->GetExtensionPath().Append("1.0_0");
  content::BrowserThread::PostTaskAndReply(content::BrowserThread::FILE, FROM_HERE,
                                           base::Bind(&monarch_app::DynamicApp::SetupMockExtension, app_ptr),
                                           base::Bind(&monarch_app::DynamicAppService::DoUnpackedExtensionLoad, this, app_ext_dir));

  //9. Store away for later
  apps_.insert(std::pair<std::string, scoped_refptr<DynamicApp>>(app_ptr->GetAppName(), std::move(app_ptr)));
  
  return true;
}

void DynamicAppService::OnAppStart(Profile* profile, const std::string& app_id){
  int i = 1;
  i += 1;
}


} //namespace monarch
