

set(rtk_SOURCE
  src/aw_psp_errno.c
  src/aw_psp_task.c
  )

set(psp_SOURCE ${rtk_SOURCE})
set(psp_HEADERS ${rtk_HEADERS})
set(psp_INCLUDE_DIR include )


add_subdirectory(../../rtk/ rtk_build)

add_library(aw_psp STATIC ${psp_SOURCE} ${psp_HEADERS} )
target_include_directories(aw_psp  PUBLIC ${psp_INCLUDE_DIR}  )
target_link_libraries(aw_psp PUBLIC aworks )
target_link_libraries(aw_psp PUBLIC rtk)

install(TARGETS aw_psp
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )

install(DIRECTORY include
  DESTINATION apollo/psp/rtk/
  )
install(DIRECTORY config
  DESTINATION apollo/psp/rtk/
  )

#export(TARGETS aw_psp FILE aw_psp.cmake EXPORT_LINK_INTERFACE_LIBRARIES )
