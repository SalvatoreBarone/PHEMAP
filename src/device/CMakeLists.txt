set (   	
	DEV_SRC

	${CMAKE_SOURCE_DIR}/src/common/memxor.c
    ${CMAKE_SOURCE_DIR}/src/communication/channel.c
	${CMAKE_SOURCE_DIR}/src/communication/transport.c
	${CMAKE_SOURCE_DIR}/src/device/rc5.c
	${CMAKE_SOURCE_DIR}/src/device/sha256.c
	# ${CMAKE_SOURCE_DIR}/src/device/rm17.c
	${CMAKE_SOURCE_DIR}/src/device/fuzzy.c
	${CMAKE_SOURCE_DIR}/src/device/device.c
	${CMAKE_SOURCE_DIR}/src/device/phemap_dev_phi.c
	${CMAKE_SOURCE_DIR}/src/device/phemap_dev_init.c
	${CMAKE_SOURCE_DIR}/src/device/phemap_dev_enroll.c
	# ${CMAKE_SOURCE_DIR}/src/device/phemap_dev_verification.c
	# ${CMAKE_SOURCE_DIR}/src/device/phemap_dev_salted.c
	# ${CMAKE_SOURCE_DIR}/src/device/phemap_dev_babel_chain.c

)

set (
    DEV_INC

	${CMAKE_SOURCE_DIR}/inc/common/memxor.h
	${CMAKE_SOURCE_DIR}/inc/common/config.h
	# ${CMAKE_SOURCE_DIR}/inc/common/phemap_carnet.h
	${CMAKE_SOURCE_DIR}/inc/communication/channel.h
	${CMAKE_SOURCE_DIR}/inc/communication/transport.h
	${CMAKE_SOURCE_DIR}/inc/device/rc5.h
	${CMAKE_SOURCE_DIR}/inc/device/sha256.h
	# ${CMAKE_SOURCE_DIR}/inc/device/rm17.h
	${CMAKE_SOURCE_DIR}/inc/device/fuzzy.h
    ${CMAKE_SOURCE_DIR}/inc/device/d_config.h
	${CMAKE_SOURCE_DIR}/inc/device/device.h
	${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_phi.h
	${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_init.h
	${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_enroll.h
	# ${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_verification.h
	# ${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_salted.h
	# ${CMAKE_SOURCE_DIR}/inc/device/phemap_dev_babel_chain.h

)

add_library(phemapdev STATIC
			${DEV_SRC}
            ${DEV_INC}
)

target_include_directories(phemapdev
                PRIVATE ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/inc/communication ${CMAKE_SOURCE_DIR}/inc/common ${CMAKE_SOURCE_DIR}/inc/device ${CMAKE_SOURCE_DIR}/inc/common/linux)
