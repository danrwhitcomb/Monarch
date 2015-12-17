// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
#define CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_

#include <stdio.h>
#include <vector>

#include "chrome/browser/web_applications/web_app_mac.h"
#include "content/public/browser/web_contents.h"
#include "base/memory/scoped_ptr.h"

namespace monarch_app {

using namespace base;
using namespace content;
using namespace web_app;

void BuildDynamicApp(content::WebContents* contents);
  
class DynamicAppManager {
  public:
    //Handles dynamically creating and destroying
    //
    static DynamicAppManager* GetDynamicAppManager();
    ~DynamicAppManager();
    bool BuildAppFromTab(WebContents* web_contents);

  private:
  
    int count_ = 0;

    DynamicAppManager();
    scoped_ptr<web_app::WebAppShortcutCreator> CreateShortcutBuilder(const base::FilePath& app_data_dir, const ShortcutInfo* shortcut_info, const extensions::FileHandlersInfo& file_handlers_info);
    int GetOpenAppNumber();

};
    
} //namespace monarch

#endif // CHROME_MONARCH_DYNAMIC_APP_MANAGER_H_
