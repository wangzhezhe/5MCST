configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)
add_library(foo SHARED foo.cpp foo.h config.h.in)