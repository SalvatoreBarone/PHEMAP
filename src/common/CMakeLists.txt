add_library(memxor STATIC 
            memxor.c
            ${CMAKE_SOURCE_DIR}/inc/common/memxor.h
            )   

#Specifies include directories to use when compiling a given target
target_include_directories(memxor
                            PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/common)
