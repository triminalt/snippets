cmake_minimum_required(VERSION 3.11.0)

set(DIR ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "DIR: ${DIR}")

function(group)
    foreach(f IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${f}")
            file(RELATIVE_PATH f_rel "${DIR}" "${f}")
        else()
            set(f_rel "${f}")
        endif()
        get_filename_component(f_path "${f_rel}" PATH)
        string(REPLACE "/" "\\" f_path_proj "${f_path}")
        source_group("${f_path_proj}" FILES "${f}")
    endforeach()
endfunction(group)

# ----------------------------------------------------------------------------
project(live555)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH x64)
else()
    set(ARCH x86)
endif()

if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Disable pointless constant condition warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127 /wd4201")

add_definitions(-DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS)

#inlucde
include_directories(SYSTEM "${DIR}/groupsock/include")
include_directories(SYSTEM "${DIR}/groupsock")
include_directories(SYSTEM "${DIR}/liveMedia/include")
include_directories(SYSTEM "${DIR}/liveMedia")
include_directories(SYSTEM "${DIR}/UsageEnvironment/include")
include_directories(SYSTEM "${DIR}/UsageEnvironment")
include_directories(SYSTEM "${DIR}/BasicUsageEnvironment/include")
include_directories(SYSTEM "${DIR}/BasicUsageEnvironment")

include_directories("${DIR}/groupsock/include")
include_directories("${DIR}/groupsock")
include_directories("${DIR}/liveMedia/include")
include_directories("${DIR}/liveMedia")
include_directories("${DIR}/UsageEnvironment/include")
include_directories("${DIR}/UsageEnvironment")
include_directories("${DIR}/BasicUsageEnvironment/include")
include_directories("${DIR}/BasicUsageEnvironment")

include_directories("${DIR}/BasicUsageEnvironment/include")

link_directories("${CMAKE_BINARY_DIR}")

# source
set(inc_DIR "${DIR}/inc")
file(GLOB_RECURSE INC
    "${inc_DIR}/groupsock/include/*.h*"
    "${inc_DIR}/groupsock/*.h*"
    "${inc_DIR}/liveMedia/include/*.h*"
    "${inc_DIR}/liveMedia/*.h*"
    "${inc_DIR}/UsageEnvironment/include/*.h*"
    "${inc_DIR}/UsageEnvironment/*.h*"
    "${inc_DIR}/BasicUsageEnvironment/include/*.h*"
    "${inc_DIR}/BasicUsageEnvironment/*.h*"
    )
group(${INC})
message(STATUS INC: ${INC})

set(group_DIR "${DIR}/groupsock")
file(GLOB_RECURSE GROUPSOCK
    "${group_DIR}/*.h*"
    "${group_DIR}/*.c*"
    )
group(${GROUPSOCK})
message(STATUS GROUPSOCK : ${GROUPSOCK})

set(liveMedia_DIR "${DIR}/liveMedia")
file(GLOB_RECURSE LIVE_MEDIA
    "${liveMedia_DIR}/*.h*"
    "${liveMedia_DIR}/*.c*"
    )
group(${LIVE_MEDIA})
message(STATUS LIVE_MEDIA : ${LIVE_MEDIA})

set(UsageEnvironment_DIR "${DIR}/UsageEnvironment")
file(GLOB_RECURSE USAGE_ENVIRONMENT
    "${UsageEnvironment_DIR}/*.h*"
    "${UsageEnvironment_DIR}/*.c*"
    )
group(${USAGE_ENVIRONMENT})
message(STATUS USAGE_ENVIRONMENT : ${USAGE_ENVIRONMENT})

set(BasicUsageEnvironment_DIR "${DIR}/BasicUsageEnvironment")
file(GLOB_RECURSE BASIC_USAGE_ENVIRONMENT
    "${BasicUsageEnvironment_DIR}/*.h*"
    "${BasicUsageEnvironment_DIR}/*.c*"
    )
group(${BASIC_USAGE_ENVIRONMENT})
message(STATUS BASIC_USAGE_ENVIRONMENT : ${BASIC_USAGE_ENVIRONMENT})

set(test_DIR "${DIR}/test")
file(GLOB_RECURSE TEST
    "${test_DIR}/*.h*"
    "${test_DIR}/*.c*"
    )
group(${TEST})
message(STATUS TEST: ${TEST})

