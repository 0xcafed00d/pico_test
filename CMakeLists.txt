cmake_minimum_required(VERSION 3.13)

project(pico_test CXX)

include(GNUInstallDirs)

add_library(pico_test INTERFACE)
add_library(pico_test::pico_test ALIAS pico_test)

target_include_directories(
	pico_test
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(
	pico_test
	INTERFACE
		cxx_std_20
)

