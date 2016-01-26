// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
#define CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_

#include <stdio.h>
#include <map>

#include "chrome/browser/apps/app_shim/app_shim_host_observer.h"
#include "chrome/browser/apps/app_shim/app_shim_host_mac.h"
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
                          public apps::AppShimHostObserver {
  public:
    //Handles dynamically creating and destroying

    DynamicAppService(BrowserContext* context);
    void ShutdownOnUIThread() override;
  
    bool BuildAppFromTab(scoped_ptr<web_app::ShortcutInfo> shortcut_info);
    void DoUnpackedExtensionLoad(const base::FilePath& ext_path);
    void LaunchDynamicApp(const extensions::Extension* extension, const base::FilePath& file_path, const std::string& error);

  private:
    ~DynamicAppService() override;
    
    //AppShimHostObserver overrides
    void AppShimLaunched(AppShimHost* host) override;
    void AppShimClosed(AppShimHost* host) override;
  
    std::map<std::string, scoped_refptr<DynamicApp>> apps_;
    ExtensionService* extension_service_;
    BrowserContext* browser_context_;

};
  
} //namespace monarch

#endif // CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
