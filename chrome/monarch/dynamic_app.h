// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef dynamic_app_h
#define dynamic_app_h

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
      content::WebContents* contents; //WebContents associated with app_shim. Not required
      GURL url; // url of app
      base::FilePath profile_path;
    };
    
     class Observer {
     public:
       
       virtual void OnMenuUpdated(Profile* profile, std::string& app_id, DynamicAppMenu* menu){}
       ~Observer(){}
     };
                     
  
    //Creaters
    static scoped_refptr<DynamicApp> Create(const DynamicAppParams& params);
  
    void SetMenu(scoped_ptr<DynamicAppMenu> menu);
  
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
    
     //Observer handlers
     void AddObserver(Observer* observer);
     void RemoveObserver(Observer* observer);
     
     //Notify observers
     void NotifyMenuChange();
    
    //WebContentsObserver
    void OnUpdateMDAMenu(const content::MDAMenuItem&) override;
  
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
