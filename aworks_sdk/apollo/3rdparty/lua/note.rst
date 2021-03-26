
****
note
****

1. 对 lua-5.3.4 作出的改动：

   - 将 lua.c luac.c 的 main　改成 xxx_main
   - lua.c 包含头文件 dumpio.h
     
2. 关于 dumpio.c ，因为 lua.c 中的 lua_readline 调用 fgets(stdin)
   不会产生阻塞（现象），然后就实现了 dumpio.c ，提供给 lua_readline ，
   对 fgets() 没影响。

