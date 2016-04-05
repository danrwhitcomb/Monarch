// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "chrome/monarch/dynamic_app_menu.h"
#include "content/public/common/mda_menu_item.h"

namespace monarch_app {
  
  scoped_ptr<DynamicAppMenu> DynamicAppMenu::Create(){
    scoped_ptr<DynamicAppMenu> menu(new DynamicAppMenu());
    return menu;
  }
  
  scoped_ptr<DynamicAppMenu> DynamicAppMenu::CreateWithMenu(const content::MDAMenuItem& menu){
    return scoped_ptr<DynamicAppMenu>(new DynamicAppMenu(menu));
  }
  
  //Item handlers
  
  content::MDAMenuItem DynamicAppMenu::GetRootItem(){
    return root_;
  }
  
  content::MDAMenuItem DynamicAppMenu::GetItem(std::string title){
    if(HasItemWithTitle(title))
      return items_[title];
    else
      return content::MDAMenuItem();
  }
  
  void DynamicAppMenu::ParseMenu(const content::MDAMenuItem& menu){
    for(auto& child : menu.children){
      items_.insert(std::pair<std::string, content::MDAMenuItem>(child.title, child));
      ParseMenu(child);
    }
  }
  
  //Private
  //Checks for items in lists
  bool DynamicAppMenu::HasItemWithTitle(std::string title){
    size_t count = items_.count(title);
    return count > 0 ? true : false;
  }
  
  DynamicAppMenu::DynamicAppMenu(){}
  DynamicAppMenu::DynamicAppMenu(const content::MDAMenuItem& root){
    root_ = root;
    items_.insert(std::pair<std::string, content::MDAMenuItem>(root.title, root));
    ParseMenu(root);
  }
  
  DynamicAppMenu::~DynamicAppMenu(){}
}
