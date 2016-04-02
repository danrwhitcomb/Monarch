// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MENU_H_
#define CHROME_MONARCH_DYNAMIC_APP_MENU_H_

#include <string>
#include <vector>
#include <map>
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "content/public/common/mda_menu_item.h"

namespace monarch_app {

class DynamicAppMenu {
  public:
  
    class Observer {
      public:
      
        virtual void OnMenuUpdated(DynamicAppMenu* menu){}
      
      private:
        ~Observer(){}
    };
  
    static scoped_ptr<DynamicAppMenu> Create();
    static scoped_ptr<DynamicAppMenu> CreateWithMenu(const content::MDAMenuItem& root);
    ~DynamicAppMenu();

    content::MDAMenuItem GetRootItem();
    content::MDAMenuItem GetItem(std::string title);
    void ParseMenu(const content::MDAMenuItem& menu);
  
    //Observer handlers
    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
  
    //Notify observers
    void NotifyMenuChange();
  
  private:
  
    bool HasItemWithTitle(std::string title);
  
    DynamicAppMenu();
    DynamicAppMenu(const content::MDAMenuItem& root);
  
    content::MDAMenuItem root_;
    std::map<std::string, content::MDAMenuItem> items_;
    base::ObserverList<Observer> observers_;
  
};

}

#endif // CHROME_MONARCH_DYNAMIC_APP_MENU_H_
