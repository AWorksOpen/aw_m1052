# video_image 控件

​	在一般的嵌入式板子上面，解析 JPG 文件或者解析 PNG 文件速度是很慢的，尤其在播放高分辨率序列帧的时候尤为明显，这个时候就可以使用 video_image 控件来实现播放序列帧了。

​	video_image 控件是采用帧间差异的图像算法，把序列帧压缩为一个自定义的视频文件中，这种方法的逻辑是空间换时间的策略，虽然整体视频文件的大小会比 JPG 文件（PNG 文件）要大，但是会比位图文件要小很多的。

## 一，video_image 控件用法

​	video_image 控件的用法其实很简单，例如：

``` xml
<window anim_hint="htranslate" style:normal:bg_color="#00000000" >
  <video_image x="0" y="0" w="800" h="480" video_name="video_12" />
  <label x="c" y="m" w="200" h="100" visible="false" style:normal:font_size="80" style:normal:text_color="#ffffff" text="the end" />
</window>
```

​	video_image 控件的属性：

| 属性       | 作用                                                 |
| ---------- | ---------------------------------------------------- |
| video_name | 需要播放的视频文件名称（该文件是放在 data 文件夹中） |
| auto_play  | 是否自动播放（默认为自动播放）                       |
| delay_paly | 是否延迟播放（默认不延迟播放）                       |

​	在使用上，video_image 控件只需要配置 video_name 属性就可以播放序列帧了，所以是非常方便的，同时播放完成后，会触发 EVT_ANIM_END 事件，可以通过 widget_on  函数注册对应事件的回调函数。

> 备注：
>
> 1. 详细用法请看 docs 文件夹中的说明文件。
> 2. 视频资源格式默认为 BGR5656，如果是使用 opengl 模式的话，请重新编译资源，因为 opengl 的资源要求是 RGBA8888，编译资源文档请看 docs 文件夹中的说明文件。

## 二，运行例子

### 准备

1. 获取 awtk 并编译

```
git clone https://github.com/zlgopen/awtk.git
cd awtk; scons; cd -
```

### 运行

1. 生成示例代码的资源

```
python scripts/update_res.py all
```
> 也可以使用 Designer 打开项目，之后点击 “打包” 按钮进行生成；
> 如果资源发生修改，则需要重新生成资源。

如果 PIL 没有安装，执行上述脚本可能会出现如下错误：
```cmd
Traceback (most recent call last):
...
ModuleNotFoundError: No module named 'PIL'
```
请用 pip 安装：
```cmd
pip install Pillow
```

2. 编译

```
scons
```
> 注意：
> 编译前先确认 SConstruct 文件中的 awtk_root 是否为 awtk 所在目录，不是则修改。
> 默认使用动态库的形式，如果需要使用静态库，修改 SConstruct 文件中的 BUILD_SHARED = 'false' 即可。

3. 运行
```
./bin/demo
```

## 文档

[AWTK 自定义控件规范](https://github.com/zlgopen/awtk/blob/master/docs/custom_widget_rules.md)
