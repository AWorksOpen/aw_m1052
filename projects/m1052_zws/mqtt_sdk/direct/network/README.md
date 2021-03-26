# 网络接口的抽象。

## 介绍

提供MQTT 和 HTTPS两种实现。

* HTTPS 用于登录并获取MQTT服务器相关信息以及下载固件。

* MQTT 用于设备上报数据和状态。

主要代码从[ws-iot-device-sdk-embedded-C](https://github.com/aws/aws-iot-device-sdk-embedded-C.git)移植而来。

## 移植

请参考：platform/linux

