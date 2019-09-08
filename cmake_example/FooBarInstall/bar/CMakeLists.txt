add_executable(bar bar.cpp)
target_link_libraries(bar foo)

install(TARGETS bar
  EXPORT FooBarInstallTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
 