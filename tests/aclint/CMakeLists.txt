project (aclint)

add_executable(${PROJECT_NAME} 
	simple_access.cpp
	${test_util_SOURCE_DIR}/sc_main.cpp
)
target_link_libraries (${PROJECT_NAME} PUBLIC test_util)

catch_discover_tests(${PROJECT_NAME})
