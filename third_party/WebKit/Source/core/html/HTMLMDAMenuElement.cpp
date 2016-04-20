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
#include "core/html/HTMLMDAMenuElement.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"


namespace blink {
  
  using namespace HTMLNames;

  inline HTMLMDAMenuElement::HTMLMDAMenuElement(Document& document)
  : HTMLElement(mdamenuTag, document){}
  
  bool HTMLMDAMenuElement::isRootMenu() const {
    HTMLElement* parent = toHTMLElement(parentElement());
    return !isHTMLMDAMenuElement(parent);
  }
  
  bool HTMLMDAMenuElement::isAppMenu() const {
    return hasAttribute(appAttr);
  }
  
  HTMLMDAMenuElement* HTMLMDAMenuElement::getRootMenuElement(){
  
    //Check if current element is root
    if(isRootMenu()){
      return this;
    }
    
    HTMLMDAMenuElement* element = toHTMLMDAMenuElement(parentElement());
    
    while(!element->isRootMenu()){
      element = toHTMLMDAMenuElement(element->parentElement());
    }
    
    return element;
  }
  
  void HTMLMDAMenuElement::finishParsingChildren(){
    if(isRootMenu()){
      document().processMDAMenuElement(this);
    }
  }
  
  Node::InsertionNotificationRequest HTMLMDAMenuElement::insertedInto(ContainerNode* insertionPoint)
  {
    HTMLElement::insertedInto(insertionPoint);
    if (insertionPoint->inDocument() && isFinishedParsingChildren())
      document().processMDAMenuElement(this);
    return InsertionDone;
  }
  
  void HTMLMDAMenuElement::removedFrom(ContainerNode* insertionPoint)
  {
    HTMLElement::removedFrom(insertionPoint);
    if (insertionPoint->inDocument() && isFinishedParsingChildren())
      document().processMDAMenuElement(this);
  }
  
  void HTMLMDAMenuElement::childrenChanged(const ChildrenChange& change)
  {
    if(isFinishedParsingChildren()){
      HTMLElement::childrenChanged(change);
      document().processMDAMenuElement(this);
    }
  }

  DEFINE_NODE_FACTORY(HTMLMDAMenuElement);
}
