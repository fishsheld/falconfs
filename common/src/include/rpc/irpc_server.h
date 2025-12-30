#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "rpc/rpc_types.h"

namespace falcon::rpc {

using RpcHandler = std::function<RpcResponse(const RpcRequest &)>;
using RpcAsyncHandler = std::function<void(const RpcRequest &, RpcCallback)>;

class IRpcServer {
public:
    virtual ~IRpcServer() = default;

    virtual void RegisterHandler(std::uint32_t opcode, RpcHandler handler) = 0;
    virtual void RegisterAsyncHandler(std::uint32_t opcode, RpcAsyncHandler handler) = 0;

    virtual RpcStatus Start(const std::string &endpoint) = 0;
    virtual void Stop() = 0;
};

}  // namespace falcon::rpc
