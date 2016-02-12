 // Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#import <Foundation/Foundation.h>

#include "apps/app_lifetime_monitor.h"
#include "apps/app_lifetime_monitor_factory.h"

#include "extensions/common/manifest_handlers/file_handler_info.h"
#include "chrome/monarch/dynamic_app.h"
#include "chrome/monarch/dynamic_app_service_factory.h"
#include "chrome/monarch/monarch_util.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/unpacked_installer.h"

#include "extensions/browser/extension_system.h"
#include "extensions/browser/uninstall_reason.h"

#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/extensions/launch_util.h"

#include "chrome/browser/ui/extensions/application_launch.h"
#include "chrome/browser/ui/extensions/app_launch_params.h"
#include "chrome/browser/ui/simple_message_box.h"

#include "content/public/browser/web_contents.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/site_instance.h"

#include "base/memory/scoped_ptr.h"
#include "base/files/file_util.h"
#include "base/files/file.h"
#include "base/mac/foundation_util.h"
#include "base/time/time.h"
#include "base/strings/utf_string_conversions.h"
#include "components/crx_file/id_util.h"

#include "chrome/monarch/dynamic_app_service.h"



namespace monarch_app {

using namespace content;
using content::BrowserThread;


DynamicAppService::DynamicAppService(BrowserContext* context):
  extension_service_(extensions::ExtensionSystem::Get(context)->extension_service()),
  browser_context_(context){
  
  //Subscribe to all app_shim updates
  apps::AppLifetimeMonitor* monitor =
    apps::AppLifetimeMonitorFactory::
    GetForProfile(Profile::FromBrowserContext(context));
    
  monitor->AddObserver(this);
}

//static
void DynamicAppService::LaunchAppWithURL(GURL& url, BrowserContext* context){
  //Make a fake shortcut_info b/c all we really need is the URL
  scoped_ptr<ShortcutInfo> info(new ShortcutInfo());
  info->url = url;
  monarch_app::DynamicAppServiceFactory::GetForContext(context)->
    BuildAppFromTab(std::move(info));
}


//static
void DynamicAppService::LaunchAppWithContents(WebContents* contents){
  
  //Might as well get all the info we can from web_contents
  scoped_ptr<web_app::ShortcutInfo> info = web_app::GetShortcutInfoForTab(contents);
  scoped_refptr<monarch_app::DynamicAppService> service =
    monarch_app::DynamicAppServiceFactory::GetForContext(contents->GetBrowserContext());
  service->BuildAppFromTab(std::move(info));
  contents->Close();
}


void DynamicAppService::ShutdownOnUIThread(){}

DynamicAppService::~DynamicAppService(){}

void DynamicAppService::DoUnpackedExtensionLoad(const base::FilePath& ext_path){
  //Since the files aren't packed as extensions, we can load it as an unpacked extensions: UnpackedInstaller makes this easy
  scoped_refptr<extensions::UnpackedInstaller> installer =
    extensions::UnpackedInstaller::Create(extension_service_);
  
  //When finished, we launch the app_shim
  installer->set_completion_callback(base::Bind(&monarch_app::DynamicAppService::LaunchDynamicApp, this));
  installer->Load(ext_path);
}

void DynamicAppService::LaunchDynamicApp(const extensions::Extension* extension,
                                         const base::FilePath& file_path,
                                         const std::string& error){
  if(error.empty()){
    extensions::LaunchContainer launch_container =
    GetLaunchContainer(extensions::ExtensionPrefs::Get(browser_context_), extension);
    
    OpenApplication(AppLaunchParams(Profile::FromBrowserContext(browser_context_),
                                    extension, launch_container,
                                    NEW_FOREGROUND_TAB, extensions::SOURCE_MANAGEMENT_API));
  }
}

bool DynamicAppService::BuildAppFromTab(scoped_ptr<web_app::ShortcutInfo> shortcut_info){
  
  //Shortcut info isn't required anywhere, but useful to hold necessary data
  
  if(!shortcut_info->url.is_valid()){
    ShowErrorForURL(shortcut_info->url);
    return false;
  }
  
  shortcut_info->profile_name = "First User";
  std::string name = web_app::GenerateApplicationNameFromURL(shortcut_info->url);
  shortcut_info->title = StringToString16(name);
  
  //Throw all app related info in its own class
  scoped_refptr<DynamicApp> app_ptr =
    DynamicApp::Create(std::move(shortcut_info), browser_context_->GetPath());
  base::FilePath app_ext_dir = app_ptr->GetExtensionPath().Append("1.0_0");
  
  //Move everything into place on the FILE thread, then install
  content::BrowserThread::PostTaskAndReply(content::BrowserThread::FILE, FROM_HERE,
                                           base::Bind(&monarch_app::DynamicApp::SetupMockExtension, app_ptr),
                                           base::Bind(&monarch_app::DynamicAppService::DoUnpackedExtensionLoad, this, app_ext_dir));

  //Save the DynamicApp
  apps_.insert(std::pair<std::string, scoped_refptr<DynamicApp>>(app_ptr->GetExtensionID(), std::move(app_ptr)));
  
  return true;
}

//Might need this in the future
void DynamicAppService::OnAppStart(Profile* profile, const std::string& app_id){}

void DynamicAppService::OnAppStop(Profile* profile, const std::string& app_id){

  //We only want to handle apps in our list
  if(apps_.find(app_id) != apps_.end()){
    base::FilePath extension_path = apps_[app_id]->GetExtensionPath();
    apps_.erase(app_id);
    
    //Wait a few seconds to delete, else we might be destroying the wrong object
    content::BrowserThread::PostDelayedTask(content::BrowserThread::UI,
          FROM_HERE, base::Bind(&monarch_app::DynamicAppService::UninstallApp,
                                this, app_id, extension_path), base::TimeDelta::FromSeconds(5));
  }
}

void DynamicAppService::UninstallApp(const std::string& app_id, const base::FilePath& extension_path){

  //Uninstall the extesion: Should unload the extension and remove the app shim bundle
  ExtensionService::UninstallExtensionHelper(extension_service_, app_id, extensions::UninstallReason::UNINSTALL_REASON_COMPONENT_REMOVED);
  
  //Since the extension was originally unpacked, it won't delete the extension source directory. Do that here on the file thread.
  content::BrowserThread::PostTask(content::BrowserThread::FILE, FROM_HERE, base::Bind(
          &monarch_app::DynamicAppService::DeleteExtensionFiles, this, extension_path));
}

void DynamicAppService::DeleteExtensionFiles(const base::FilePath& extension_path){
  //Had to make this func b/c PostTask was having problems calling it directly
  base::DeleteFile(extension_path, true);
}

DynamicApp* DynamicAppService::GetAppWithID(const std::string& app_id){
  return apps_[app_id].get(); //Bet you can't guess what this does
}

void DynamicAppService::ShowErrorForURL(GURL& url){
  std::string url_mesg("Unable to create a desktop app from the url: " + url.GetContent())
  ;
  
  chrome::ShowMessageBox(NULL, string16(ASCIIToUTF16("Unable to Load App")),
                         StringToString16(url_mesg),
                         chrome::MESSAGE_BOX_TYPE_WARNING);
}


} //namespace monarch
