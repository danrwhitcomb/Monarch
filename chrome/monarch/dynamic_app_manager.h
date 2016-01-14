// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
#define CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_

#include <stdio.h>
#include <map>

#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/browser/extensions/extension_service.h"
#include "chrome/browser/profiles/profile.h"
#include "extensions/browser/extension_prefs.h"
#include "extensions/common/extension.h"
#include "content/public/browser/web_contents.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/monarch/dynamic_app.h"

class Profile;

namespace extensions {
  class ExtensionPrefs;
}

namespace monarch_app {

using namespace base;
using namespace content;
using namespace web_app;

class DynamicAppManager;


struct DynamicAppManagerDeleteTraits {
  public:
    static void Destruct(const DynamicAppManager* x);
};

void BuildDynamicApp(scoped_ptr<web_app::ShortcutInfo> shortcut_info, ExtensionService* ext_ser);


class DynamicAppManager : public base::RefCountedThreadSafe<DynamicAppManager, DynamicAppManagerDeleteTraits> {
  public:
    //Handles dynamically creating and destroying
    //
    static DynamicAppManager* GetDynamicAppManager(ExtensionService* extension_service);
    static void Destruct(const DynamicAppManager* x);
  
    bool BuildAppFromTab(scoped_ptr<web_app::ShortcutInfo> shortcut_info, ExtensionService* ext_ser);
    void DoUnpackedExtensionLoad(const base::FilePath& ext_path);
    void LaunchDynamicApp(const extensions::Extension* extension,
                        const base::FilePath& file_path,
                        const std::string& error);

  private:
  
    DynamicAppManager(ExtensionService* extension_service);
    virtual ~DynamicAppManager();

    scoped_ptr<web_app::WebAppShortcutCreator> CreateShortcutBuilder(const base::FilePath& app_data_dir, const ShortcutInfo* shortcut_info, const extensions::FileHandlersInfo& file_handlers_info);
  
    web_app::ShortcutLocations GenerateShortcutLocations();
    bool LoadExtension(base::FilePath& ext_path);
  
    std::map<std::string, scoped_ptr<DynamicApp>> apps_;
    ExtensionService* extension_service_;
    BrowserContext* browser_context_;

};
  
} //namespace monarch

#endif // CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
