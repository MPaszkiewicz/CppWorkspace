add_library(sorting INTERFACE)

file(GLOB SORTING_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

target_sources(sorting INTERFACE ${SORTING_HEADERS})

target_include_directories(sorting INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(test_sorting "test_sorting.cpp" "quick_sort.h")
target_link_libraries(test_sorting PRIVATE sorting)

enable_testing()
add_test(NAME sorting_tests COMMAND test_sorting)