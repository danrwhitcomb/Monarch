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
#include "base/memory/ref_counted.h"

#include "chrome/browser/web_applications/web_app_mac.h"
#include "base/memory/scoped_ptr.h"

namespace monarch_app {

class DynamicApp : public base::RefCountedThreadSafe<DynamicApp> {
  public:
  
    static scoped_refptr<DynamicApp> Create(scoped_ptr<web_app::ShortcutInfo> info, base::FilePath profile_path);
  
    DynamicApp(scoped_ptr<web_app::ShortcutInfo> info, base::FilePath profile_path);
  
    bool CopyBaseExtension();
    void SetupMockExtension();
  
    std::string GetExtensionID();
    std::string GetAppName();
    std::string GetPlainAppURL();
    base::FilePath GetExtensionPath();
    base::FilePath GetAppBundlePath();
    web_app::ShortcutInfo* GetShortcutInfo();
    bool CreateShortcut();
    void SetExtensionID(std::string extension_id);
    
  private:
    friend class base::RefCountedThreadSafe<DynamicApp>;
  
    scoped_ptr<web_app::ShortcutInfo> shortcut_info_;
    std::string extension_dir_;
    base::FilePath profile_path_;
  
    ~DynamicApp();
  
    bool ReplaceManifestData();
    bool ReplaceBackgroundJSData();
    bool ReplaceHTMLData();
};

}
#endif /* dynamic_app_hpp */
