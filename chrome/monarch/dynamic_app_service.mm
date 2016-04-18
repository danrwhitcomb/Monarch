
 // Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#import <Foundation/Foundation.h>

#include "apps/app_lifetime_monitor.h"
#include "apps/app_lifetime_monitor_factory.h"
#include "base/memory/scoped_ptr.h"
#include "base/files/file_util.h"
#include "base/files/file.h"
#include "base/files/file_enumerator.h"
#include "base/mac/foundation_util.h"
#include "base/time/time.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/apps/app_shim/app_shim_handler_mac.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/web_applications/web_app.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/unpacked_installer.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/extensions/launch_util.h"
#include "chrome/browser/ui/extensions/application_launch.h"
#include "chrome/browser/ui/extensions/app_launch_params.h"
#include "chrome/browser/ui/simple_message_box.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/monarch/dynamic_app.h"
#include "chrome/monarch/dynamic_app_service_factory.h"
#include "chrome/monarch/monarch_util.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/site_instance.h"
#include "components/crx_file/id_util.h"
#include "extensions/browser/extension_host.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/process_manager.h"
#include "extensions/browser/uninstall_reason.h"
#include "extensions/common/manifest_handlers/file_handler_info.h"

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
  
  //Subscribe to process manager
  extensions::ProcessManager* manager = extensions::ProcessManager::Get(context);
  if(manager){
    manager->AddObserver(this);
  }
  
  content::BrowserThread::PostTask(content::BrowserThread::FILE,
  FROM_HERE, base::Bind(&monarch_app::DynamicAppService::CleanupOldApps, this));
}

//static
void DynamicAppService::LaunchAppWithURL(GURL& url, BrowserContext* context){
  monarch_app::DynamicAppServiceFactory::GetForContext(context)->
    BuildApp(url);
}

//static
void DynamicAppService::LaunchAppWithContents(WebContents* contents){
  
  //Might as well get all the info we can from web_contents
//  scoped_ptr<web_app::ShortcutInfo> info = web_app::GetShortcutInfoForTab(contents);
  scoped_refptr<monarch_app::DynamicAppService> service =
    monarch_app::DynamicAppServiceFactory::GetForContext(contents->GetBrowserContext());
  GURL url = contents->GetURL();
  service->BuildApp(url);
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
    
    std::string extension_id = extension->id();
    scoped_refptr<DynamicApp> app = apps_[extension_id];
  
    if(app){
      app->SetExtension(extension);
//     Launch app
      extensions::LaunchContainer launch_container =
      GetLaunchContainer(extensions::ExtensionPrefs::Get(browser_context_), extension);
      
      AppLaunchParams params =
        AppLaunchParams(Profile::FromBrowserContext(browser_context_), app->GetWebContents(), extension, launch_container,
            NEW_FOREGROUND_TAB,extensions::SOURCE_MANAGEMENT_API);
      
      OpenApplication(params);
      
      //Add temp file
      content::BrowserThread::PostTask(content::BrowserThread::FILE,
                                       FROM_HERE,
                                       base::Bind(&monarch_app::DynamicAppService::AddTempShortcutIndicator, this, extension));
    }
  }
  
}

bool DynamicAppService::BuildApp(GURL& url){
  DynamicApp::DynamicAppParams params;
  params.app_name = GenerateAppNameFromURL(url);
  params.url = url;
  params.profile_path = browser_context_->GetPath();
  
  scoped_refptr<DynamicApp> app_ptr = DynamicApp::Create(params);
  
  content::BrowserThread::PostTaskAndReply(content::BrowserThread::FILE, FROM_HERE,
      base::Bind(&monarch_app::DynamicApp::SetupMockExtension, app_ptr),
      base::Bind(&monarch_app::DynamicAppService::DoUnpackedExtensionLoad, this, app_ptr->GetExtensionPath()));
  
  //Save the DynamicApp
  apps_.insert(std::pair<std::string, scoped_refptr<DynamicApp>>(app_ptr->GetExtensionID(), std::move(app_ptr)));
  
  return true;
}

void DynamicAppService::OnExtensionFrameRegistered(const std::string&extension_id,
                                                   content::RenderFrameHost* render_frame_host){
  if(HasAppWithID(extension_id)){
    DynamicApp* app = GetAppWithID(extension_id);
    
    WebContents* contents = WebContents::FromRenderFrameHost(render_frame_host);
    if(contents && app){
      app->SetWebContents(contents);
    }
  }
}

