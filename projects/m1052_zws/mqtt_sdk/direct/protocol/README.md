## 设备端与服务器之间通讯协议及设备端的参考代码

|   目录  | 说明    
| ------- |-------------
| common    | 设备端的代码
| protocol.md| 协议文档


### 一、移植步骤

1.实现设置访问接口。 参考simulator/settings.c

2.实现MQTT发送接口。 参考simulator/mqtt\_client.c

3.根据实际设备定义设备描述文件。参考 ../device-definition/samples

4.根据设备描述文件生成代码，并编写其中有TODO说明的部分的代码。

5.集成。把以上代码放到一起，与mqtt协议联调。参考simulator

### 二、单元测试

```
make test
```
