// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_MAC_MENU_ITEM_DTO_H_
#define CHROME_COMMON_MAC_MENU_ITEM_DTO_H_

#include <string>

namespace apps {
  
struct MenuDTO {
  
  MenuDTO(){}
  ~MenuDTO(){}
  
  std::string title;
  std::string parent;
  bool hasSubmenu;
};

}

#endif //CHROME_COMMON_MAC_MENU_ITEM_DTO_H_
