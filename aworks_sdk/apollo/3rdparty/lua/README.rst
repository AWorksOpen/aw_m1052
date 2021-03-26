
*******
Lua 5.3
*******

ReadMe
======

adapter
-------

提供跟 **AWorks** 有关的代码。

adapter/SConscript 会对选中版本的代码自动进行安装，
修改相关文件。

adapter/lua-5.3.4
-----------------

对应的适配包（？）

会被 adapter/SConscript 安装其中的文件到官方目录，
基本上对应的工作都会放到这里。

添加了如下文件/目录：

bin:
   提供 mingw 版的 lua 解析器、编译器、动态库。

test:
   放官方提供的测试 Script。


SConscript:
   根据平台 SCons 的用法，输出相关库跟头文件。

xxx_Conf.py
   Lua 的配置文件，这里是默认的。


lua-5.3.4
---------

官方的 Lua。

note
----

- 《Programming in Lua》(第一版，pdf 格式)，对应 Lua 5.0，入门书籍。
-  note.rst 一些笔记。 

使用
====

如果是想注册 Lua 到 shell ，可以这么做:

.. code:: c

   #include "aw_lua.h"

   void foo(void) 
   {
       aw_lua_shell_init();  /* register lua to shell */
   }

如果是想把 Lua 当做库使用，可以这么做:

.. code:: c

   #include "lauxlib.h"

   void foo(void) 
   {
       lua_State *L = luaL_newstate();  /* create state */

       /* the use of C API of lua */
   }


源码工程
========

运行过一次 SCons 之后，相关源文件会被修改，因此可以放入工程编译
（修改相关工程配置）。

问题
====

- 目前在 AWorks 上运行的 Lua ，有一些标准库的使用是不支持的。
