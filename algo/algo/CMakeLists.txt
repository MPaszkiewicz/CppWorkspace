# CMakeList.txt : CMake project for algo, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (algo "algo.cpp" "algo.h")
target_link_libraries(algo PRIVATE vector sorting)
#target_include_directories(algo PRIVATE ${PROJECT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET algo PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
