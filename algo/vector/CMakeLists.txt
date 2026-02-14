
add_library(vector INTERFACE)

file(GLOB VECTOR_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")

target_sources(vector INTERFACE ${VECTOR_HEADERS})

target_include_directories(vector INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}
)