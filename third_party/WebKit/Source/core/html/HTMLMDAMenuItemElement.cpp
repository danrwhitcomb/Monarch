/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
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
 */

#include "config.h"
#include "core/html/HTMLMDAMenuItemElement.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Text.h"
#include "wtf/text/StringBuilder.h"

namespace blink {
  
  using namespace HTMLNames;
  
  inline HTMLMDAMenuItemElement::HTMLMDAMenuItemElement(Document& document)
  : HTMLElement(mdamenuitemTag, document)
  {
  }
  
//  Node::InsertionNotificationRequest HTMLBaseElement::insertedInto(ContainerNode* insertionPoint)
//  {
//    HTMLElement::insertedInto(insertionPoint);
//    if (insertionPoint->inDocument())
//      document().processBaseElement();
//    return InsertionDone;
//  }
//  
//  void HTMLBaseElement::removedFrom(ContainerNode* insertionPoint)
//  {
//    HTMLElement::removedFrom(insertionPoint);
//    if (insertionPoint->inDocument())
//      document().processBaseElement();
//  }
//

  const String HTMLMDAMenuItemElement::action() const
  {
    return getAttribute(actionAttr);
  }
  
  DEFINE_NODE_FACTORY(HTMLMDAMenuItemElement);
  
}