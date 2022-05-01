#include "munit/munit.h"

#include "fix.h"
#include <math.h>

#define FRACT_250	(1 << 14)
#define FRACT_500	(1 << 15)
#define FRACT_750	(1 << 15 | 1 << 14)

static MunitResult test_from_float(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_int32(fix_from_float(1.0f), ==, 1 << 16);
	munit_assert_int32(fix_from_float(0.5f), ==, 1 << 15);
	munit_assert_int32(fix_from_float(2.0f), ==, 1 << 17);
	munit_assert_int32(fix_from_float(-1.0f), ==, -(1u << 16));
	munit_assert_int32(fix_from_float(-0.5f), ==, -(1u << 15));
	munit_assert_int32(fix_from_float(-2.0f), ==, -(1u << 17));

    return MUNIT_OK;
}

static MunitResult test_to_float(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_float(fix_float(1 << 16), ==, 1.0f);
	munit_assert_float(fix_float(1 << 15), ==, 0.5f);
	munit_assert_float(fix_float(1 << 17), ==, 2.0f);

	munit_assert_float(fix_float(-(1 << 16)), ==, -1.0f);
	munit_assert_float(fix_float(-(1 << 15)), ==, -0.5f);
	munit_assert_float(fix_float(-(1 << 17)), ==, -2.0f);

    return MUNIT_OK;
}

static MunitResult test_add(const MunitParameter params[], void* user_data_or_fixture) {

	fix a = fix_make(1, 0);
	fix b = fix_make(1, 0);
	munit_assert_int32(a + b, ==, fix_make(2, 0));

	a = fix_make(1, 0);
	b = fix_make(0, FRACT_500);
	munit_assert_int32(a + b, ==, fix_make(1, FRACT_500));

	a = fix_make(1, FRACT_250);
	b = fix_make(0, FRACT_250);
	munit_assert_int32(a + b, ==, fix_make(1, FRACT_500));

	a = fix_make(1, FRACT_750);
	b = fix_make(0, FRACT_250);
	munit_assert_int32(a + b, ==, fix_make(2, 0));

    return MUNIT_OK;
}

static MunitResult test_subtract(const MunitParameter params[], void* user_data_or_fixture) {

	fix a = fix_make(1, 0);
	fix b = fix_make(1, 0);
	munit_assert_int32(a - b, ==, fix_make(0, 0));

	a = fix_make(1, 0);
	b = fix_make(0, FRACT_500);
	munit_assert_int32(a - b, ==, fix_make(0, FRACT_500));

	a = fix_make(1, FRACT_250);
	b = fix_make(0, FRACT_250);
	munit_assert_int32(a - b, ==, fix_make(1, 0));

	a = fix_make(1, FRACT_750);
	b = fix_make(0, FRACT_250);
	munit_assert_int32(a - b, ==, fix_make(1, FRACT_500));

    return MUNIT_OK;
}

static MunitResult test_multiply(const MunitParameter params[], void* user_data_or_fixture) {

	fix a = fix_make(1, 0);
	fix b = fix_make(1, 0);
	munit_assert_int32(fix_mul(a,b), ==, fix_make(1, 0));

	a = fix_make(5, 0);
	b = fix_make(0, 0);
	munit_assert_int32(fix_mul(a,b), ==, fix_make(0, 0));

	a = fix_make(5, 0);
	b = fix_make(2, 0);
	munit_assert_int32(fix_mul(a,b), ==, fix_make(10, 0));

	a = fix_make(5, 0);
	b = fix_make(0, FRACT_500);
	munit_assert_int32(fix_mul(a,b), ==, fix_make(2, FRACT_500));

    return MUNIT_OK;
}

static MunitResult test_divide(const MunitParameter params[], void* user_data_or_fixture) {

	fix a = fix_make(1, 0);
	fix b = fix_make(1, 0);
	munit_assert_int32(fix_div(a,b), ==, fix_make(1, 0));

	a = fix_make(0, 0);
	b = fix_make(5, 0);
	munit_assert_int32(fix_div(a,b), ==, fix_make(0, 0));

	a = fix_make(5, 0);
	b = fix_make(0, FRACT_500);
	munit_assert_int32(fix_div(a,b), ==, fix_make(10, 0));

	a = fix_make(5, 0);
	b = fix_make(2, 0);
	munit_assert_int32(fix_div(a,b), ==, fix_make(2, FRACT_500));

    return MUNIT_OK;
}

