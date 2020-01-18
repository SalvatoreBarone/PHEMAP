add_library(channel STATIC
            channel.c
            ${CMAKE_SOURCE_DIR}/inc/communication/channel.h
)

target_include_directories(channel
                PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/communication ${CMAKE_SOURCE_DIR}/inc/common ${CMAKE_SOURCE_DIR}/inc/common/linux)

find_library(LIBRT rt) 
if(LIBRT)
target_link_libraries(channel PRIVATE Threads::Threads linux_thread linux_timer ${LIBRT})
endif()

add_library(transport STATIC
            transport.c
            ${CMAKE_SOURCE_DIR}/inc/communication/transport.h
)

target_include_directories(transport
                PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/communication ${CMAKE_SOURCE_DIR}/inc/common)