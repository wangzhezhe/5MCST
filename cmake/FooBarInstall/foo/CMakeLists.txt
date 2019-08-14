configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)
add_library(foo SHARED foo.cpp foo.h config.h.in)

set_target_properties(foo PROPERTIES
  PUBLIC_HEADER "foo.h;${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS foo
  EXPORT FooBarInstallTargets
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/foo")



