set (
    AS_SRC

    ${CMAKE_SOURCE_DIR}/src/authservice/phemap_chain.c
    ${CMAKE_SOURCE_DIR}/src/authservice/phemap_as_enrollment.c
    # ${CMAKE_SOURCE_DIR}/src/authservice/phemap_as_init.c

)

set (
    AS_INC

    ${CMAKE_SOURCE_DIR}/inc/authservice/phemap_chain.h
    ${CMAKE_SOURCE_DIR}/inc/authservice/phemap_as_enrollment.h
    # ${CMAKE_SOURCE_DIR}/inc/authservice/phemap_as_init.h

)

#controlla se è corretta la sintassi
add_library(phemapas STATIC
            ${AS_SRC}
            ${AS_INC}
)

target_include_directories(phemapas
                PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/communication ${CMAKE_SOURCE_DIR}/inc/common ${CMAKE_SOURCE_DIR}/inc/authservice)

# add_library(phemapas_enroll STATIC
#             phemap_as_enrollment.c  
#             ${CMAKE_SOURCE_DIR}/inc/authservice/phemap_as_enrollment.h
# )

# target_include_directories(phemapas_enroll
#                 PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/communication ${CMAKE_SOURCE_DIR}/inc/common ${CMAKE_SOURCE_DIR}/inc/authservice)
