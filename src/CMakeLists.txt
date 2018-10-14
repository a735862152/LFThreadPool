#cmake 最低版本号要求
target_include_directories(ThreadPoolDemo PUBLIC ${CMAKE_SOURCE_DIR}/src/BaseType)


target_sources(ThreadPoolDemo PUBLIC ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Task)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/ThreadPool)


#target_link_libraries(Tutorial m ${EXTRAL_LIBS})



#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#install(TARGETS ThreadPoolDemo RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static PUBLIC_HEADER DESTINATION include)