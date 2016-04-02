/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebMDAMenuElement_h
#define WebMDAMenuElement_h

#include "WebElement.h"

namespace blink {
  
  class HTMLMDAMenuElement;
  
  // A container for passing around a reference to a form element. Provides some
  // information about the form.
  class WebMDAMenuElement final : public WebElement {
  public:
    ~WebMDAMenuElement() { reset(); }
    
    WebMDAMenuElement() : WebElement() { }
    WebMDAMenuElement(const WebMDAMenuElement& element) : WebElement(element) { }
    
    WebMDAMenuElement& operator=(const WebMDAMenuElement& element)
    {
      WebElement::assign(element);
      return *this;
    }
    
    void assign(const WebMDAMenuElement& element) { WebElement::assign(element); }
    
    
    BLINK_EXPORT WebString title() const;
    
#if BLINK_IMPLEMENTATION
    WebMDAMenuElement(const PassRefPtrWillBeRawPtr<HTMLMDAMenuElement>&);
    WebMDAMenuElement& operator=(const PassRefPtrWillBeRawPtr<HTMLMDAMenuElement>&);
    operator PassRefPtrWillBeRawPtr<HTMLMDAMenuElement>() const;
#endif
  };
  
  DECLARE_WEB_NODE_TYPE_CASTS(WebMDAMenuElement);
  
  BLINK_EXPORT WebMDAMenuElement* toWebMDAMenuElement(WebElement*);
  // This returns 0 if the specified WebElement is not a WebInputElement.
  inline const WebMDAMenuElement* toWebMDAMenuElement(const WebElement* element)
  {
    return toWebMDAMenuElement(const_cast<WebElement*>(element));
  }
  
} // namespace blink

#endif
