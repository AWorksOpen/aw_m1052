## image\_draw\_type\_t
### 概述
图片绘制方法常量定义。
### 常量
<p id="image_draw_type_t_consts">

| 名称 | 说明 | 
| -------- | ------- | 
| IMAGE\_DRAW\_DEFAULT | 缺省显示。将图片按原大小显示在目标矩形的左上角。 |
| IMAGE\_DRAW\_CENTER | 居中显示。将图片按原大小显示在目标矩形的中央。 |
| IMAGE\_DRAW\_ICON | 图标显示。同居中显示，但会根据屏幕密度调整大小。 |
| IMAGE\_DRAW\_SCALE | 缩放显示。将图片缩放至目标矩形的大小(不保证宽高成比例)。 |
| IMAGE\_DRAW\_SCALE\_AUTO | 自动缩放显示。将图片缩放至目标矩形的宽度或高度(选取最小的比例)，并居中显示。 |
| IMAGE\_DRAW\_SCALE\_DOWN | 如果图片比目标矩形大，自动缩小显示，否则居中显示。 |
| IMAGE\_DRAW\_SCALE\_W | 宽度缩放显示。 将图片缩放至目标矩形的宽度，高度按此比例进行缩放，超出不部分不显示。 |
| IMAGE\_DRAW\_SCALE\_H | 高度缩放显示。将图片缩放至目标矩形的高度，宽度按此比例进行缩放，超出不部分不显示。 |
| IMAGE\_DRAW\_REPEAT | 平铺显示。 |
| IMAGE\_DRAW\_REPEAT\_X | 水平方向平铺显示，垂直方向缩放。 |
| IMAGE\_DRAW\_REPEAT\_Y | 垂直方向平铺显示，水平方向缩放。 |
| IMAGE\_DRAW\_REPEAT\_Y\_INVERSE | 垂直方向平铺显示，水平方向缩放(从底部到顶部)。 |
| IMAGE\_DRAW\_PATCH9 | 9宫格显示。将图片分成等大小的9块，4个角按原大小显示在目标矩形的4个角，左右上下和中间5块分别缩放显示在对应的目标区域。 |
| IMAGE\_DRAW\_PATCH3\_X | 水平方向3宫格显示，垂直方向居中显示。将图片在水平方向上分成等大小的3块，左右两块按原大小显示在目标矩形的左右，中间一块缩放显示在目标区域中间剩余部分。 |
| IMAGE\_DRAW\_PATCH3\_Y | 垂直方向3宫格显示，水平方向居中显示。将图片在垂直方向上分成等大小的3块，上下两块按原大小显示在目标矩形的上下，中间一块缩放显示在目标区域中间剩余部分。 |
| IMAGE\_DRAW\_PATCH3\_X\_SCALE\_Y | 水平方向3宫格显示，垂直方向缩放显示。将图片在水平方向上分成等大小的3块，左右两块按原大小显示在目标矩形的左右，中间一块缩放显示在目标区域中间剩余部分。 |
| IMAGE\_DRAW\_PATCH3\_Y\_SCALE\_X | 垂直方向3宫格显示，水平方向缩放显示。将图片在垂直方向上分成等大小的3块，上下两块按原大小显示在目标矩形的上下，中间一块缩放显示在目标区域中间剩余部分。 |
| IMAGE\_DRAW\_REPEAT9 | 平铺9宫格显示。将图片分成4个角和5块平铺块，4个角按原大小显示在目标矩形的4个角，其余5块会平铺对应的目标区域。
切割方法为（如下图）：
如果图片宽度为奇数，则中间一块为一列数据，如果图片宽度为偶数，则中间一块为二列数据，其他数据分为左右块
如果图片高度为奇数，则中间一块为一行数据，如果图片高度为偶数，则中间一块为二行数据，其他数据分为上下块
中间一块数据根据上面两条规则组成4中情况，分别是一列一行数据，一列两行数据，两列一行数据和两行两列数据 |
| IMAGE\_DRAW\_REPEAT3\_X | 水平方向3宫格显示，垂直方向居中显示。将图片在水平方向上分成左右相等两块和中间一块，如果图片宽度为奇数，则中间一块为一列数据，如果图片宽度为偶数，则中间一块为二列数据，其他数据分为左右块。
左右两块按原大小显示在目标矩形的左右，中间一列像素点平铺显示在目标区域中间剩余部分。 |
| IMAGE\_DRAW\_REPEAT3\_Y | 垂直方向3宫格显示，水平方向居中显示。将图片在垂直方向上分成上下相等两块和中间一块，如果图片高度为奇数，则中间一块为一行数据，如果图片高度为偶数，则中间一块为二行数据，其他数据分为上下块
上下两块按原大小显示在目标矩形的上下，中间一块平铺显示在目标区域中间剩余部分。 |