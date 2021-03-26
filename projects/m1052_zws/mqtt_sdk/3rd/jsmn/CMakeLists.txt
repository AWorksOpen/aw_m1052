if(ARM_LINUX)
include(../../arm-linux.mk)
endif()

set(LIB_SRC 
    jsmn.c
)
add_library(jsmn ${LIB_SRC})


