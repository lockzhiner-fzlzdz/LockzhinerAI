project(test_edit)

set(TARGET_INSTALL_DIR "bin/edit")

set(TARGET_LIST "")
add_executable(Test-Capture test_capture.cc)
list(APPEND TARGET_LIST Test-Capture)

add_executable(Test-FPS test_fps.cc)
list(APPEND TARGET_LIST Test-FPS)

# link and include
foreach(test_project ${TARGET_LIST})
    target_link_libraries(${test_project} PRIVATE ${CMAKE_PROJECT_NAME})
endforeach()

# install
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${TARGET_INSTALL_DIR})
install(TARGETS ${TARGET_LIST} DESTINATION ${TARGET_INSTALL_DIR})