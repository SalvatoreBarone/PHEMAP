cmake_minimum_required(VERSION 3.10)

#set the project name
project(phemap)

#set version number
set (phemap_VERSION_MAJOR 1)
set (phemap_VERSION_MINOR 0)

set(CMAKE_BUILD_TYPE Debug)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wfatal-errors")

#add thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

include_directories(
    ${CMAKE_SOURCE_DIR}/inc 
    ${CMAKE_SOURCE_DIR}/inc/common 
    ${CMAKE_SOURCE_DIR}/inc/common/linux 
    ${CMAKE_SOURCE_DIR}/inc/communication 
    ${CMAKE_SOURCE_DIR}/inc/authservice
    ${CMAKE_SOURCE_DIR}/inc/device
)

# add the executable
add_executable(peerMain ${CMAKE_SOURCE_DIR}/src/peer.c)

# target runPeer is only build when `make runPeer` is issued
add_custom_target(runPeer
    COMMAND peerMain
    DEPENDS peerMain
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# add the executable
add_executable(enrollMain ${CMAKE_SOURCE_DIR}/src/enroll.c)

# target runPeer is only build when `make runEnroll` is issued
add_custom_target(runEnroll
    COMMAND 
    DEPENDS enrollMain
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# add the executable
add_executable(phemap_initMain ${CMAKE_SOURCE_DIR}/src/phemap_init_test.c)

# target runPeer is only build when `make runEnroll` is issued
add_custom_target(runPhemapInit
    COMMAND 
    DEPENDS phemap_initMain
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)
###necessito di fare prima gli altri
find_library(LIBRT rt) 
if(LIBRT)
# target_link_libraries(peerMain PRIVATE Threads::Threads channel transport phemapas_enroll ${LIBRT})
target_link_libraries(peerMain PRIVATE Threads::Threads channel transport phemapas ${LIBRT})
target_link_libraries(enrollMain PRIVATE Threads::Threads channel transport phemapas phemapdev ${LIBRT})
target_link_libraries(phemap_initMain PRIVATE Threads::Threads channel transport phemapas phemapdev ${LIBRT})
#target_link_libraries(target_name ${LIBRT})
endif()

