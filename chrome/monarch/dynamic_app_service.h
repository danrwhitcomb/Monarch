// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
#define CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_

#include <stdio.h>
#include <map>

#include "apps/app_lifetime_monitor.h"
#include "apps/app_lifetime_monitor_factory.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/monarch/dynamic_app.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/common/extension.h"
#include "content/public/browser/web_contents.h"
#include "base/memory/scoped_ptr.h"
#include "components/keyed_service/core/refcounted_keyed_service.h"

class Profile;

namespace extensions {
  class ExtensionPrefs;
}

namespace monarch_app {

using namespace base;
using namespace content;
using namespace web_app;

class DynamicAppService : public RefcountedKeyedService,
                          public apps::AppLifetimeMonitor::Observer {
  public:
    //Handles dynamically creating and destroying

    static void LaunchAppWithContents(WebContents* contents);
    static void LaunchAppWithURL(GURL& url, BrowserContext* context);
                            
    DynamicAppService(BrowserContext* context);
    void ShutdownOnUIThread() override;
  
    bool BuildAppFromContents(content::WebContents* contents);
    
    void LaunchDynamicApp(const extensions::Extension* extension,
                          const base::FilePath& file_path,
                          const std::string& error);
    
    DynamicApp* GetAppWithID(const std::string& app_id);
    void UninstallApp(const std::string& app_id,
                      const base::FilePath& extension_path);
                  
    
    //AppLifetimeMonitor Overrides
    void OnAppStart(Profile* profile, const std::string& app_id) override;
    void OnAppStop(Profile* profile, const std::string& app_id) override;
    void OnChromeTerminating() override;
    

  private:
  
    void DoUnpackedExtensionLoad(const base::FilePath& ext_path);
    void DeleteExtensionFiles(const base::FilePath& extension_path);
    void ShowErrorForURL(GURL& url);
    std::string GenerateAppNameFromURL(const GURL& url);
  
    ~DynamicAppService() override;
    std::map<std::string, scoped_refptr<DynamicApp>> apps_;
    ExtensionService* extension_service_;
    BrowserContext* browser_context_;
};
  
} //namespace monarch

#endif // CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
