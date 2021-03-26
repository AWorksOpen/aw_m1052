#project( libcurl C)
#cmake_minimum_required(VERSION 3.1)

#add_subdirectory( ${AWORKS_SDKROOT} aworks_root )
set(LIB_NAME aw_unity)



add_library(${LIB_NAME} STATIC
    aworks_adapter/aworks_unity_adapter.c
    src/unity.c
    extras/fixture/src/unity_fixture.c
  )

target_include_directories(${LIB_NAME} PUBLIC
    aworks_adapter
    src
    extras/fixture/src
  )


target_link_libraries(${LIB_NAME}  PUBLIC aworks aw_common )

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION ${AWORKS_LIB_INSTALL_DIR}
  RUNTIME DESTINATION bin
  )

install(DIRECTORY aworks_adapter/
  DESTINATION include/Unity/
  FILES_MATCHING PATTERN "*.h" 
  )
install(DIRECTORY src/
  DESTINATION include/Unity/
  FILES_MATCHING PATTERN "*.h" 
  )
install(DIRECTORY extras/fixture/src/
  DESTINATION include/Unity/
  FILES_MATCHING PATTERN "*.h" 
  )
