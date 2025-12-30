#pragma once

#include "rpc/rpc_types.h"

namespace falcon::rpc {

class IRpcClient {
public:
    virtual ~IRpcClient() = default;

    virtual RpcResponse Call(const RpcRequest &request) = 0;
    virtual RpcFuture CallAsync(const RpcRequest &request) = 0;
    virtual void CallAsync(const RpcRequest &request, RpcCallback callback) = 0;
};

}  // namespace falcon::rpc
