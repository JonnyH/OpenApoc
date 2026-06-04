# project name, and type
PROJECT(OpenApoc_Launcher CXX C)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

qt_standard_project_setup()

set (LAUNCHER_SOURCE_FILES
	main.cpp
	launcherwindow.cpp
	launcherwindow.ui)

set(sources ${sources} "apocicon.rc")

list(APPEND ALL_SOURCE_FILES ${LAUNCHER_SOURCE_FILES})

qt_add_executable(OpenApoc_Launcher ${LAUNCHER_SOURCE_FILES} ${sources})

target_link_libraries(OpenApoc_Launcher PRIVATE Qt6::Widgets)

target_link_libraries(OpenApoc_Launcher PRIVATE OpenApoc_Library)
target_link_libraries(OpenApoc_Launcher PRIVATE OpenApoc_Framework)

set_target_properties(OpenApoc_Launcher PROPERTIES
		WIN32_EXECUTABLE ON
		MACOSX_BUNDLE ON
)

set_target_properties(OpenApoc_Launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

qt_generate_deploy_app_script(
	TARGET OpenApoc_Launcher
	OUTPUT_SCRIPT deploy_script
	NO_TRANSLATIONS
	NO_COMPILER_RUNTIME
)
install(SCRIPT ${deploy_script})
install(TARGETS OpenApoc_Launcher
	RUNTIME DESTINATION bin
	BUNDLE  DESTINATION .
)

