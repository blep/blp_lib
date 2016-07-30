cmake_minimum_required(VERSION 3.0)

enable_testing()

project( SmallLRU CXX )

set(CPP_TESTING_ROOT thirdparty/cpp_testing CACHE PATH "Root directory of cpp_testing project")

option( SMALL_LRU_WITH_TEST "Compile FVMath test" OFF ) 

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

macro(UseCompilationWarningAsError)
	if ( MSVC )
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX ")
	endif( MSVC )
endmacro() 


IF (SMALL_LRU_WITH_TEST)
	add_subdirectory( ${CPP_TESTING_ROOT} )
	include_directories( ${CPP_TESTING_ROOT}/include )
	add_subdirectory( test/small_lru_test )
ENDIF (SMALL_LRU_WITH_TEST)
