/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2010 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#ifndef HTMLMDAMenuItemElement_h
#define HTMLMDAMenuItemElement_h

#include "core/html/HTMLElement.h"

namespace blink {
  
class HTMLMDAMenuItemElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
  public:
    DECLARE_NODE_FACTORY(HTMLMDAMenuItemElement);
    
    const String action() const;
      
  private:
    explicit HTMLMDAMenuItemElement(Document&);
    
    // Might want to keep these around for dynamics
//    InsertionNotificationRequest insertedInto(ContainerNode*) override;
//    void removedFrom(ContainerNode*) override;
    //    void childrenChanged(const ChildrenChange&) override;
  };
  
} // namespace blink

#endif // HTMLMDAMenuItemElement_h
