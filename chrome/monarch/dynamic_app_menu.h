// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_MONARCH_DYNAMIC_APP_MENU_H_
#define CHROME_MONARCH_DYNAMIC_APP_MENU_H_

#include <string>
#include <vector>
#include <map>
#include "base/memory/scoped_ptr.h"
#include "content/public/common/mda_menu_item.h"

namespace monarch_app {

class DynamicAppMenu {
  public:
  
    static scoped_ptr<DynamicAppMenu> Create();
    static scoped_ptr<DynamicAppMenu> CreateWithMenu(const content::MDAMenuItem& root);
    ~DynamicAppMenu();

    content::MDAMenuItem GetRootItem();
    content::MDAMenuItem GetItem(std::string title);
    void ParseMenu(const content::MDAMenuItem& menu);
  
  private:
  
    bool HasItemWithTitle(std::string title);
  
    DynamicAppMenu();
    DynamicAppMenu(const content::MDAMenuItem& root);
  
    content::MDAMenuItem root_;
    std::map<std::string, content::MDAMenuItem> items_;  
};

}

#endif // CHROME_MONARCH_DYNAMIC_APP_MENU_H_
