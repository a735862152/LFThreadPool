#cmake 最低版本号要求
cmake_minimum_required(VERSION 3.12.2)


enable_language(CXX)

#项目信息
project(LFThreadPool)
add_executable(ThreadPoolDemo )
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
	set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
	set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
	# cmake reports AMD64 on Windows, but we might be building for 32-bit.
	if (CMAKE_CL_64)
		set(ARCH "x86_64")
	else()
		set(ARCH "x86")
	endif()
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
	set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
	set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
	set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
	set(ARCH "arm")
else()
	message(FATAL_ERROR "Unknown processor:" ${CMAKE_SYSTEM_PROCESSOR})
endif()
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(ThreadPoolDemo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)

