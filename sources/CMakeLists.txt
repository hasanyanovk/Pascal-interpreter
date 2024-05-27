set(target ${PROJ_LIBRARY})

add_subdirectory(parser)

file(GLOB hdrs "*.h*")
file(GLOB srcs "*.cpp")

add_library(${target} STATIC ${srcs} ${hdrs})
target_link_libraries(${target} ${LIBRARY_DEPS} ${PROJ_LIBRARY}_parser)