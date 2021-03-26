
# 完善生成的代码

## 1. 完善控件的实现

根据自己的需要完善 src 目录下的程序。

> 关于自定义控件的更多说明，请参考 [自定义控件规范](../../awtk/docs/custom_widget_rules.md)

## 2. 完善测试程序

根据自己的需要完善 tests 目录下 xxx_test.cc。xxx 是实际控件的类型名。

## 3. 完善 demo 的控件 style

根据自己的需要完善 design/default/styles/main.xml

如二维码控件的 style 改为：

```xml
<qr>
  <style name="default" >
    <normal fg_color="black" bg_color="white"/>
    <scanned fg_color="gray" bg_color="#f0f0f0"/>
  </style>
</qr>
```
> 也可以使用 Designer 打开项目，之后打开该文件进行编辑。

## 4. 完善 demo 的 ui 文件

根据自己的需要完善 design/default/ui/main.xml

如二维码控件的 ui 改为：

```xml
<window theme="main" >
  <qr x="c" y="m" w="240" h="240" text="https://github.com/zlgopen/awtk" />
  <button x="c" y="b:30" w="80" h="30" name="close" text="Close"/>
</window>
```
> 也可以使用 Designer 打开项目，之后打开该文件进行编辑。

## 5. 完善 demo

根据自己的需要完善 demos/main.c

## 6. 编译运行

### 6.1 生成 demo 的资源

可以使用资源生成工具 update_res.py 生成资源，更多信息请参考 [资源生成工具](../template/scripts/README.md)

```
python scripts/update_res.py all
```
> 也可以使用 Designer 打开项目，之后点击 “打包” 按钮进行生成
> 注意：如果资源发生修改，需要重新生成资源。


### 6.2 编译

请参考 [编译选项](build_options.md)


### 6.3 运行

```
 ./bin/demo
```

## 7. 注意事项

* 修改依赖的 AWTK

如果需要依赖其他版本的 AWTK，则重新设置 AWTK 的路径即可。

AWTK 的路径由 scripts/awtk_locator.py 文件中的 locateAWTK 函数指定，一般只需修改函数内的 awtk_root。

比如新的 AWTK 路径为 C:/AWTK/SDK/awtk_agge，则可以将  scripts/awtk_locator.py 文件修改如下：

```python
...
    def getAwtkScriptsRoot():
        return os.path.join(locateAWTK('awtk_agge'), 'scripts')


    def getAwtkRoot():
        return locateAWTK('awtk_agge')


    def getAwtkLinuxFbRoot():
        return locateAWTK('awtk-linux-fb')


    def locateAWTK(awtk):
        awtk_root = 'C:/AWTK/SDK/' + awtk
        if not os.path.exists(awtk_root):
            dirnames = ['../'+awtk, '../../'+awtk, '../../../'+awtk]
            for dirname in dirnames:
                if os.path.exists(dirname):
                    awtk_root = dirname
                    break
        return os.path.abspath(awtk_root)
...
```

修改后，按第 6 章节的步骤重新编译运行。
