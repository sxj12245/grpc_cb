// Licensed under the Apache License, Version 2.0.
// Author: Jin Qing (http://blog.csdn.net/jq0123)

#ifndef GRPC_CB_CLIENT_CLIENT_SYNC_READER_H
#define GRPC_CB_CLIENT_CLIENT_SYNC_READER_H

#include <cassert>     // for assert()
#include <functional>  // for std::function

#include <grpc_cb/channel.h>                         // for MakeSharedCall()
#include <grpc_cb/impl/client/client_reader_data.h>  // for ClientReaderDataSptr
#include <grpc_cb/impl/client/client_reader_helper.h>  // for ClientReaderHelper
#include <grpc_cb/impl/client/client_reader_init_cqtag.h>  // for ClientReaderInitCqTag
#include <grpc_cb/status.h>                                // for Status
#include <grpc_cb/status_callback.h>                       // for StatusCallback

namespace grpc_cb {

// Copyable. Client sync reader.
template <class Response>
class ClientSyncReader GRPC_FINAL {
 public:
  // Todo: Also need to template request?
  inline ClientSyncReader(const ChannelSptr& channel, const std::string& method,
                      const ::google::protobuf::Message& request);

 public:
  inline bool BlockingReadOne(Response* response) const {
    assert(response);
    Data& d = *data_sptr_;
    return ClientReaderHelper::BlockingReadOne(d.call_sptr, d.cq_sptr,
                                               *response, d.status);
  }

  inline Status BlockingRecvStatus() const {
    const Data& d = *data_sptr_;
    return ClientReaderHelper::BlockingRecvStatus(d.call_sptr, d.cq_sptr);
  }

 private:
  // Wrap all data in shared struct pointer to make copy quick.
  using Data = ClientReaderData<Response>;
  using DataSptr = ClientReaderDataSptr<Response>;
  DataSptr data_sptr_;
};  // class ClientSyncReader<>

template <class Response>
ClientSyncReader<Response>::ClientSyncReader(
    const ChannelSptr& channel, const std::string& method,
    const ::google::protobuf::Message& request)
    : data_sptr_(new ClientReaderData<Response>) {
  assert(channel);
  CompletionQueueSptr cq_sptr(new CompletionQueue);
  CallSptr call_sptr = channel->MakeSharedCall(method, *cq_sptr);
  data_sptr_->cq_sptr = cq_sptr;
  data_sptr_->call_sptr = call_sptr;
  ClientReaderInitCqTag* tag = new ClientReaderInitCqTag(call_sptr);
  if (tag->Start(request)) return;
  delete tag;
  data_sptr_->status.SetInternalError("Failed to start client reader stream.");
}  // ClientSyncReader()

}  // namespace grpc_cb

#endif  // GRPC_CB_CLIENT_CLIENT_SYNC_READER_H