# Transport/RPC 抽象层设计

本文档描述 FalconFS 统一的传输/RPC 抽象层接口，避免上层直接依赖 brpc/protobuf attachment。

## 目标

- 为客户端与服务端提供与业务无关的统一接口。
- 统一请求/响应封装体与错误码语义。
- 支持同步/异步的统一调用方式。
- 便于替换 brpc 或接入其他传输实现。

## 目录与接口位置

- 头文件：`common/src/include/rpc/`
  - `rpc_types.h`
  - `irpc_channel.h`
  - `irpc_client.h`
  - `irpc_server.h`

## 请求/响应封装体

```cpp
struct RpcRequest {
    std::uint32_t opcode;
    Payload payload;           // bytes
    MetadataMap metadata;      // string->string
};

struct RpcResponse {
    RpcStatus status;
    Payload payload;
    MetadataMap metadata;
};
```

- `payload` 以 bytes 方式承载业务数据，上层可自行选择 protobuf/flatbuffers 等序列化方式。
- `metadata` 用于透传如 trace-id、auth token、route 标签等信息。

## 错误码与超时语义

错误码采用统一枚举 `RpcStatusCode`：

- `kOk`：成功
- `kInvalidArgument` / `kNotFound` / `kPermissionDenied` / `kUnauthenticated` 等用于上层业务语义
- `kDeadlineExceeded`：严格的调用截止时间超时
- `kTimeout`：连接/IO/传输超时
- `kUnavailable` / `kTransportError`：传输层不可用或连接异常
- `kInternal`：未知/内部错误

超时语义：

- `IRpcChannel::SetTimeout()` 统一设置调用默认超时。
- 单次请求如需覆盖，可通过 `RpcRequest::metadata` 传递自定义超时字段，由实现层解析。
- 当超时发生时，客户端应返回 `kDeadlineExceeded` 或 `kTimeout`（按实现定义明确映射）。

## 重试策略

`RetryPolicy` 描述重试配置：

- `max_retries`：最大重试次数
- `base_delay` / `max_delay`：指数退避参数
- `jitter`：抖动系数

由 `IRpcChannel` 持有和执行具体重试策略。

## 接口说明

### IRpcChannel

职责：建立/销毁连接、发送请求、超时设置、重试策略。

核心接口：

- `RpcStatus Connect(const std::string &endpoint)`
- `void Disconnect()`
- `RpcResponse Send(const RpcRequest &request)`
- `void SendAsync(const RpcRequest &request, RpcCallback callback)`
- `void SetTimeout(std::chrono::milliseconds timeout)`
- `void SetRetryPolicy(const RetryPolicy &policy)`

### IRpcClient

职责：业务调用入口，面向“请求/响应模型”。

- `RpcResponse Call(const RpcRequest &request)`
- `RpcFuture CallAsync(const RpcRequest &request)`
- `void CallAsync(const RpcRequest &request, RpcCallback callback)`

### IRpcServer

职责：注册 handler 并替代 `brpc::Server`。

- `void RegisterHandler(std::uint32_t opcode, RpcHandler handler)`
- `void RegisterAsyncHandler(std::uint32_t opcode, RpcAsyncHandler handler)`
- `RpcStatus Start(const std::string &endpoint)`
- `void Stop()`

## 同步/异步调用方式

- 同步：`Call()`/`Send()` 返回 `RpcResponse`。
- 异步：
  - 回调方式：`CallAsync(request, callback)`
  - Future 方式：`CallAsync(request)` 返回 `RpcFuture`

实现层可在内部选择线程池/事件循环，但对外暴露统一接口。

## 错误码映射建议

具体实现可将底层错误码统一映射到 `RpcStatusCode`：

- 连接建立失败：`kUnavailable` 或 `kTransportError`
- 连接中断：`kUnavailable`
- IO 超时：`kTimeout`
- 业务超时：`kDeadlineExceeded`
- 底层未知异常：`kInternal`

## 后续落地建议

1. 在 brpc 适配层中实现 `IRpcChannel/IRpcClient/IRpcServer`。
2. 在 `falcon_client` 中替换对 brpc 的直接依赖。
3. 对外暴露统一的 error code，使业务逻辑只处理 `RpcStatusCode`。
