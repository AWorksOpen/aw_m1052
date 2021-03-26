
# goahead源码
file(GLOB aw_goahead_source  "../../../3rdparty/net/goahead-2.5/*.c" )
file(GLOB aw_goahead_headers  "../../../3rdparty/net/goahead-2.5/*.h" )

# adapter源码
file(GLOB_RECURSE aw_goahead_source_adapter "source/*.c" )
file(GLOB_RECURSE aw_goahead_header_adapter "include/*.h" )
# message("headers:${aw_goahead_header_adapter}")
# 移除不需要的文件
list(REMOVE_ITEM aw_goahead_source 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/net/goahead-2.5/cgi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/net/goahead-2.5/sockGen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/net/goahead-2.5/webrom.c
    )
    
add_library(aw_goahead STATIC
  ${aw_goahead_source}
  ${aw_goahead_source_adapter}
  )
add_library(aw_goahead_fs STATIC
  ${aw_goahead_source}
  ${aw_goahead_source_adapter}
  )

target_include_directories( aw_goahead PUBLIC
  include
  include/arch
  ../../../3rdparty/net/goahead-2.5/
  )
target_include_directories( aw_goahead_fs PUBLIC
  include
  include/arch
  ../../../3rdparty/net/goahead-2.5/
  )

#检查是否存在sys/types.h
include(CheckIncludeFile)
CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
if(NOT HAVE_SYS_TYPES_H)
# 不存在sys/types.h, 需要定义fd_mask,NBBY, NFDBITS
target_compile_definitions(aw_goahead PRIVATE "NOT_HAVE_SYS_TYPES_H;WEBS_PAGE_ROM" )
target_compile_definitions(aw_goahead_fs PRIVATE "NOT_HAVE_SYS_TYPES_H;DISABLE_WEBS_PAGE_ROM" )
endif()

# 需要链接的库
target_link_libraries(aw_goahead PUBLIC aworks aw_common aw_lwip_1.4.x )
target_link_libraries(aw_goahead_fs PUBLIC aworks aw_common aw_lwip_1.4.x )

install(TARGETS aw_goahead aw_goahead_fs
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )

# 安装文件
install(FILES ${aw_goahead_headers} ${aw_goahead_header_adapter} DESTINATION include/goahead/ )
 
aw_export(aw_goahead)
aw_export(aw_goahead_fs)
