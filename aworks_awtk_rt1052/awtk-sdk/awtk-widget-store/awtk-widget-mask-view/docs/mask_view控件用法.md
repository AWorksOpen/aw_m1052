# mask_view 控件用法

​	mask_view 控件主要是用于异性裁剪，用户提供蒙板就可以把需要的部分裁剪成蒙板的样子。

​	而 mask_view 控件主要分为两个部分，分别是 mask_view 控件和 mask_view_item 控件，其中主要是裁剪逻辑是 mask_view 控件负责，而 mask_view_item 控件用来标记对应子集是否为蒙板采样区还是背景区（绘图区），下面是 mask_view 控件用法。

```xml
<window >
  <mask_view x="c" y="m" w="120" h="96" fg_interval_time="1000" bg_interval_time="16" >
    <mask_view_item x="0" y="0" w="100%" h="100%" mask_view="true">
      <image_value x="0" y="0"  w="100%" h="100%" value="10" min="0" max="99" format="%02d" image="num_" animation="value(from=10, to=99, yoyo_times=0, duration=300000, easing=linear)"/>
    </mask_view_item>
    <mask_view_item x="0" y="0" w="100%" h="100%" mask_view="false">
      <canvas x="0" y="0" w="100%" h="100%" style:bg_color="black" />
    </mask_view_item>
  </mask_view>
</window>
```

## 一，mask_view_item 控件作用

​	mask_view_item 控件的作用主要是用于标记哪一个是蒙板区，哪一个是背景区（绘图区），其属性为 mask_view。

​	**如果 mask_view 值为 true ，则表示该 mask_view_item 控件为蒙板区，该控件的子集所有绘图都是绘制蒙板的**。

​	**如果 mask_view 值为 false，则表示该 mask_view_item 控件为背景区（绘图区），该控件的子集所有绘图都会被蒙板所裁剪，然后显出到屏幕上面**。

> 特别注意：
>
> 1. 蒙板区使用的所有素材必须是黑白图或者是灰度图，不能用其他颜色，否则裁剪效果无法估计。
> 2. 蒙板的素材中的黑白图后者灰度图的格式可是 rgb888，bgr888，rgb565 和 bgr565。（只需要保证颜色是灰黑白就可以了。）

## 二，mask_view 控件作用

​	mask_view 控件主要是作为一个裁剪的主要的逻辑部分，mask_view 控件有两个属性比较重要，如下表：

| 属性             | 作用                           |
| ---------------- | ------------------------------ |
| fg_interval_time | 蒙板区的采样时间间隔           |
| bg_interval_time | 背景区（绘图区）的采样时间间隔 |

> 备注：上面的两个属性主要是用于提高效率，蒙板区和背景区（绘图区）如果不是每一张都绘图的话，就可以属性来修改其采样时间间隔，减低其运算量，因为同一个蒙板或者同一个画面只需要计算一次嘛。