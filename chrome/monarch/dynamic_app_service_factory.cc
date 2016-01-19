// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/monarch/dynamic_app_service_factory.h"

#include "chrome/browser/profiles/incognito_helpers.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_process_host.h"

namespace monarch_app {

// static
scoped_refptr<DynamicAppService> DynamicAppServiceFactory::GetForContext(content::BrowserContext* context) {
  return static_cast<DynamicAppService*>(GetInstance()->GetServiceForBrowserContext(context, true).get());

  //return static_cast<scoped_refptr<DynamicAppService>>(GetInstance()->GetServiceForBrowserContext(context, true).get());
}

// static
scoped_refptr<DynamicAppService> DynamicAppServiceFactory::GetForRenderProcessId(int render_process_id) {

  content::RenderProcessHost* host =
  content::RenderProcessHost::FromID(render_process_id);
  
  //Host doesn't exist
  if (!host)
    return NULL;
  
  content::BrowserContext* context = host->GetBrowserContext();
  
  //Context doesn't exist
  if (!context)
    return NULL;
  
  return GetForContext(context);
}

// static
DynamicAppServiceFactory* DynamicAppServiceFactory::GetInstance() {
  return base::Singleton<DynamicAppServiceFactory>::get();
}

scoped_refptr<RefcountedKeyedService> DynamicAppServiceFactory::BuildServiceInstanceFor(content::BrowserContext* context) const {

  RefcountedKeyedService* service = new DynamicAppService(context);

  return scoped_refptr<RefcountedKeyedService>(service);
}

DynamicAppServiceFactory::DynamicAppServiceFactory()
: RefcountedBrowserContextKeyedServiceFactory("DynamicAppService",
                                    BrowserContextDependencyManager::GetInstance()) {}

DynamicAppServiceFactory::~DynamicAppServiceFactory() {}


content::BrowserContext* DynamicAppServiceFactory::GetBrowserContextToUse(content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

}

