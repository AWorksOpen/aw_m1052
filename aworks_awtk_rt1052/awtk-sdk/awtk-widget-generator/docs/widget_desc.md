
# 自定义控件描述文件

## 基本信息

自定义控件描述文件是一个JSON格式的文件，主要参数有：

* name 控件的类型名。全小写英文字母，单词之间用下划线连接。

* desc 功能描述。

* date 创建的日期。

* team 团队名称。

* author 作者联系方式。

* copyright 版权声明。

* includes 包含其它第三放头文件。

* props 属性集合。

```json
  "name":"draggable",
  "date":"2019-10-25",
  "team":"AWTK Develop Team",
  "author":"Li XianJing <xianjimli@hotmail.com>",
  "desc":"make parent widget or window draggable",
  "copyright":"Guangzhou ZHIYUAN Electronics Co.,Ltd.",
  "includes":[],
```

## 属性描述

* name 属性名。全小写英文字母，单词之间用下划线连接。

* desc 功能描述。

* init_value 初始值。

* type 属性类型。

```json
    {
      "name":"top",
      "desc":"拖动范围的顶部限制。缺省为父控件的顶部。",
      "init_value":-1,
      "type":"int32_t"
    },
```

完整示例请参考：samples
