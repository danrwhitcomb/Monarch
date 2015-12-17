# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      # GN version: //chrome/monarch
      'target_name': 'chrome_monarch',
      'type': 'static_library',
      'dependencies': [
        '../base/base.gyp:base',
        '../url/url.gyp:url_lib',
        '../content/content.gyp:content_browser',
      ],
      'conditions': [],
      'sources': [
        'monarch/dynamic_app_manager.cc',
        'monarch/dynamic_app_manager.h'
      ],
    },
  ],
}