# lib
add_library(live555 STATIC
    ${GROUPSOCK}
    ${LIVE_MEDIA}
    ${USAGE_ENVIRONMENT}
    ${BASIC_USAGE_ENVIRONMENT}
    )

target_link_libraries(live555
    Ws2_32.lib
)

#target_compile_options(live555 PRIVATE "/MT$<$<STREQUAL:$<CONFIGURATION>,Debug>:d>")


# test
add_executable(server WIN32
    ${INC}
    ${DIR}/test/cc.cxx
    ${DIR}/test/arrptr.hxx
    ${DIR}/test/bit.hxx
    ${DIR}/test/adts.hxx
    ${DIR}/test/aac_pump.hxx
    ${DIR}/test/aac_producer.hxx
    ${DIR}/test/aac_source.hxx
    ${DIR}/test/aac_subsession.hxx
    ${DIR}/test/aac_server.hxx
    ${DIR}/test/h264_pump.hxx
    ${DIR}/test/h264_producer.hxx
    ${DIR}/test/h264_source.hxx
    ${DIR}/test/h264_subsession.hxx
    ${DIR}/test/h264_server.hxx
    ${DIR}/test/main.cxx
    )
target_link_libraries(server
   Ws2_32.lib
   live555.lib
)
set_target_properties(server PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

#set_target_properties(live555-test PROPERTIES OUTPUT_NAME "live555-test")
#set_target_properties(live555-test PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
#target_compile_options(live555-test PRIVATE "/MT$<$<STREQUAL:$<CONFIGURATION>,Debug>:d>")


# mediaServer
set(mediaServer_DIR "${DIR}/mediaServer")
file(GLOB_RECURSE MEDIA_SERVER
    "${mediaServer_DIR}/*.h*"
    "${mediaServer_DIR}/*.c*"
    )
group(${MEDIA_SERVER})
message(STATUS MEDIA_SERVER: ${MEDIA_SERVER})

add_executable(mediaServer WIN32
    ${INC}
    ${MEDIA_SERVER}
    )
target_link_libraries(mediaServer
   Ws2_32.lib
   live555.lib
)
set_target_properties(mediaServer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# proxyServer
set(proxyServer_DIR "${DIR}/proxyServer")
file(GLOB_RECURSE PROXY_SERVER
    "${proxyServer_DIR}/*.h*"
    "${proxyServer_DIR}/*.c*"
    )
group(${PROXY_SERVER})
message(STATUS PROXY_SERVER: ${PROXY_SERVER})

add_executable(proxyServer WIN32
    ${INC}
    ${PROXY_SERVER}
    )
target_link_libraries(proxyServer
   Ws2_32.lib
   live555.lib
)
set_target_properties(proxyServer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# testProgs

# testH264VideoStreamer
add_executable(testH264VideoStreamer WIN32
    ${INC}
    ${DIR}/testProgs/testH264VideoStreamer.cpp
    )
target_link_libraries(testH264VideoStreamer
   Ws2_32.lib
   live555.lib
)
set_target_properties(testH264VideoStreamer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# testH264VideoToTransportStream
add_executable(testH264VideoToTransportStream WIN32
    ${INC}
    ${DIR}/testProgs/testH264VideoToTransportStream.cpp
    )
target_link_libraries(testH264VideoToTransportStream
   Ws2_32.lib
   live555.lib
)
set_target_properties(testH264VideoToTransportStream PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# registerRTSPStream
add_executable(registerRTSPStream WIN32
    ${INC}
    ${DIR}/testProgs/registerRTSPStream.cpp
    ) 
target_link_libraries(registerRTSPStream
   Ws2_32.lib
   live555.lib
)
set_target_properties(registerRTSPStream PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# testOnDemandRTSPServer
add_executable(testOnDemandRTSPServer WIN32
    ${INC}
    ${DIR}/testProgs/testOnDemandRTSPServer.cpp
    ) 
target_link_libraries(testOnDemandRTSPServer
   Ws2_32.lib
   live555.lib
)
set_target_properties(testOnDemandRTSPServer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

# openRTSP
add_executable(openRTSP WIN32
    ${INC}
    ${DIR}/testProgs/playCommon.hh
    ${DIR}/testProgs/playCommon.cpp
    ${DIR}/testProgs/openRTSP.cpp
    ) 
target_link_libraries(openRTSP
   Ws2_32.lib
   live555.lib
)
set_target_properties(openRTSP PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
