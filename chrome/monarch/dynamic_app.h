//
//  dynamic_app.hpp
//  sources_for_indexing
//
//  Created by Dan Whitcomb on 12/20/15.
//
//

#ifndef dynamic_app_h
#define dynamic_app_h

#include <stdio.h>
#include <string>
#include "base/files/file_path.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "base/memory/scoped_ptr.h"

namespace monarch_app {

class DynamicApp {
  public:
  
    DynamicApp(scoped_ptr<web_app::ShortcutInfo> info);
    ~DynamicApp();
  
    std::string GetExtensionID();
    std::string GetAppName();
    std::string GetPlainAppURL();
    base::FilePath GetExtensionPath();
    base::FilePath GetAppBundlePath();
    web_app::ShortcutInfo* GetShortcutInfo();
    bool SetupMockExtension();
    bool CreateShortcut();
    void SetExtensionID(std::string extension_id);
  
  private:
    scoped_ptr<web_app::ShortcutInfo> shortcut_info_;
    std::string extension_dir_;
  
    bool ReplaceManifestData();
    bool ReplaceBackgroundJSData();
    bool ReplaceHTMLData();
};

}
#endif /* dynamic_app_hpp */
