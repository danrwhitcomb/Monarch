// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef dynamic_app_service_factory_h
#define dynamic_app_service_factory_h

#include <stdio.h>
#include "components/keyed_service/content/refcounted_browser_context_keyed_service_factory.h"
#include "components/keyed_service/core/keyed_service.h"
#include "chrome/monarch/dynamic_app_service.h"

namespace monarch_app {

class DynamicAppServiceFactory : public RefcountedBrowserContextKeyedServiceFactory {
  public:
  
    static scoped_refptr<DynamicAppService> GetForContext(content::BrowserContext* context);
    static scoped_refptr<DynamicAppService> GetForRenderProcessId(int render_process_id);
    static DynamicAppServiceFactory* GetInstance();
  
    scoped_refptr<RefcountedKeyedService> BuildServiceInstanceFor(content::BrowserContext* context) const override;
  
  private:

    friend struct base::DefaultSingletonTraits<DynamicAppServiceFactory>;
    
    DynamicAppServiceFactory();
    ~DynamicAppServiceFactory() override;
  
    content::BrowserContext* GetBrowserContextToUse(content::BrowserContext* context) const override;
};

}

#endif /* dynamic_app_service_factory_h */
