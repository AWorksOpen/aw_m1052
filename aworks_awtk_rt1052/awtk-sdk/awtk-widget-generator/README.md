# 自定义控件代码生成器

## 安装 nodejs

本代码生成器依赖于 [nodejs](https://nodejs.org/zh-cn/)，需要先安装 [nodejs](https://nodejs.org/zh-cn/)。

然后通过在终端中执行如下命令以安装 fs-extra 模块。

```
npm install -g fs-extra
```

## 使用方法

1. 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

2. 获取 awtk-widget-generator

```
git clone https://github.com/zlgopen/awtk-widget-generator.git
cd awtk-widget-generator
```

3. 编写自定义控件的描述文件

请参考 [自定义控件的描述文件](docs/widget_desc.md)

4. 生成自定义控件

使用 index.js 脚本生成自定义控件的代码和工程，第1个参数必须的，表示自定义控件的描述文件。

> output_dir: 表示输出目录，如果不指定，则默认生成的文件会放到上一级目录。
>
> awtk_dir：表示awtk所在的目录，如果不指定，则默认为“../awtk”。

```
Usage: node index.js widget_desc.json [output_dir] [awtk_dir]
```

如：

```
node index.js samples/number_label.json
```

5. 完善生成的自定义控件

生成的控件代码只是一个框架，需要进一步完善才能工作。

请参考 [完善生成的自定义控件](docs/improve_generated_widget.md)
