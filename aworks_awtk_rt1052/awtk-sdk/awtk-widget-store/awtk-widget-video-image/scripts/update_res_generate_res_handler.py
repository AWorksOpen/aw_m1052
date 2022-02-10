#!/usr/bin/python

import os
import sys
import platform

# 序列帧的文件名格式
video_image_format="s%02d"

# 视频文件保存的颜色格式，需要对应 LCD 的类型
video_image_bitmap_format="RGBA8888"

# 序列帧的帧间延迟时间（每一帧的时间间隔，单位为毫秒）
video_image_delay=80

# 由于部分板子的贴图需要对齐，所以这里是需要设置 line_length 对齐模式，具体的请看文档
# video_image_line_length_model="0:16"

def getAwtkRoot(ctx):
  if 'awtk_root' in ctx:
    return ctx['awtk_root']
  else:
    import awtk_locator as locator
    locator.init()
    return locator.AWTK_ROOT

# 根据平台来自定选择 LCD 的类型，但是实际情况有可能不一样，所以用户可以自行修改下面的函数。
def updateVideoImageBitmapFormat(awtk_root):
  global video_image_bitmap_format

  sys.path.insert(0, awtk_root)
  import awtk_config as awtk
  # 这里的判读是为了兼容 awtk 的默认 LCD 类型而设计的，所以用户如果实际情况不一样，请手动修改。
  if os.environ['NANOVG_BACKEND'] == 'AGGE' or os.environ['NANOVG_BACKEND'] == 'AGG' or os.environ['VGCANVAS'] == 'CAIRO':
    video_image_bitmap_format="BGR565"

  print("video_image_bitmap_format:", video_image_bitmap_format);

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

def genVideoImageGen(src_dir):
  global video_image_line_length_model
  data_dir = joinPath(src_dir, 'data')
  if os.path.exists(data_dir):
    image_dir = joinPath(src_dir, 'images/video')
    if os.path.exists(image_dir):
      tool_path = os.path.abspath('bin/diff_image_to_video_gen')
      if platform.system() == "Windows" :
        tool_path += ".exe"
      if os.path.exists(tool_path):
        for video_image_dir in os.listdir(image_dir) :
          cmd = tool_path + " " + os.path.abspath(joinPath(image_dir, video_image_dir)) + " " + video_image_format + " " + joinPath(data_dir, video_image_dir)  + " " + video_image_bitmap_format + " " + str(video_image_delay)
          if "video_image_line_length_model" in globals().keys() :
            cmd += (" " + video_image_line_length_model)
          os.system(cmd)
      else :
        print(" not find diff_image_to_video_gen.exe")


def on_generate_res_before(ctx):
    print('======================= '+ ctx['type'] + ' =======================')
    # ctx = {'type': 'ui', 'theme': 'default', 'imagegen_options': 'mono', 'input': './', 'output': './'}
    if ctx['type'] == "data" :
        updateVideoImageBitmapFormat(getAwtkRoot(ctx))
        genVideoImageGen("design/default")


def on_generate_res_after(ctx):
    print('======================= '+ ctx['type'] + ' =======================')
