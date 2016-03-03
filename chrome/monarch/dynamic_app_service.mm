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


namespace content{
  class NavigationController;
}

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

  content::WebContents* web_contents(content::WebContents::Create(content::WebContents::CreateParams(context)));
  content::NavigationController::LoadURLParams params(url);
  web_contents->GetController().LoadURLWithParams(params);
  
  monarch_app::DynamicAppServiceFactory::GetForContext(context)->
    BuildAppFromContents(web_contents);
}


//static
void DynamicAppService::LaunchAppWithContents(WebContents* contents){
  
  //Might as well get all the info we can from web_contents
//  scoped_ptr<web_app::ShortcutInfo> info = web_app::GetShortcutInfoForTab(contents);
  scoped_refptr<monarch_app::DynamicAppService> service =
    monarch_app::DynamicAppServiceFactory::GetForContext(contents->GetBrowserContext());
  service->BuildAppFromContents(contents);
  //contents->Close();
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
    
    scoped_refptr<DynamicApp> app = apps_[extension->id()];
  
    //Launch app
    extensions::LaunchContainer launch_container =
    GetLaunchContainer(extensions::ExtensionPrefs::Get(browser_context_), extension);
    
    OpenApplication(AppLaunchParams(Profile::FromBrowserContext(browser_context_), app->GetWebContents(),
        extension, launch_container, NEW_FOREGROUND_TAB,
        extensions::SOURCE_MANAGEMENT_API));
  }
}

bool DynamicAppService::BuildAppFromContents(content::WebContents* contents){
  DynamicApp::DynamicAppParams params;
  params.app_name = GenerateAppNameFromURL(contents->GetURL());
  params.contents = contents;
  params.url = contents->GetURL();
  params.profile_path = browser_context_->GetPath();
  
  scoped_refptr<DynamicApp> app_ptr = DynamicApp::Create(params);
  
  content::BrowserThread::PostTaskAndReply(content::BrowserThread::FILE, FROM_HERE,
      base::Bind(&monarch_app::DynamicApp::SetupMockExtension, app_ptr),
      base::Bind(&monarch_app::DynamicAppService::DoUnpackedExtensionLoad, this, app_ptr->GetExtensionPath()));
  
  //Save the DynamicApp
  apps_.insert(std::pair<std::string, scoped_refptr<DynamicApp>>(app_ptr->GetExtensionID(), std::move(app_ptr)));
  
  return true;
}

//Probably useful in the future
void DynamicAppService::OnAppStart(Profile* profile, const std::string& app_id){}

void DynamicAppService::OnAppStop(Profile* profile, const std::string& app_id){

  //We only want to handle apps in our list
  if(apps_.find(app_id) != apps_.end()){
    base::FilePath extension_path = apps_[app_id]->GetExtensionPath();
    apps_.erase(app_id);
    
    //Wait a few seconds to delete, else we might be destroying the wrong object
    content::BrowserThread::PostDelayedTask(content::BrowserThread::UI,
          FROM_HERE, base::Bind(&monarch_app::DynamicAppService::UninstallApp,
                                this, app_id, extension_path), base::TimeDelta::FromSeconds(1));
  }
}

void DynamicAppService::OnChromeTerminating(){
  for(std::map<std::string, scoped_refptr<DynamicApp>>::iterator it = apps_.begin(); it != apps_.end();) {
  
    base::FilePath extension_path = it->second->GetExtensionPath();
    std::string app_id = it->second->GetExtensionID();
    apps_.erase(it++);
    
    if(base::PathExists(extension_path)){
      UninstallApp(app_id, extension_path);
    }
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
  //Had to make this func b/c PostTask was having problems calling it directly... I think parameters need to be const to be able to Post Task it
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

std::string DynamicAppService::GenerateAppNameFromURL(const GURL& url){

  std::string name;
  if(url.has_host()){
    std::string host = url.host();
    name += host.substr(0, host.find("."));
    name[0] = std::toupper(name[0]);
  } else {
    name = url.spec();
  }
  
  return name;
}


} //namespace monarch
