add_executable(small_lru_test
    small_lru_test.cpp
	../../include/blp/small_lru.hpp
) 

target_link_libraries( small_lru_test cpput3_main_lib )

add_test(small_lru_test small_lru_test)
