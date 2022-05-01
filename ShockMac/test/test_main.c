#include "munit/munit.h"

extern MunitTest fix_tests[];
extern MunitTest fix24_tests[];

static MunitSuite extern_suites[] = {
	{	.prefix = "/fix",
		.tests = fix_tests,
		.suites = NULL,
		.iterations = 1,
		.options = MUNIT_SUITE_OPTION_NONE
	},
	{	.prefix = "/fix24",
		.tests = fix24_tests,
		.suites = NULL,
		.iterations = 1,
		.options = MUNIT_SUITE_OPTION_NONE
	},
	{ NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE}
};

static const MunitSuite suite = {
	"",
	NULL,		// tests
	extern_suites,
	1,			// iterations,
	MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[]) {
	return munit_suite_main_custom(&suite, NULL, argc, argv, NULL);
}
