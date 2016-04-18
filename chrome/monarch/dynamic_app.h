// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_H_
#define CHROME_MONARCH_DYNAMIC_APP_H_

#include <stdio.h>
#include <string>

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/web_applications/web_app_mac.h"
#include "chrome/monarch/dynamic_app_menu.h"
#include "content/public/browser/web_contents_observer.h"
#include "extensions/common/extension.h"

namespace content {
  struct MDAMenuItem;
}

namespace monarch_app {

class DynamicApp : public base::RefCountedThreadSafe<DynamicApp>,
                   public content::WebContentsObserver {
  public:
  
    struct DynamicAppParams {
    
      DynamicAppParams();
      ~DynamicAppParams();
      
      std::string app_name; // Display name for the application
      GURL url; // url of app
      base::FilePath profile_path;
    };
    
     class Observer {
     public:
       
       virtual void OnMenuUpdated(DynamicAppMenu* menu){}
       ~Observer(){}
     };
                     
  
    //Creaters
    static scoped_refptr<DynamicApp> Create(const DynamicAppParams& params);
  
    DynamicAppMenu* GetMenu();
    void SetMenu(scoped_ptr<DynamicAppMenu> menu);
    void RefreshMenu();
    void NotifyMenuChange();
    void ExecuteActionForItem(std::string& title);
  
    //Copying over the extension
    bool CopyBaseExtension();
    void SetupMockExtension();
  
    //Getters
    std::string GetExtensionID();
    std::string GetAppName();
    GURL GetURL();
    content::WebContents* GetWebContents();
    extensions::Extension* GetExtension();
  
    std::string GetPlainAppURL();
  
    //Path getters
    base::FilePath GetExtensionPath();
    base::FilePath GetAppBundlePath();
    void SetExtensionID(std::string extension_id);
    
     //Observer handlers
     void AddObserver(Observer* observer);
     void RemoveObserver(Observer* observer);
     
     //Notify observers
    
    //WebContentsObserver
    void OnUpdateMDAMenu(const content::MDAMenuItem&) override;
    void DocumentLoadedInFrame(content::RenderFrameHost* render_frame_host) override;
  
  private:
    friend class base::RefCountedThreadSafe<DynamicApp>;
    friend class DynamicAppService;
  
    //constructors
    DynamicApp(const DynamicAppParams& params);
    void SetWebContents(content::WebContents* contents);
    void SetExtension(const extensions::Extension* extension);

  
    //Characteristic data
    std::string app_name_;
    std::string extension_id_;
    content::WebContents* contents_;
    extensions::Extension* extension_;
    GURL url_;
    base::FilePath extension_path_;
    base::FilePath profile_path_;
    scoped_ptr<DynamicAppMenu> menu_;
    
    //Observer
    base::ObserverList<Observer> observers_;
  
    ~DynamicApp() override;
  
    bool ReplaceManifestData();
    bool ReplaceBackgroundJSData();
    bool ReplaceHTMLData();
};

}
#endif /* dynamic_app_hpp */
