# docker-compose 集群启动指南

## 运行环境
- **运行地点：** 请在宿主机（安装了 Docker Engine 与 docker compose 插件的主机）上执行本文档中的命令，而不是在任意容器内部。
- **原因：** docker compose 需要直接访问宿主机的 Docker 守护进程来拉起 9 个容器；在容器里运行会隔离网络与数据卷，导致部署不完整。

## 前置准备
1. 确认本地已经存在 Compose 文件所在的代码目录：
   ```bash
   git clone <falconfs 仓库地址>
   cd falconfs/cloud_native/deployment_script/
   ```
2. 根据需要提前创建宿主机数据目录（可选，示例以 `/mnt/falcon-data` 为根目录）：
   ```bash
   sudo mkdir -p /mnt/falcon-data/zk-{1..3}/data /mnt/falcon-data/zk-{1..3}/datalog
   sudo mkdir -p /mnt/falcon-data/cn-{1..3}/data
   sudo mkdir -p /mnt/falcon-data/dn-{1..3}/data
   sudo chown -R $USER:$USER /mnt/falcon-data
   ```

## 启动/管理命令
在 `cloud_native/deployment_script/` 目录中执行以下命令：

```bash
# 启动 3 个 ZooKeeper、3 个 CN、3 个 DN 容器
sudo docker compose up -d

# 查看运行状态
sudo docker compose ps

# 宿主机访问示例
# - ZooKeeper: localhost:2181/2182/2183
# - DN:        localhost:5432/5433/5434
# - CN:        localhost:5435/5436/5437

# 停止并清理容器
sudo docker compose down
```

> 如果希望查看实时日志，可执行 `sudo docker compose logs -f <服务名>`，例如 `sudo docker compose logs -f cn1`。

## 常见问题
- **镜像找不到：** 请先在宿主机上导入或 `docker load`/`docker pull` 对应的镜像，确保与 Compose 文件中指定的标签一致。
- **端口冲突或数据目录占用：** 调整 `docker-compose.yaml` 中的相关配置或修改宿主机数据目录再重新启动。

