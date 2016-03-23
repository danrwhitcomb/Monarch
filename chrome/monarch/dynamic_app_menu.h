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
#include "chrome/common/mac/menu_item_dto.h"

namespace monarch_app {

class DynamicAppMenu {
  public:
  
    class MenuItem {
      public:
        MenuItem();
        ~MenuItem();
      
        //Attributes
        std::string title;
        std::string action;
        bool enabled;
      
      private:
        friend DynamicAppMenu;
        MenuItem* parent;
        std::vector<MenuItem*> children;
      
    };
  
    class Observer {
      public:
      
        virtual void OnMenuUpdated(DynamicAppMenu* menu){}
        virtual void OnItemEnabled(MenuItem* item){}
        virtual void OnItemDisabled(MenuItem* item){}
      
      private:
        ~Observer(){}
    };
  
    static scoped_ptr<DynamicAppMenu> Create();
    ~DynamicAppMenu();

    //Relation handlers
  
    //
    MenuItem* GetRootItem();
    MenuItem* GetItem(std::string title);

    //Items handlers
    void AddItemToParent(scoped_ptr<MenuItem> item, MenuItem* parent);
    void DisableItem(std::string title);
    void EnableItem(std::string title);
    void RemoveItem(std::string title);
  
    //Observer handlers
    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
  
  
  private:
  
    bool HasItemWithTitle(std::string title);
  
    void NotifyMenuChange(DynamicAppMenu* menu);
    void NotifyItemEnabled(MenuItem* item);
    void NotifyItemDisabled(MenuItem* item);
  
    DynamicAppMenu(scoped_ptr<MenuItem> root);

    MenuItem* root_;
    std::map<std::string, scoped_ptr<MenuItem>> items_;
    base::ObserverList<Observer> observers_;
  
  
};

}

#endif // CHROME_MONARCH_DYNAMIC_APP_MENU_H_
