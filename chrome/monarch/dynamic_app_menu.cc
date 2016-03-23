// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "chrome/monarch/dynamic_app_menu.h"

namespace monarch_app {

  DynamicAppMenu::MenuItem::MenuItem()
    : enabled(true),
      parent(nullptr){}
  
  DynamicAppMenu::MenuItem::~MenuItem(){}
  
  scoped_ptr<DynamicAppMenu> DynamicAppMenu::Create(){
    scoped_ptr<DynamicAppMenu::MenuItem> rootItem(new DynamicAppMenu::MenuItem());
    scoped_ptr<DynamicAppMenu> menu(new DynamicAppMenu(std::move(rootItem)));
  
    return menu;
  }
  
  //Item handlers
  
  DynamicAppMenu::MenuItem* DynamicAppMenu::GetRootItem(){
    return root_;
  }
  
  DynamicAppMenu::MenuItem* DynamicAppMenu::GetItem(std::string title){
    if(HasItemWithTitle(title))
      return items_[title].get();
    else
      return nullptr;
  }
  
  void DynamicAppMenu::AddItemToParent(scoped_ptr<DynamicAppMenu::MenuItem> item, DynamicAppMenu::MenuItem* parent){
    if(!HasItemWithTitle(item->title)){
      parent->children.push_back(item.get());
      item->parent = parent;
    
      items_.insert(std::pair<std::string, scoped_ptr<DynamicAppMenu::MenuItem>>(item->title, std::move(item)));
    }
  }
  
  void DynamicAppMenu::DisableItem(std::string title){
    if(HasItemWithTitle(title))
      items_[title]->enabled = false;
  }
  
  void DynamicAppMenu::EnableItem(std::string title){
    if(HasItemWithTitle(title))
      items_[title]->enabled = true;
  }
  
  void DynamicAppMenu::RemoveItem(std::string title){
    MenuItem* item = GetItem(title);
    if(item){
      item->parent->children.erase(
        std::remove(item->parent->children.begin(),
                    item->parent->children.end(), item),
                    item->parent->children.end());
      
      items_.erase(title);
    }
  }
  
  //Private
  //Checks for items in lists
  bool DynamicAppMenu::HasItemWithTitle(std::string title){
    size_t count = items_.count(title);
    return count > 0 ? true : false;
  }

  //Notifying observers
  void DynamicAppMenu::NotifyMenuChange(DynamicAppMenu* menu){
    FOR_EACH_OBSERVER(Observer, observers_, OnMenuUpdated(menu));
  }
  
  void DynamicAppMenu::NotifyItemEnabled(MenuItem *item){
    FOR_EACH_OBSERVER(Observer, observers_, OnItemEnabled(item));
  }
  
  void DynamicAppMenu::NotifyItemDisabled(DynamicAppMenu::MenuItem* item){
    FOR_EACH_OBSERVER(Observer, observers_, OnItemDisabled(item));
  }
  
  //constructos/destructors
  DynamicAppMenu::DynamicAppMenu(scoped_ptr<MenuItem> root){
    items_.insert(std::pair<std::string, scoped_ptr<DynamicAppMenu::MenuItem>>(root->title, std::move(root)));
    root_ = root.get();
  }
  
  DynamicAppMenu::~DynamicAppMenu(){}
}
