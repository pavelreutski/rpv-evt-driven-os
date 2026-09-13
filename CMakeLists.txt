cmake_minimum_required(VERSION 3.20.0)

project(rpv-evt-driven-kernel
	LANGUAGES C
	DESCRIPTION "rpv event driven kernel")

# public kernel properties

set(EVENT_DATA_FILE "" CACHE FILEPATH "User event data file")

set(KERNEL_PROFILE "LITE" CACHE STRING "Kernel profile (LITE, FULL)")
set_property(CACHE KERNEL_PROFILE PROPERTY STRINGS LITE FULL)

# internal cmake properties

set(KERNEL_AUTOGEN_DIRECTORY "${CMAKE_BINARY_DIR}/autogen/kernel")
set(KERNEL_DATA_AUTOGEN_DIRECTORY "${CMAKE_BINARY_DIR}/autogen/data")

# core sources

file(GLOB KERNEL_CORE_C_SOURCES CONFIGURE_DEPENDS ./src/kernel.c
                                                  ./src/kernel_cmd.c
                                                  ./src/kernel_mon.c
                                                  ./src/kernel_shell.c
                                                  ./src/kernel_stdio.c
                                                  ./src/cmd/*.c)

# system journal sources

file(GLOB KERNEL_SYSJ_SUBSYS_C_SOURCES CONFIGURE_DEPENDS ./src/cmd/sysj/*.c
														 ./src/sysj/kernel_jrnl.c)

# file i/o sources

file(GLOB KERNEL_FIO_SUBSYS_C_SOURCES CONFIGURE_DEPENDS ./src/utils/*.c
                                                        ./src/cmd/fio/*.c
														./src/fio/kernel_fsh.c
                                                        ./src/fio/kernel_fio.c)

string(TOUPPER "${KERNEL_PROFILE}" KERNEL_PROFILE)
	
if(KERNEL_PROFILE STREQUAL "LITE")

	message(STATUS "kernel profile: lite -> basic kernel features enabled")

	set(KERNEL_ENABLE_FIO    OFF) # disable file i/o
	set(KERNEL_ENABLE_SYSJ   OFF) # disable system journal

	set(SHELL_COMMAND_ARGS     3) # lite max args
	set(SHELL_COMMAND_BUFFER  15) # lite command buffer size
	set(SHELL_COMMANDS_STACK   1) # lite commands stack depth

elseif(KERNEL_PROFILE STREQUAL "FULL")

	message(STATUS "kernel profile: full -> all kernel features enabled")

	set(KERNEL_ENABLE_FIO     ON) # enable file i/o
	set(KERNEL_ENABLE_SYSJ    ON) # enable system journal

	set(SHELL_COMMAND_ARGS    10) # full max args 
	set(SHELL_COMMAND_BUFFER 255) # full command buffer size
	set(SHELL_COMMANDS_STACK   5) # full commands stack depth

else()
	message(FATAL_ERROR "invalid kernel profile: ${KERNEL_PROFILE}")
endif()

# event data file parsing

set(EVENT_DATA_TYPES "")    # event data type definitions
set(EVENT_DATA_MEMBERS "")  # event data union members
set(EVENT_DATA_INCLUDES "") # event data includes (e.g. stdlib or libc)

if(EVENT_DATA_FILE)

	if(NOT EXISTS "${EVENT_DATA_FILE}")
        message(FATAL_ERROR "EVENT_DATA_FILE does not exist: " "${EVENT_DATA_FILE}")
    endif()

    file(STRINGS "${EVENT_DATA_FILE}" EVENT_DATA_LINES)

	set(EVENT_DATA_SECTION "")

	foreach(line IN LISTS EVENT_DATA_LINES)

        string(STRIP "${line}" stripped_line)
		string(TOLOWER "${stripped_line}" stripped_line)

        # Section markers
        if("${stripped_line}" STREQUAL "/* event types begin */")
            set(EVENT_DATA_SECTION "types")
            continue()
        endif()

        if("${stripped_line}" STREQUAL "/* event types end */")
            set(EVENT_DATA_SECTION "")
            continue()
        endif()

        if("${stripped_line}" STREQUAL "/* event members begin */")
            set(EVENT_DATA_SECTION "members")
            continue()
        endif()

        if("${stripped_line}" STREQUAL "/* event members end */")
            set(EVENT_DATA_SECTION "")
            continue()
        endif()

        # Ignore empty lines
        if("${stripped_line}" STREQUAL "")
            continue()
        endif()

        # Includes
        if("${stripped_line}" MATCHES "^#include[ \t]+")
            string(APPEND EVENT_DATA_INCLUDES "${line}\n")
            continue()
        endif()

        # Type definitions
        if("${EVENT_DATA_SECTION}" STREQUAL "types")
            string(APPEND EVENT_DATA_TYPES "${line}\n")
            continue()
        endif()

        # Union members
        if("${EVENT_DATA_SECTION}" STREQUAL "members")
            string(APPEND EVENT_DATA_MEMBERS "    ${line} \\\n")
            continue()
        endif()

        message(FATAL_ERROR "Unexpected line outside a USER_EVENT section:\n" "${line}")

    endforeach()

endif()

if(EVENT_DATA_MEMBERS)
    set(EVENT_DATA_DEFINE "#define USER_EVENT_DATA \\\n${EVENT_DATA_MEMBERS}")
else()
    set(EVENT_DATA_DEFINE "#define USER_EVENT_DATA")
endif()

# event data generation

configure_file(./data/event_data.h.in ${KERNEL_DATA_AUTOGEN_DIRECTORY}/event_data.h)

# config generation

configure_file(./config/kernel_conf.h.in ${KERNEL_AUTOGEN_DIRECTORY}/kernel_conf.h)

# object/interface libraries

add_library(kernel-core OBJECT)

target_compile_features(kernel-core PRIVATE c_std_23)
target_sources(kernel-core PRIVATE ${KERNEL_CORE_C_SOURCES})

target_include_directories(kernel-core PRIVATE ./src
											   ./include
											   ./include/core											   
											   ${KERNEL_AUTOGEN_DIRECTORY}
                                               ${KERNEL_DATA_AUTOGEN_DIRECTORY})

add_library(rpv-kernel INTERFACE)
target_sources(rpv-kernel INTERFACE $<TARGET_OBJECTS:kernel-core>)

target_include_directories(rpv-kernel INTERFACE ./include													
												./include/core
                                                ${KERNEL_DATA_AUTOGEN_DIRECTORY})

if (KERNEL_ENABLE_FIO)

	add_library(kernel-fio OBJECT)
	
	target_compile_features(kernel-fio PRIVATE c_std_23)
	target_sources(kernel-fio PRIVATE ${KERNEL_FIO_SUBSYS_C_SOURCES})

	target_sources(rpv-kernel INTERFACE $<TARGET_OBJECTS:kernel-fio>)

	target_include_directories(kernel-fio PRIVATE ./include
										   		  ./include/fio
										   		  ./include/sysj
										   		  ./include/core)

	target_include_directories(rpv-kernel INTERFACE ./include/fio)
	
endif()

if (KERNEL_ENABLE_SYSJ)

	add_library(kernel-sysj OBJECT)

	target_compile_features(kernel-sysj PRIVATE c_std_23)
	target_sources(kernel-sysj PRIVATE ${KERNEL_SYSJ_SUBSYS_C_SOURCES})
	
	target_sources(rpv-kernel INTERFACE $<TARGET_OBJECTS:kernel-sysj>)

	target_include_directories(kernel-sysj PRIVATE ./include
												   ./include/core
												   ./include/sysj)

	target_include_directories(rpv-kernel INTERFACE ./include/sysj)

endif()
