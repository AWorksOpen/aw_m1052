# video_image 控件用法

​	在一般的嵌入式板子上面，解析 JPG 文件或者解析 PNG 文件速度是很慢的，尤其在播放高分辨率序列帧的时候尤为明显，这个时候就可以使用 video_image 控件来实现播放序列帧了。

​	video_image 控件是采用帧间差异的图像算法，把序列帧压缩为一个自定义的视频文件中，这种方法的逻辑是空间换时间的策略，虽然整体视频文件的大小会比 JPG 文件（PNG 文件）要大，但是会比位图文件要小很多的。

### 一，video_image 控件一般用法

​	video_image 控件的用法其实很简单，例如：

``` xml
<window anim_hint="htranslate" style:normal:bg_color="#00000000" >
  <video_image x="0" y="0" w="800" h="480" video_name="video_12" />
  <label x="c" y="m" w="200" h="100" visible="false" style:normal:font_size="80" style:normal:text_color="#ff0000" text="the end" />
</window>
```

​	video_image 控件的属性：

| 属性       | 作用                                                 |
| ---------- | ---------------------------------------------------- |
| video_name | 需要播放的视频文件名称（该文件是放在 data 文件夹中） |
| auto_play  | 是否自动播放（默认为自动播放）                       |
| delay_paly | 是否延迟播放（默认不延迟播放）                       |

​	在使用上，video_image 控件只需要配置 video_name 属性就可以播放序列帧了，所以是非常方便的，同时播放完成后，会触发 EVT_ANIM_END 事件，可以通过 widget_on  函数注册对应事件的回调函数。

### 二，video_image 控件特殊用法

​	在上面 video_image 控件只是一般用法，把序列帧的数据保存到 bitmap 上面，然后再通过绘图方法把 bitmap 拷贝到 lcd 上面显示，由于一般情况下 video_image 控件的分辨率都比较大，把 bitmap 的数据拷贝到 lcd 上面显示效率会比较低，尤其是 video_image 控件用在开机动画的时候，效率问题尤为明显。

​	video_image 控件提供了一个直接把序列帧数据保存到 lcd 上面，这样子可以少了一个把 bitmap 拷贝到 lcd 上面的过程，提高效率。

​	由于 video_image 控件把序列帧的数据还是会保存到 bitmap 上面，所以我们需要把 lcd 的 buffer 作为 bitmap 的 buffer ，然后把该 bitmap 给到 video_image 控件中使用，video_image 控件控件就会把数据保存到这个 bitmap 来实现减少一次拷贝的过程。

支持该用法的函数列表：

| 函数                                | 作用                                                         |
| ----------------------------------- | ------------------------------------------------------------ |
| video_image_set_draw_video_image    | 设置是否自动绘制视频贴图数据                                 |
| video_image_set_init_bitmap_func    | 设置自定义初始化序列帧位图函数（如果返回非 RET_OK 的话，就会使用默认的方法创建位图。） |
| video_image_set_dispose_image_func  | 设置自定义释放序列帧位图函数                                 |
| video_image_set_draw_image_func     | 设置自定义绘制序列帧位图函数                                 |
| video_image_set_to_image_begin_func | 设置解码序列帧前回调函数                                     |
| video_image_set_to_image_end_func   | 设置解码序列帧后回调函数                                     |

用法：

