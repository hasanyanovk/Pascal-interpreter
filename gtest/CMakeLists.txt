set(target "gtest")

add_library(${target} STATIC gtest-all.cc)

target_link_libraries(${target} ${pthread})
