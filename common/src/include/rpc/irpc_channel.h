#pragma once

#include <chrono>
#include <string>

#include "rpc/rpc_types.h"

namespace falcon::rpc {

class IRpcChannel {
public:
    virtual ~IRpcChannel() = default;

    virtual RpcStatus Connect(const std::string &endpoint) = 0;
    virtual void Disconnect() = 0;

    virtual RpcResponse Send(const RpcRequest &request) = 0;
    virtual void SendAsync(const RpcRequest &request, RpcCallback callback) = 0;

    virtual void SetTimeout(std::chrono::milliseconds timeout) = 0;
    virtual std::chrono::milliseconds GetTimeout() const = 0;

    virtual void SetRetryPolicy(const RetryPolicy &policy) = 0;
    virtual RetryPolicy GetRetryPolicy() const = 0;
};

}  // namespace falcon::rpc
