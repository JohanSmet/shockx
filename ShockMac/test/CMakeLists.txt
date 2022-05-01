# unit tests
enable_testing()

set (TEST_TARGET test_runner)
set (DIR_TEST ShockMac/test)

add_executable(${TEST_TARGET})
target_sources(${TEST_TARGET} PRIVATE
	${DIR_TEST}/test_main.c
	${DIR_TEST}/test_fix.c

	vendor/munit/munit.c
	vendor/munit/munit.h
)
target_include_directories(${TEST_TARGET} PRIVATE vendor)
target_link_libraries(${TEST_TARGET} PRIVATE ${TARGET_LIB_FIX})

add_test(NAME unittests COMMAND ${TEST_TARGET} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