static MunitResult test_mul_div(const MunitParameter params[], void* user_data_or_fixture) {

	fix a = fix_make(1, 0);
	fix b = fix_make(1, 0);
	fix c = fix_make(2, 0);
	munit_assert_int32(fix_mul_div(a,b,c), ==, fix_make(0, FRACT_500));

	a = fix_make(2, 0);
	b = fix_make(5, 0);
	c = fix_make(10, 0);
	munit_assert_int32(fix_mul_div(a,b,c), ==, fix_make(1, 0));

    return MUNIT_OK;
}

static MunitResult test_sqrt(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_int32(fix_sqrt(-(1u << 16)), ==, fix_from_float(0.0f));
	munit_assert_int32(fix_sqrt(fix_make(0, 0)), ==, fix_from_float(0.0f));
	munit_assert_int32(fix_sqrt(fix_make(1, 0)), ==, fix_from_float(1.0f));

    return MUNIT_OK;
}

static MunitResult test_dist(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_int32(fix_pyth_dist(fix_make(1, 0), fix_make(0, 0)), ==, fix_from_float(1.0f));
	munit_assert_int32(fix_pyth_dist(fix_make(0, 0), fix_make(1, 0)), ==, fix_from_float(1.0f));
	munit_assert_int32(fix_pyth_dist(fix_make(3, 0), fix_make(4, 0)), ==, fix_from_float(5.0f));

    return MUNIT_OK;
}

static MunitResult test_fast_dist(const MunitParameter params[], void* user_data_or_fixture) {

	fix result = fix_fast_pyth_dist(fix_make(1, 0), fix_make(0, 0));
	float expected = 1.0f;
	munit_assert_float(fabs(fix_float(result) - expected), <, expected * 0.13f);

	result = fix_fast_pyth_dist(fix_make(0, 0), fix_make(1, 0));
	expected = 1.0f;
	munit_assert_float(fabs(fix_float(result) - expected), <, expected * 0.13f);

	result = fix_fast_pyth_dist(fix_make(3, 0), fix_make(4, 0));
	expected = 5.0f;
	munit_assert_float(fabs(fix_float(result) - expected), <, expected * 0.13f);


    return MUNIT_OK;
}

