message("${CMAKE_BUILD_TYPE}")
# project settings

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(EMBY_FOLDER ../../..)
set(EMBY_PLATFORM X86_Unix)

if (EMBY_FOLDER)
    message(STATUS "including EMBY cmake:" ${EMBY_FOLDER}/emby.cmake)
    set(ENV{EMBY_FOLDER} ["${EMBY_FOLDER}"])
    include("${EMBY_FOLDER}/emby.cmake")
endif()


add_compile_options("${COMPILE_OPTS}")
add_link_options("${COMPILE_OPTS}")
set(CMAKE_EXE_LINKER_FLAGS  "${COMPILE_OPTS}")

if(LINUX)
   add_compile_options(-fsingle-precision-constant)
endif()
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0  -Werror=return-type)

# uncomment to mitigate c++17 absolute addresses warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register -fno-exceptions  -fno-use-cxa-atexit -fno-threadsafe-statics ")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)


#definition for project
add_compile_definitions(EMBY_BUILD_x86)
add_compile_options(-Os -g)

#get absolute path from Emby rotto path from here
get_filename_component(ROOT_DIR "../../" ABSOLUTE)
message(STATUS ${ROOT_DIR})

message(STATUS ${PROJECT_SOURCE_DIR})



## report size
function(show_object_size target)
    string(REPLACE "objcopy" "size" CMAKE_OBJSIZE "${CMAKE_OBJCOPY}")
    message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf)
    message(STATUS ${CMAKE_OBJSIZE})

    add_custom_command(
            TARGET ${target} POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_OBJSIZE} --format=berkeley --totals ${CMAKE_CURRENT_BINARY_DIR}/${target}
    )
endfunction()

#show_object_size(${PROJECT_NAME})
