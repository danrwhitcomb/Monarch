// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MOJO_EDK_SYSTEM_TEST_UTILS_H_
#define MOJO_EDK_SYSTEM_TEST_UTILS_H_

#include "base/test/test_io_thread.h"
#include "base/time/time.h"
#include "mojo/edk/test/scoped_ipc_support.h"
#include "mojo/public/c/system/types.h"
#include "mojo/public/cpp/system/macros.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace mojo {
namespace edk {
namespace test {

MojoDeadline DeadlineFromMilliseconds(unsigned milliseconds);

// A timeout smaller than |TestTimeouts::tiny_timeout()|, as a |MojoDeadline|.
// Warning: This may lead to flakiness, but this is unavoidable if, e.g., you're
// trying to ensure that functions with timeouts are reasonably accurate. We
// want this to be as small as possible without causing too much flakiness.
MojoDeadline EpsilonDeadline();

// |TestTimeouts::tiny_timeout()|, as a |MojoDeadline|. (Expect this to be on
// the order of 100 ms.)
MojoDeadline TinyDeadline();

// |TestTimeouts::action_timeout()|, as a |MojoDeadline|. (Expect this to be on
// the order of 10 s.)
MojoDeadline ActionDeadline();

// Sleeps for at least the specified duration.
void Sleep(MojoDeadline deadline);

// Stopwatch -------------------------------------------------------------------

// A simple "stopwatch" for measuring time elapsed from a given starting point.
class Stopwatch {
 public:
  Stopwatch();
  ~Stopwatch();

  void Start();
  // Returns the amount of time elapsed since the last call to |Start()| (in
  // microseconds).
  MojoDeadline Elapsed();

 private:
  base::TimeTicks start_time_;

  MOJO_DISALLOW_COPY_AND_ASSIGN(Stopwatch);
};

// A base class which initializes and shuts down the necessary objects so that
// Mojo system calls can be made.
class MojoSystemTest : public testing::Test {
 public:
  MojoSystemTest();
  ~MojoSystemTest() override;

  base::TestIOThread* test_io_thread() { return &test_io_thread_; }

 private:
  base::MessageLoop message_loop_;
  base::TestIOThread test_io_thread_;
  test::ScopedIPCSupport ipc_support_;

  MOJO_DISALLOW_COPY_AND_ASSIGN(MojoSystemTest);
};

}  // namespace test
}  // namespace edk
}  // namespace mojo

#endif  // MOJO_EDK_SYSTEM_TEST_UTILS_H_
