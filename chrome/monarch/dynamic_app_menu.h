// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MENU_H_
#define CHROME_MONARCH_DYNAMIC_APP_MENU_H_

#include <string>
#include <vector>
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"

namespace monarch_app {

class DynamicAppMenu {

  public:
  
    struct MenuItem {
      MenuItem();
      ~MenuItem();
      
      //Attributes
      std::string title;
      std::string parentTitle;
      std::string action;
      bool enabled;
    };
  
    class Observer {
      public:
      
        virtual void OnMenuUpdated(DynamicAppMenu* menu){}
        virtual void OnItemEnabled(MenuItem* item){}
        virtual void OnItemDisabled(MenuItem* item){}
      
      private:
        ~Observer(){}
    };
  
    static scoped_ptr<DynamicAppMenu> Create(std::string title, bool isRoot, DynamicAppMenu* parent);
    ~DynamicAppMenu();

    //Relation handlers
  
    //return nullptr if menu is root
    DynamicAppMenu* GetParentMenu();
    bool isRootMenu();

    //Items handlers
    MenuItem* GetItem(std::string title);
    MenuItem* GetItemAtIndex(size_t index);
    void AddItem(scoped_ptr<MenuItem> item);
    void AddItemAtIndex(scoped_ptr<MenuItem> item, int index);
    void DisableItem(std::string title);
    void EnableItem(std::string title);
    void RemoveItem(std::string title);
    //returns -1 if item is not found, 0...n otherwise
    int IndexOfItem(std::string title);
  
    //Children handlers
    DynamicAppMenu* GetChildMenu(std::string title);
    DynamicAppMenu* GetChildMenuAtIndex(size_t index);
    void AddChild(scoped_ptr<DynamicAppMenu> child);
    void RemoveChild(std::string title);
    int IndexOfMenu(std::string title);
  
    //Observer handlers
    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
  
    //Getters
    std::string title(){ return title_; }
    DynamicAppMenu* parent() { return parent_; }
  
  private:
  
    bool HasItemWithTitle(std::string title);
    bool HasChildWithTitle(std::string title);
  
    void NotifyMenuChange(DynamicAppMenu* menu);
    void NotifyItemEnabled(MenuItem* item);
    void NotifyItemDisabled(MenuItem* item);
  
    DynamicAppMenu(std::string title, bool isRoot, DynamicAppMenu* parent);
  
    DynamicAppMenu* parent_;
    std::vector<scoped_ptr<MenuItem>> items_;
    std::vector<scoped_ptr<DynamicAppMenu>> children_;
    std::string title_;
  
    base::ObserverList<Observer> observers_;
  
    DISALLOW_COPY_AND_ASSIGN(DynamicAppMenu);
};

}

#endif // CHROME_MONARCH_DYNAMIC_APP_MENU_H_
