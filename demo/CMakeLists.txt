add_executable(example example.cpp)
target_link_libraries(example LINK_PUBLIC curleasy)
target_include_directories(example PUBLIC "${PROJECT_SOURCE_DIR}/include")
