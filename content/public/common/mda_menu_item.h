// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_COMMON_MDA_MENU_ITEM_H_
#define CONTENT_PUBLIC_COMMON_MDA_MENU_ITEM_H_

#include <string>
#include <vector>

namespace content{

struct MDAMenuItem {
  MDAMenuItem();
  ~MDAMenuItem();
  
  //Attributes
  std::string title;
  std::string action;
  bool enabled;
  
  std::vector<MDAMenuItem> children;
};

}

#endif //CONTENT_PUBLIC_COMMON_MDA_MENU_ITEM_H_
