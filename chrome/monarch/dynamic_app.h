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
#include "chrome/browser/profiles/profile.h"

#include "chrome/browser/web_applications/web_app_mac.h"
#include "base/memory/scoped_ptr.h"

namespace monarch_app {

class DynamicApp : public base::RefCountedThreadSafe<DynamicApp> {
  public:
  
    struct DynamicAppParams {
    
      DynamicAppParams();
      ~DynamicAppParams();
      
      std::string app_name; // Display name for the application
      content::WebContents* contents; //WebContents associated with app_shim. Not required
      GURL url; // url of app
      base::FilePath profile_path;
    };
  
    //Creaters
    static scoped_refptr<DynamicApp> Create(const DynamicAppParams& params);
  
    //Copying over the extension
    bool CopyBaseExtension();
    void SetupMockExtension();
  
    //Getters
    std::string GetExtensionID();
    std::string GetAppName();
    GURL GetURL();
    content::WebContents* GetWebContents();
  
    std::string GetPlainAppURL();
  
    //Path getters
    base::FilePath GetExtensionPath();
    base::FilePath GetAppBundlePath();
    void SetExtensionID(std::string extension_id);
  
  private:
    friend class base::RefCountedThreadSafe<DynamicApp>;
  
    //constructors
    DynamicApp(const DynamicAppParams& params);
  
    //Characteristic data
    std::string app_name_;
    std::string extension_id_;
    content::WebContents* contents_;
    GURL url_;
    base::FilePath extension_path_;
    base::FilePath profile_path_;
  
    ~DynamicApp();
  
    bool ReplaceManifestData();
    bool ReplaceBackgroundJSData();
    bool ReplaceHTMLData();
};

}
#endif /* dynamic_app_hpp */
