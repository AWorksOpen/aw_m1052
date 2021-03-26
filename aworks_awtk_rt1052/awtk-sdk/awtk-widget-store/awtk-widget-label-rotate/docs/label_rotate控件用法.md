# label_rotate 控件

​	由于 AWTK 没有可以旋转的文本控件，所以 label_rotate 控件是为了解决该问题的，label_rotate 控件和普通的 label 控件的用法基本上是一样的。

​	其中在 UI 中只需要配置 orientation 属性即可以设置该旋转方向了。

```xml
  <view x="0" y="0" w="100%" h="100%" children_layout="default(c=2,r=2)">
    <label_rotate orientation="orientation_0" text="旋转0度"/>
    <label_rotate orientation="orientation_90" text="旋转90度"/>
    <label_rotate orientation="orientation_180" text="旋转180度"/>
    <label_rotate orientation="orientation_270" text="旋转270度"/>
  </view>
```

​	而代码中可以通过调用 label_rotate_set_orientation 函数来设置其选择方法。

```c
/**
* 文本选择方向枚举类型
**/
typedef enum _label_rotate_orientation_t {
  LABEL_ROTATE_ORIENTATION_0 = 0x0,
  LABEL_ROTATE_ORIENTATION_90,
  LABEL_ROTATE_ORIENTATION_180,
  LABEL_ROTATE_ORIENTATION_270,
} label_rotate_orientation_t;

/**
* @method label_rotate_set_orientation
* 设置文本选择方向。
* @annotation ["scriptable"]
* @param {widget_t*} widget 控件对象。
* @param {label_rotate_orientation_t}  orientation 旋转方向。
*
* @return {ret_t} 返回RET_OK表示成功，否则表示失败。
*/
ret_t label_rotate_set_orientation(widget_t *widget, label_rotate_orientation_t orientation);
```

**备注：**

​	使用的时候，需要在程序最开始通过调用 label_rotate_register 函数来注册使用，注意一定需要在打开窗口之前调用。

```c
/**
* @method label_rotate_register
* 注册可旋转文本控件。
*
* @annotation ["scriptable", "static"]
*
* @return {ret_t} 返回 RET_OK 表示成功，否则表示失败。
*/
ret_t label_rotate_register(void);
```

