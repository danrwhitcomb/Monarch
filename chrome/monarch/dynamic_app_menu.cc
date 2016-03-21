// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "chrome/monarch/dynamic_app_menu.h"

namespace monarch_app {
  DynamicAppMenu::MenuItem::MenuItem(){}
  DynamicAppMenu::MenuItem::~MenuItem(){}
  
  scoped_ptr<DynamicAppMenu> DynamicAppMenu::Create(std::string title, bool isRoot, DynamicAppMenu* parent){
    scoped_ptr<DynamicAppMenu> menu(new DynamicAppMenu(title, isRoot, parent));
    return menu;
  }
  
  //Item handlers
  
  DynamicAppMenu::MenuItem* DynamicAppMenu::GetItem(std::string title){
    int i = IndexOfItem(title);
    if(i == -1)
      return nullptr;
    
    return items_[i].get();
  }
  
  DynamicAppMenu::MenuItem* DynamicAppMenu::GetItemAtIndex(size_t index){
    if(index > items_.size() - 1)
      return nullptr;
    else
      return items_[index].get();
  }
  
  void DynamicAppMenu::AddItem(scoped_ptr<DynamicAppMenu::MenuItem> item){
    if(!HasItemWithTitle(item->title))
      items_.push_back(std::move(item));
  }
  
  void DynamicAppMenu::AddItemAtIndex(scoped_ptr<DynamicAppMenu::MenuItem> item, int index){
    if(!HasItemWithTitle(item->title))
      items_.insert(items_.begin() + index, std::move(item));
  }
  
  void DynamicAppMenu::DisableItem(std::string title){
    DynamicAppMenu::MenuItem* item = GetItem(title);
    if(item)
      item->enabled = false;
  }
  
  void DynamicAppMenu::EnableItem(std::string title){
    DynamicAppMenu::MenuItem* item = GetItem(title);
    if(item)
      item->enabled = true;
  }
  
  void DynamicAppMenu::RemoveItem(std::string title){
    int i = IndexOfItem(title);
    if(i != -1)
      items_.erase(items_.begin() + i - 1);
  }
  
  int DynamicAppMenu::IndexOfItem(std::string title){
    int i = 0;
    bool found = false;
    for(auto const &item : items_){
      if(item->title == title){
        found = true;
        break;
      }
      i++;
    }
    
    return found ? i : -1;
  }
  
  //Children handlers
  DynamicAppMenu* DynamicAppMenu::GetChildMenu(std::string title){
    int i = IndexOfMenu(title);
    return i == -1 ? nullptr : children_[i].get();
  }
  
  DynamicAppMenu* DynamicAppMenu::GetChildMenuAtIndex(size_t index){
    if(children_.size() > index - 1)
      return children_[index].get();
    else
      return nullptr;
  }
  
  void DynamicAppMenu::AddChild(scoped_ptr<DynamicAppMenu> child){
    if(!HasChildWithTitle(child->title()))
      children_.push_back(std::move(child));
  }
  
  void DynamicAppMenu::RemoveChild(std::string title){
    int i = IndexOfMenu(title);
    if(i != -1)
      children_.erase(children_.begin() + i - 1);
  }
  
  int DynamicAppMenu::IndexOfMenu(std::string title){
    int i = 0;
    bool found = false;
    for(auto const &menu : children_){
      if(menu->title() == title){
        found = true;
        break;
      }
      i++;
    }
    
    return found ? i : -1;
  }
  
  //Private
  //Checks for items in lists
  bool DynamicAppMenu::HasItemWithTitle(std::string title){
    int i = IndexOfItem(title);
    return i == -1 ? true : false;
  }
  
  bool DynamicAppMenu::HasChildWithTitle(std::string title){
    int i = IndexOfMenu(title);
    return i == -1 ? true : false;
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
  DynamicAppMenu::DynamicAppMenu(std::string title, bool isRoot, DynamicAppMenu* parent)
  {
    parent_ = !isRoot ? parent : nullptr;
    title_ = isRoot ? "root" : title;
  }
  
  DynamicAppMenu::~DynamicAppMenu(){}
}