static MunitResult test_sin_cos(const MunitParameter params[], void* user_data_or_fixture) {

	fix sin, cos;

	fix_sincos(degrees_to_fixang(0), &sin, &cos);
	munit_assert_int32(sin, ==, fix_make(0, 0));
	munit_assert_int32(cos, ==, fix_make(1, 0));

	fix_sincos(degrees_to_fixang(45), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

	fix_sincos(degrees_to_fixang(90), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(1.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	fix_sincos(degrees_to_fixang(135), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	fix_sincos(degrees_to_fixang(180), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-1.0f)), <, 10);

	fix_sincos(degrees_to_fixang(225), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	fix_sincos(degrees_to_fixang(270), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-1.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	fix_sincos(degrees_to_fixang(315), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

    return MUNIT_OK;
}

static MunitResult test_sin(const MunitParameter params[], void* user_data_or_fixture) {

	fix sin = fix_sin(degrees_to_fixang(0));
	munit_assert_int32(sin, ==, fix_make(0, 0));

	sin = fix_sin(degrees_to_fixang(45));
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);

	sin = fix_sin(degrees_to_fixang(90));
	munit_assert_int32(abs(sin - fix_from_float(1.0f)), <, 10);

	sin = fix_sin(degrees_to_fixang(135));
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);

	sin = fix_sin(degrees_to_fixang(180));
	munit_assert_int32(abs(sin - fix_from_float(0.0f)), <, 10);

	sin = fix_sin(degrees_to_fixang(225));
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);

	sin = fix_sin(degrees_to_fixang(270));
	munit_assert_int32(abs(sin - fix_from_float(-1.0f)), <, 10);

	sin = fix_sin(degrees_to_fixang(315));
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);

    return MUNIT_OK;
}

static MunitResult test_cos(const MunitParameter params[], void* user_data_or_fixture) {

	fix cos = fix_cos(degrees_to_fixang(0));
	munit_assert_int32(cos, ==, fix_make(1, 0));

	cos = fix_cos(degrees_to_fixang(45));
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

	cos = fix_cos(degrees_to_fixang(90));
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	cos = fix_cos(degrees_to_fixang(135));
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	cos = fix_cos(degrees_to_fixang(180));
	munit_assert_int32(abs(cos - fix_from_float(-1.0f)), <, 10);

	cos = fix_cos(degrees_to_fixang(225));
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	cos = fix_cos(degrees_to_fixang(270));
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	cos = fix_cos(degrees_to_fixang(315));
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

    return MUNIT_OK;
}

static MunitResult test_fast_sin_cos(const MunitParameter params[], void* user_data_or_fixture) {

	fix sin, cos;

	fix_fastsincos(degrees_to_fixang(0), &sin, &cos);
	munit_assert_int32(sin, ==, fix_make(0, 0));
	munit_assert_int32(cos, ==, fix_make(1, 0));

	fix_fastsincos(degrees_to_fixang(45), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

	fix_fastsincos(degrees_to_fixang(90), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(1.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	fix_fastsincos(degrees_to_fixang(135), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	fix_fastsincos(degrees_to_fixang(180), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(0.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-1.0f)), <, 10);

	fix_fastsincos(degrees_to_fixang(225), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(-0.707106781f)), <, 10);

	fix_fastsincos(degrees_to_fixang(270), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-1.0f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.0f)), <, 10);

	fix_fastsincos(degrees_to_fixang(315), &sin, &cos);
	munit_assert_int32(abs(sin - fix_from_float(-0.707106781f)), <, 10);
	munit_assert_int32(abs(cos - fix_from_float(0.707106781f)), <, 10);

    return MUNIT_OK;
}

static MunitResult test_asin(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_int32(abs(fix_asin(fix_from_float(0.0f)) - degrees_to_fixang(0)), <, 2);
	munit_assert_int32(abs(fix_asin(fix_from_float(0.707106781f)) - degrees_to_fixang(45)), <, 2);
	munit_assert_int32(abs(fix_asin(fix_from_float(1.0f)) - degrees_to_fixang(90)), <, 2);
	munit_assert_int32(abs(fix_asin(fix_from_float(-0.707106781f)) - degrees_to_fixang(-45)), <, 2);
	munit_assert_int32(abs(fix_asin(fix_from_float(-1.0f)) - degrees_to_fixang(-90)), <, 2);

    return MUNIT_OK;
}

static MunitResult test_acos(const MunitParameter params[], void* user_data_or_fixture) {

	munit_assert_int32(abs(fix_acos(fix_from_float(0.0f)) - degrees_to_fixang(90)), <, 2);
	munit_assert_int32(abs(fix_acos(fix_from_float(0.707106781f)) - degrees_to_fixang(45)), <, 2);
	munit_assert_int32(abs(fix_acos(fix_from_float(1.0f)) - degrees_to_fixang(0)), <, 2);
	munit_assert_int32(abs(fix_acos(fix_from_float(-0.707106781f)) - degrees_to_fixang(135)), <, 2);
	munit_assert_int32(abs(fix_acos(fix_from_float(-1.0f)) - degrees_to_fixang(180)), <, 2);

    return MUNIT_OK;
}

static MunitResult test_atan2(const MunitParameter params[], void* user_data_or_fixture) {

	fixang result = fix_atan2(fix_make(1, 0), fix_make(1, 0));
	munit_assert_int32(abs(result - degrees_to_fixang(45)), <, 2);

	result = fix_atan2(fix_make(1, 0), fix_make(0, 0));
	munit_assert_int32(abs(result - degrees_to_fixang(90)), <, 2);

	result = fix_atan2(-(1u << 16), fix_make(0, 0));
	munit_assert_int32(abs(result - degrees_to_fixang(-90)), <, 2);

	result = fix_atan2(fix_make(1, 0), -(1u << 16));
	munit_assert_int32(abs(result - degrees_to_fixang(135)), <, 2);

	result = fix_atan2(-(1u << 16), -(1u << 16));
	munit_assert_int32(abs(result - degrees_to_fixang(-135)), <, 2);

    return MUNIT_OK;
}

MunitTest fix_tests[] = {
    { "/from_float", test_from_float, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/to_float", test_to_float, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/add", test_add, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/subtract", test_subtract, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/multiply", test_multiply, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/divide", test_divide, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/mul_div", test_mul_div, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/sqrt", test_sqrt, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/dist", test_dist, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/fast_dist", test_fast_dist, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/sin_cos", test_sin_cos, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/sin", test_sin, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/cos", test_cos, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/fast_sin_cos", test_fast_sin_cos, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/asin", test_asin, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/acos", test_acos, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { "/atan2", test_atan2, NULL, NULL,  MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
