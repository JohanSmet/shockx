#include "munit/munit.h"

#include "rnd.h"

static MunitResult test_range_zero_one(RndStream *rs) {

	RndSeed(rs, 0xDEADBEEF);

	for (size_t i = 0; i < 10; ++i) {
		float random = fix_float(RndFix(rs));
		munit_assert_float(random, >=, 0.0f);
		munit_assert_float(random, <, 1.0f);
	}

	return MUNIT_OK;
}

static MunitResult test_range_integer(RndStream *rs) {

	RndSeed(rs, 0xDEADBEEF);

	for (size_t i = 0; i < 10; ++i) {
		int32_t random = RndRange(rs, 20, 40);
		munit_assert_int32(random, >=, 20);
		munit_assert_int32(random, <=, 40);
	}

	for (size_t i = 0; i < 10; ++i) {
		int32_t random = RndRange(rs, -40, 40);
		munit_assert_int32(random, >=, -40);
		munit_assert_int32(random, <=, 40);
	}

	return MUNIT_OK;
}

static MunitResult test_range_fix(RndStream *rs) {

	RndSeed(rs, 0xDEADBEEF);

	for (size_t i = 0; i < 10; ++i) {
		float random = fix_float(RndRangeFix(rs, fix_make(2,0), fix_make(4, 0)));
		munit_assert_float(random, >=, 2.0f);
		munit_assert_float(random, <=, 4.0f);
	}

	for (size_t i = 0; i < 10; ++i) {
		float random = fix_float(RndRangeFix(rs, fix_from_float(-4.0f), fix_from_float(4.0f)));
		munit_assert_float(random, >=, -4.0f);
		munit_assert_float(random, <=, 4.0f);
	}

	return MUNIT_OK;
}

static MunitResult test_distribution(RndStream *rs) {
#define NUM_BINS		100
#define NUM_PER_BIN		1000

	RndSeed(rs, 0xDEADBEEF);

	// fill bins randomly
	int32_t bins[NUM_BINS] = {0};

	for (size_t i = 0; i < (NUM_BINS * NUM_PER_BIN); ++i) {
		int32_t b = RndRange(rs, 0, NUM_BINS - 1);
		bins[b] += 1;
	}

	// with a uniform distribution each bin should contain about 1000 samples
	for (size_t i = 0; i < NUM_BINS; ++i) {
		munit_assert_int32(abs(bins[i] - NUM_PER_BIN), <=, 50);
	}

	return MUNIT_OK;
}

static MunitResult test_lc16_range_zero_one(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_LC16(rsLc16);
    return test_range_zero_one(&rsLc16);
}

static MunitResult test_lc16_range_integer(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_LC16(rsLc16);
    return test_range_integer(&rsLc16);
}

static MunitResult test_lc16_range_fix(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_LC16(rsLc16);
    return test_range_fix(&rsLc16);
}

static MunitResult test_lc16_distribution(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_LC16(rsLc16);
    return test_distribution(&rsLc16);
}

static MunitResult test_gauss16_range_zero_one(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16(rsGauss16);
    return test_range_zero_one(&rsGauss16);
}

static MunitResult test_gauss16_range_integer(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16(rsGauss16);
    return test_range_integer(&rsGauss16);
}

static MunitResult test_gauss16_range_fix(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16(rsGauss16);
    return test_range_fix(&rsGauss16);
}

static MunitResult test_gauss16fast_range_zero_one(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16FAST(rsGauss16Fast);
    return test_range_zero_one(&rsGauss16Fast);
}

static MunitResult test_gauss16fast_range_integer(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16FAST(rsGauss16Fast);
    return test_range_integer(&rsGauss16Fast);
}

static MunitResult test_gauss16fast_range_fix(const MunitParameter params[], void* user_data_or_fixture) {
	RNDSTREAM_GAUSS16FAST(rsGauss16Fast);
    return test_range_fix(&rsGauss16Fast);
}

MunitTest rnd_tests[] = {
    { "/lc16_range_zero_one", test_lc16_range_zero_one, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/lc16_range_integer", test_lc16_range_integer, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/lc16_range_fix", test_lc16_range_fix, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/lc16_distribution", test_lc16_distribution, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/gauss16_range_zero_one", test_gauss16_range_zero_one, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/gauss16_range_integer", test_gauss16_range_integer, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/gauss16_range_fix", test_gauss16_range_fix, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/gauss16fast_range_zero_one", test_gauss16fast_range_zero_one, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/gauss16fast_range_integer", test_gauss16fast_range_integer, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
	{ "/gauss16fast_range_fix", test_gauss16fast_range_fix, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
