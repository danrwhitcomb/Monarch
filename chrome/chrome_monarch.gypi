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
        '../content/content.gyp:content_browser',
        '../content/content.gyp:content_common',
      ],
      'conditions': [],
      'sources': [
        'monarch/dynamic_app_service.mm',
        'monarch/dynamic_app_service.h',
        'monarch/dynamic_app.h',
        'monarch/dynamic_app.cc',
        'monarch/monarch_util.h',
        'monarch/monarch_util.mm',
        'monarch/dynamic_app_service_factory.h',
        'monarch/dynamic_app_service_factory.cc'
      ],
    },
    {
      # GN version: //chrome/monarch
      'target_name': 'chrome_monarch_base',
      'type': 'none',
      'dependencies': [],
      'conditions': [],
      'sources': [
        "../monarch/base_ext/1.0_0/main.js",
        "../monarch/base_ext/1.0_0/main.html",
        "../monarch/base_ext/1.0_0/manifest.json"
      ],
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)/base_ext',
          'files': [
            # Loader bundle for platform apps.
            '../monarch/base_ext',
          ],
        },
      ]
    }
  ]
}
