add_library(linux_thread STATIC 
            linux_thread.c
            ${CMAKE_SOURCE_DIR}/inc/common/linux/linux_thread.h
            )   

#Specifies include directories to use when compiling a given target
target_include_directories(linux_thread
                            PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/common/linux)

add_library(linux_timer STATIC 
            linux_timer.c
            ${CMAKE_SOURCE_DIR}/inc/common/linux/linux_timer.h
            )   

#Specifies include directories to use when compiling a given target
target_include_directories(linux_timer
                            PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/common/linux)
