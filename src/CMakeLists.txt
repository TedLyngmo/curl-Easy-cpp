add_library(curleasy curleasy.cpp)
set_target_properties(curleasy PROPERTIES PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/include/curleasy/curleasy.hpp")
target_include_directories(curleasy PUBLIC "${PROJECT_SOURCE_DIR}/include")
find_library(LIBCURL curl)
target_link_libraries(curleasy ${LIBCURL})

INSTALL(TARGETS curleasy
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/curleasy
)

#target_include_directories(mylib PUBLIC
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/mylib>
#  $<INSTALL_INTERFACE:include/mylib>  # <prefix>/include/mylib
#)
