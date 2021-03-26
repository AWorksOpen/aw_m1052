

add_library(aworks INTERFACE )

target_include_directories(aworks
  INTERFACE include posix
  )
target_link_libraries(aworks INTERFACE aw_psp )

install(DIRECTORY ./ DESTINATION apollo/interface
  FILES_MATCHING PATTERN "*.h"
  )

#export(TARGETS aworks FILE aworks.cmake EXPORT_LINK_INTERFACE_LIBRARIES )
