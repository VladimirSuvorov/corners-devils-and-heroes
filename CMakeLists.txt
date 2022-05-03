
cmake_minimum_required(VERSION 3.21) 
project(CornersTheGame LANGUAGES CXX)
#set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
	set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
	set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()


message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")
message("CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")


execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
				WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
				OUTPUT_VARIABLE PACKAGE_GIT_VERSION
				ERROR_QUIET
				OUTPUT_STRIP_TRAILING_WHITESPACE)

include(extensions.cmake)

init_submodule_autodetection()
check_all_submodules("glfw3;glbinding")

add_subdirectory(externals/glfw3 EXCLUDE_FROM_ALL) #glfw3
add_subdirectory(externals/glbinding EXCLUDE_FROM_ALL) #glbinding
add_subdirectory(externals/glm EXCLUDE_FROM_ALL) #glm
add_subdirectory(externals/GSL EXCLUDE_FROM_ALL) #glm

recurse_find_files(game_sources "${CMAKE_CURRENT_SOURCE_DIR}/src;" "cpp;")
add_executable(CornersTheGame ${game_sources})
target_compile_options(CornersTheGame PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W1>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
set_target_properties(CornersTheGame PROPERTIES LINK_FLAGS "/PROFILE")
#[[
target_compile_options(CornersTheGame PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)]]


#target_include_directories(CornersTheGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/glm)
target_include_directories(CornersTheGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/stb)
#target_include_directories(CornersTheGame PRIVATE glm)

target_link_libraries(CornersTheGame PRIVATE Microsoft.GSL::GSL)
target_link_libraries(CornersTheGame PRIVATE glfw)
target_link_libraries(CornersTheGame PRIVATE glbinding::glbinding)
target_link_libraries(CornersTheGame PRIVATE glm::glm)

recurse_find_files(resources "${CMAKE_CURRENT_SOURCE_DIR}/resources;" "png;shader;")
install(TARGETS CornersTheGame DESTINATION ${PROJECT_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE})
add_custom_command(TARGET CornersTheGame POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${resources} 
                       $<TARGET_FILE_DIR:CornersTheGame>
                   )
install(FILES ${resources} DESTINATION ${PROJECT_BINARY_DIR}/../bin/${CMAKE_BUILD_TYPE})