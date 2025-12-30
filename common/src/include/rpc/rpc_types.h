#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <unordered_map>
#include <vector>

namespace falcon::rpc {

using MetadataMap = std::unordered_map<std::string, std::string>;
using Payload = std::vector<std::uint8_t>;

enum class RpcStatusCode : std::uint32_t {
    kOk = 0,
    kCancelled = 1,
    kUnknown = 2,
    kInvalidArgument = 3,
    kDeadlineExceeded = 4,
    kNotFound = 5,
    kAlreadyExists = 6,
    kPermissionDenied = 7,
    kUnauthenticated = 8,
    kResourceExhausted = 9,
    kFailedPrecondition = 10,
    kAborted = 11,
    kOutOfRange = 12,
    kUnimplemented = 13,
    kInternal = 14,
    kUnavailable = 15,
    kDataLoss = 16,
    kTimeout = 17,
    kTransportError = 18,
};

struct RpcStatus {
    RpcStatusCode code{RpcStatusCode::kOk};
    std::string message;

    bool ok() const { return code == RpcStatusCode::kOk; }
};

struct RetryPolicy {
    std::uint32_t max_retries{0};
    std::chrono::milliseconds base_delay{0};
    std::chrono::milliseconds max_delay{0};
    double jitter{0.0};
};

struct RpcRequest {
    std::uint32_t opcode{0};
    Payload payload;
    MetadataMap metadata;
};

struct RpcResponse {
    RpcStatus status;
    Payload payload;
    MetadataMap metadata;
};

using RpcCallback = std::function<void(const RpcResponse &)>;
using RpcFuture = std::future<RpcResponse>;

}  // namespace falcon::rpc
