// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#include "client_async_reader_helper.h"

//#include <cassert>     // for assert()
//#include <functional>  // for std::function
//
//#include <grpc_cb/impl/call_sptr.h>                              // for CallSptr
#include "client_reader_async_read_cqtag.h"  // for ClientReaderAsyncReadCqTag
//#include <grpc_cb/impl/client/client_reader_async_recv_status_cqtag.h>  // for ClientReaderAsyncRecvStatusCqTag
//#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
//#include <grpc_cb/status.h>                 // for Status
//#include <grpc_cb/status_callback.h>        // for StatusCallback
//#include <grpc_cb/support/config.h>      // for GRPC_FINAL

namespace grpc_cb {

ClientAsyncReaderHelper::ClientAsyncReaderHelper(
    CompletionQueueSptr cq_sptr, CallSptr call_sptr, Status& status,
    const ClientAsyncReadHandlerSptr& read_handler_sptr,
    const StatusCallback& on_status)
    : cq_sptr_(cq_sptr), call_sptr_(call_sptr), status_(status) {
  assert(cq_sptr);
  assert(call_sptr);
  // XXXX store handler and on_status
}

ClientAsyncReaderHelper::~ClientAsyncReaderHelper() {}

#if 0
// Callback on each message.
template <class Response>
inline void OnReadEach(const Response& msg,
    const ClientReaderDataSptr<Response>& data_sptr);

// Callback on end of reading or by error.
template <class Response>
inline void OnEnd(const Status& status,
    const ClientReaderDataSptr<Response>& data_sptr);

inline void AsyncRecvStatus(
    const CallSptr& call_sptr,
    Status& status,
    const StatusCallback& on_status);
#endif

// Setup next async read.
void ClientAsyncReaderHelper::AsyncReadNext() {
  // const ClientReaderDataSptr<Response>& data_sptr
  // assert(data_sptr);
  if (!status_.ok()) return;

  auto* tag = new ClientReaderAsyncReadCqTag(
      call_sptr_,
      // XXX [data_sptr](const Response& msg) { OnReadEach(msg, data_sptr); },
      [](const Status& status) { /* XXX OnEnd(status, data_sptr);*/});
  if (tag->Start()) return;

  delete tag;
  status_.SetInternalError("Failed to async read server stream.");
  if (on_status_) on_status_(status_);
}

#if 0
inline void AsyncRecvStatus(
    const CallSptr& call_sptr,
    Status& status,
    const StatusCallback& on_status) {
  assert(status.ok());

  auto* tag = new ClientReaderAsyncRecvStatusCqTag(call_sptr, on_status);
  if (tag->Start()) return;

  delete tag;
  status.SetInternalError("Failed to receive status.");
  if (on_status) on_status(status);
}

template <class Response>
inline void OnReadEach(const Response& msg,
    const ClientReaderDataSptr<Response>& data_sptr) {
  Status& status = data_sptr->status;
  assert(status.ok());

  std::function<void(const Response&)>& on_msg = data_sptr->on_msg;
  if (on_msg) on_msg(msg);

  AsyncReadNext(data_sptr);
  // Old tag will be deleted after return in BlockingRun().
}

template <class Response>
inline void OnEnd(const Status& status,
    const ClientReaderDataSptr<Response>& data_sptr) {
  StatusCallback& on_status = data_sptr->on_status;
  if (status.ok()) {
    AsyncRecvStatus(data_sptr->call_sptr,
        data_sptr->status, on_status);
    return;
  }

  if (on_status) on_status(status);
}
#endif

}  // namespace grpc_cb