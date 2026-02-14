add_library(vector INTERFACE)

file(GLOB VECTOR_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

target_sources(vector INTERFACE ${VECTOR_HEADERS})

target_include_directories(vector INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(test_merge_sort test_merge_sort.cpp)
target_link_libraries(test_merge_sort PRIVATE vector)

enable_testing()
add_test(NAME merge_sort_tests COMMAND test_merge_sort)