```c
#include "lcd/lcd_mem.h"
#include "lcd/lcd_mem_special.h"

ret_t video_image_dispose_image(void* ctx, bitmap_t* bitmap) {
  bitmap->w = 0;
  bitmap->h = 0;
  bitmap->format = 0;
  bitmap->buffer = NULL;
  return RET_OK;
}
/* 这个是 pc 的 AGGE 模式下才是这样设置 offline buffer ，其他模式慎用 */
ret_t video_image_init_image(void* ctx, bitmap_t* init_bitmap, uint32_t w, uint32_t h, uint32_t channels, bitmap_format_t format) {
  canvas_t* c = widget_get_canvas(window_manager());
  lcd_t* lcd = c->lcd;
  lcd_mem_special_t* special = (lcd_mem_special_t*)lcd;
  bitmap_format_t lcd_format = lcd_get_desired_bitmap_format(lcd);
  uint32_t lcd_channels = bitmap_get_bpp_of_format(lcd_format);
  if (lcd->w != w || lcd->h != h || lcd_channels != channels || lcd_format != format) {
    assert(!" lcd->w != w || lcd->h != h || lcd_channels != channels || lcd_format != format ");
    return RET_FAIL;
  }
  /* 把 offline buffer 设置到 bitmap 上面，让序列帧的位图数据保持到 offline buffer 上面。 */
  init_bitmap->w = lcd->w;
  init_bitmap->h = lcd->h;
  init_bitmap->format = special->lcd_mem->format;
  init_bitmap->buffer = special->lcd_mem->offline_gb;
  graphic_buffer_attach(special->lcd_mem->offline_gb, special->lcd_mem->offline_fb, w, h);
  bitmap_set_line_length(init_bitmap, tk_max(lcd_channels * w, special->lcd_mem->line_length));

  return RET_OK;
}
ret_t application_init(void) {  
  widget_t* win = window_open("main");
  widget_t* video_image = widget_lookup_by_type(win, WIDGET_TYPE_VIDEO_IMAGE, TRUE);
  
  /* 由于会把数据直接绘制到 offline 上面了，所以 video_image 控件不需要调用绘制函数把 bitmap 绘制到 lcd 上面。 */
  video_image_set_draw_video_image(video_image, FALSE);
  /* 
   * 让视频数据直接画在 lcd 的 offline 上面，减少一次整屏的贴图的拷贝数据，极限提高效率的做法
   * 但是在初始化视频位图的时候可能会出现每个平台不一样的情况，所以必须要了解清楚才可以使用。
   * 还有更高效的拷贝到 lcd 的 olineline 上面，这个会有更多的限制，请慎用。
   */
  video_image_set_init_bitmap_func(video_image, video_image_init_image, NULL);
  /* 
   * 一定要在设置 video_name 之前设置 init_bitmap 函数，
   * 需要注意的是，如果 xml 有设置 video_name 的话，就需要在 video_image_set_video_name 函数后才可以设置 dispose_image 函数。
   * 因为在 video_image_set_video_name 会创建 bitmap，由于 xml 中有设置 video_name，在执行这段代码前就会创建了一个 bitmap了，
   * 所以需要通过原来默认的方法释放该 bitmap。
   * 如果想避免这个情况，可以不在 xml 中写入 video_name，通过代码来设置 video_name。
   */
  video_image_set_video_name(video_image, "video_12");
  video_image_set_dispose_image_func(video_image, video_image_dispose_image, NULL);
}
```

> 备注：这里的演示代码是基于 pc 的 AGGE 模式来写的，具体平台需要根据不同创建 LCD 的方法来写，要不然容易崩溃，使用这个方法虽然可以提高效率，但是用不好就容易导致程序画面异常和崩溃，所以请一定要搞清楚之后再使用。

### 三，序列帧压缩工具（diff_image_to_video_gen）

​	由于 video_image 控件只负责播放显示序列帧而已，真是把序列帧压缩是通过 diff_image_to_video_gen 工具来完成，该工具可以在 window 和 linux 上面运行。

​	diff_image_to_video_gen 工具的用法：diff_image_to_video_gen.exe [空格] image_dir [空格] image_name_format [空格] save_file_path [空格] frame_image_type [空格] delays

| 参数              | 说明                   | 备注                                             |
| ----------------- | ---------------------- | ------------------------------------------------ |
| image_dir         | 序列帧的文件夹路径     |                                                  |
| image_name_format | 序列帧的文件名格式     | 例如：本例子中 s01.jpg 文件，则文件名格式为 s%2d |
| save_file_path    | 视频文件保存路径       |                                                  |
| frame_image_type  | 视频文件保存的颜色格式 | 需要对应 LCD 的类型                              |
| delays            | 序列帧的帧间延迟时间   | 每一帧的时间间隔，单位为毫秒                     |

> 备注：
>
> 本例子的序列帧放在 .\design\default\images\video\video_12 文件夹中，序列帧的文件名格式为 s%2d。

​	而本例子中是统一通过调用 update_res.py 脚本或者 designer 来生成资源的，如下：

```cmd
python scripts/update_res.py all
```

​	同时需要注意的是，虽然是可以通过 update_res.py 脚本或者 designer 来生成资源，但是其生成资源的路径或者序列帧的文件名格式等参数都是被写死了，所以用户如果需要修改的话，请打开 .\scripts\update_res_generate_res_handler.py，如下根据注释来修改：

```python
# 序列帧的文件名格式
video_image_format="s%02d"

# 视频文件保存的颜色格式，需要对应 LCD 的类型
if os.environ['NANOVG_BACKEND'] == 'AGGE' or os.environ['NANOVG_BACKEND'] == 'AGG' or os.environ['VGCANVAS'] == 'CAIRO':
  video_image_bitmap_format="BGR565"
else :
  video_image_bitmap_format="RGBA8888"

# 序列帧的帧间延迟时间（每一帧的时间间隔，单位为毫秒）
video_image_delay=80

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

def genVideoImageGen(src_dir):
  data_dir = joinPath(src_dir, 'data')
  if os.path.exists(data_dir):
    image_dir = joinPath(src_dir, 'images/video')
    if os.path.exists(image_dir):
      tool_path = os.path.abspath('bin/diff_image_to_video_gen')
      if platform.system() == "Windows" :
        tool_path += ".exe"
      if os.path.exists(tool_path):
        for video_image_dir in os.listdir(image_dir) :
          os.system(tool_path + " " + os.path.abspath(joinPath(image_dir, video_image_dir)) + " " + video_image_format + " " + joinPath(data_dir, video_image_dir)  + " " + video_image_bitmap_format +" " + str(video_image_delay))
      else :
        print(" not find diff_image_to_video_gen.exe")
```