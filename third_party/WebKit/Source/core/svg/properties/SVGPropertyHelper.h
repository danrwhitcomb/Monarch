// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGPropertyHelper_h
#define SVGPropertyHelper_h

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/svg/properties/SVGProperty.h"

namespace blink {

template<typename Derived>
class SVGPropertyHelper : public SVGPropertyBase {
public:
    SVGPropertyHelper()
        : SVGPropertyBase(Derived::classType())
    {
    }

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String& value) const
    {
        RefPtrWillBeRawPtr<Derived> property = Derived::create();
        property->setValueAsString(value, IGNORE_EXCEPTION);
        return property.release();
    }
};

#define DEFINE_SVG_PROPERTY_TYPE_CASTS(thisType)\
    DEFINE_TYPE_CASTS(thisType, SVGPropertyBase, value, value->type() == thisType::classType(), value.type() == thisType::classType());\
    inline PassRefPtrWillBeRawPtr<thisType> to##thisType(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)\
    {\
        RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;\
        ASSERT(base->type() == thisType::classType());\
        return static_pointer_cast<thisType>(base.release());\
    }


} // namespace blink

#endif // SVGPropertyHelper_h
