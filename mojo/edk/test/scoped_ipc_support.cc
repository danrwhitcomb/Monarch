// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mojo/edk/test/scoped_ipc_support.h"

#include "base/message_loop/message_loop.h"
#include "mojo/edk/embedder/embedder.h"

namespace mojo {
namespace edk {
namespace test {

namespace internal {

ScopedIPCSupportHelper::ScopedIPCSupportHelper()
    : event_(true, false) {  // Manual reset.
}

ScopedIPCSupportHelper::~ScopedIPCSupportHelper() {
  if (base::MessageLoop::current() &&
      base::MessageLoop::current()->task_runner() == io_thread_task_runner_) {
    ShutdownIPCSupportOnIOThread();
  } else {
    ShutdownIPCSupportAndWaitForNoChannels();
    event_.Wait();
  }
}

void ScopedIPCSupportHelper::Init(
    ProcessDelegate* process_delegate,
    scoped_refptr<base::TaskRunner> io_thread_task_runner) {
  io_thread_task_runner_ = io_thread_task_runner;
  // Note: Run delegate methods on the I/O thread.
  InitIPCSupport(io_thread_task_runner_, process_delegate,
                 io_thread_task_runner_);
}

void ScopedIPCSupportHelper::OnShutdownCompleteImpl() {
  event_.Signal();
}

}  // namespace internal

ScopedIPCSupport::ScopedIPCSupport(
    scoped_refptr<base::TaskRunner> io_thread_task_runner) {
  helper_.Init(this, io_thread_task_runner.Pass());
}

ScopedIPCSupport::~ScopedIPCSupport() {
}

void ScopedIPCSupport::OnShutdownComplete() {
  helper_.OnShutdownCompleteImpl();
}

}  // namespace test
}  // namespace edk
}  // namespace mojo
