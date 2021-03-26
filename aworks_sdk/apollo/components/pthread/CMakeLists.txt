

add_library(pthread STATIC
  source/pthread.c
  source/pthread_cond.c
  )


target_include_directories( pthread PUBLIC
  include
  )
  

#检查是否存在rtk.h
include(CheckIncludeFile)
CHECK_INCLUDE_FILE(rtk.h HAVE_RTK)
if(HAVE_RTK)

endif()

# 需要链接的库
target_link_libraries(pthread PUBLIC aworks aw_common psp )

install(TARGETS pthread
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )

# 安装文件
install(FILES 
    include/aw_pthreadtypes.h 
    include/pthread.h 

DESTINATION include/ )
 
aw_export(pthread)
