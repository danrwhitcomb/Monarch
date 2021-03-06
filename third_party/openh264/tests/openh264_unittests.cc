// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "testing/gtest/include/gtest/gtest.h"

namespace openh264 {

class OpenH264Tests : public testing::Test {
};

TEST_F(OpenH264Tests, DummyTestAlwaysPass) {
}

}  // namespace openh264

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