void DynamicAppService::OnExtensionFrameUnregistered(const std::string& extension_id,
                                                       content::RenderFrameHost* render_frame_host){
  if(HasAppWithID(extension_id)){
    base::FilePath extension_path = apps_[extension_id]->GetExtensionPath();
    apps_.erase(extension_id);
    
    //Wait a few seconds to delete, else we might be destroying the wrong object
    UninstallApp(extension_id, extension_path);
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

void DynamicAppService::AddTempShortcutIndicator(const extensions::Extension* app){
  //Get path of shortcut directory
  base::FilePath prof_path = browser_context_->GetPath();
  base::FilePath shortcut_dir = GetTempAppDirectory(prof_path).Append("_crx_" + app->id());
  
  //Write an empty file to dir
  base::WriteFile(shortcut_dir.Append(".temp"), "", 0);
}

void DynamicAppService::CleanupOldApps(){
  base::FilePath prof_path = browser_context_->GetPath();
  base::FilePath temp_ext_dir = GetTempExtDirectory(prof_path);
  
  //Delete Extension Directory
  base::DeleteFile(temp_ext_dir, true);
  
  //Delete all shortcuts that have the .temp file in their directories
  base::FilePath shortcuts_dir = GetTempAppDirectory(prof_path);
  std::vector<base::FilePath> paths_to_delete;
  
  base::FileEnumerator enumer(shortcuts_dir, false, base::FileEnumerator:: DIRECTORIES);
 
  //Find all the paths to delete and save them so we don't delete in place
  for (base::FilePath dir = enumer.Next(); !dir.empty(); dir = enumer.Next()){
    if(base::PathExists(dir.Append(".temp"))){
      paths_to_delete.push_back(dir);
    }
  }
  
  //Delete them after finding them
  for(base::FilePath dir : paths_to_delete){
    base::DeleteFile(dir, true);
  }
}

void DynamicAppService::UninstallApp(const std::string& app_id, const base::FilePath& extension_path){

  //Uninstall the extesion: Should unload the extension and remove the app shim bundle
  //Since this is pretty significantly delayed from the closing of the actual app,
  //we have to make another check for the app in the map, in case the user has reopened
  //the same page
  if(apps_.find(app_id) == apps_.end()){
    
    ExtensionService::UninstallExtensionHelper(extension_service_, app_id, extensions::UninstallReason::UNINSTALL_REASON_COMPONENT_REMOVED);
    //Since the extension was originally unpacked, it won't delete the extension source directory. Do that here on the file thread.
    content::BrowserThread::PostTask(content::BrowserThread::FILE, FROM_HERE, base::Bind(
            &monarch_app::DynamicAppService::DeleteExtensionFiles, this, extension_path));
  }
}

void DynamicAppService::DeleteExtensionFiles(const base::FilePath& extension_path){
  //Had to make this func b/c PostTask was having problems calling it directly... I think parameters need to be const to be able to Post Task it
  base::DeleteFile(extension_path, true);
}

DynamicApp* DynamicAppService::GetAppWithID(const std::string& app_id){
  if(HasAppWithID(app_id))
    return apps_[app_id].get(); //Bet you can't guess what this does
  else
    return nullptr;
}

bool DynamicAppService::HasAppWithID(const std::string& app_id){
  return apps_.find(app_id) != apps_.end();
}


void DynamicAppService::ShowErrorForURL(GURL& url){
  std::string url_mesg("Unable to create a desktop app from the url: " + url.GetContent())
  ;
  
  chrome::ShowMessageBox(NULL, string16(ASCIIToUTF16("Unable to Load App")),
                         StringToString16(url_mesg),
                         chrome::MESSAGE_BOX_TYPE_WARNING);
}

DynamicApp* DynamicAppService::GetAppWithContents(WebContents* contents){
  
  for(std::map<std::string, scoped_refptr<DynamicApp>>::iterator it = apps_.begin(); it != apps_.end(); ++it){
    if (it->second->GetWebContents()->GetRoutingID() == contents->GetRoutingID()){
      return it->second.get();
    }
  }
  
  return nullptr;
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
