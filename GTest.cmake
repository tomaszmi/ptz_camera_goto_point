OPTION(UNIT_TESTING "Enable unit testing" ON)

if (UNIT_TESTING)

enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/googletest)

function(cxx_test name sources)
  add_executable(${name} ${sources})
  target_link_libraries(${name} ${ARGN} gtest_main gmock)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

else (UNIT_TESTING)

function(cxx_test name sources)
endfunction()

endif (UNIT_TESTING)
