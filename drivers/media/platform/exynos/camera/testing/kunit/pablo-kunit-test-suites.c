// SPDX-License-Identifier: GPL-2.0
/*
 * Samsung Exynos SoC series Pablo driver
 *
 * Exynos Pablo image subsystem functions
 *
 * Copyright (c) 2021 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/version.h>

#include "pablo-kunit-test.h"

#if (KERNEL_VERSION(6, 1, 0) <= LINUX_VERSION_CODE)
#define KUNIT_TEST_SUITE_INIT(s) __kunit_test_suites_init(s, pablo_kunit_test_subsuite_num(s))
#define KUNIT_TEST_SUITE_EXIT(s) __kunit_test_suites_exit(s, pablo_kunit_test_subsuite_num(s))
#else
#define KUNIT_TEST_SUITE_INIT(s) __kunit_test_suites_init(s)
#define KUNIT_TEST_SUITE_EXIT(s) __kunit_test_suites_exit(s)
#endif

extern struct kunit_suite * const * const pablo_kunit_test_suites[];

static bool pablo_kunit_test_suite_is_null(struct kunit_suite * const *suites)
{
	if (suites && suites[0]->test_cases)
		return false;

	return true;
}

static void pablo_test_suite_is_null_kunit_test(struct kunit *test)
{
	bool ret;
	struct kunit_suite test_suite_dummy = {
		.name = "test-suite-dummy",
		.test_cases = NULL,
	};
	struct kunit_suite *suite_array[] = { &test_suite_dummy, NULL };

	ret = pablo_kunit_test_suite_is_null(suite_array);
	KUNIT_EXPECT_EQ(test, ret, (bool)true);
}

static struct kunit_case pablo_kunit_test_cases[] = {
	KUNIT_CASE(pablo_test_suite_is_null_kunit_test),
	{},
};

struct kunit_suite pablo_kunit_test_suite = {
	.name = "pablo-kunit-test-suite",
	.test_cases = pablo_kunit_test_cases,
};
define_pablo_kunit_test_suites(&pablo_kunit_test_suite);

struct kunit_suite test_suite_end = {
	.name = "pablo-kunit-test-suite-end",
	.test_cases = NULL,
};
define_pablo_kunit_test_suites_end(&test_suite_end);

static int pablo_kunit_test_subsuite_num(struct kunit_suite * const *subsuite)
{
	int num_of_suites = 0;

	while (*subsuite) {
		num_of_suites++;
		subsuite++;
	}

	return num_of_suites;
}

static void pablo_kunit_print_tap_header(void)
{
	int num_of_suites = 0;

	struct kunit_suite * const * const *suites;

	suites = pablo_kunit_test_suites;
	while (!pablo_kunit_test_suite_is_null(*suites)) {
		num_of_suites += pablo_kunit_test_subsuite_num(*suites);
		suites++;
	}

	pr_info("PABLO TAP version 14\n");
	pr_info("1..%d\n", num_of_suites);
}

static void __run_kunit_test(void)
{
	struct kunit_suite * const * const *suites;

	pablo_kunit_print_tap_header();

	suites = pablo_kunit_test_suites;
	while (!pablo_kunit_test_suite_is_null(*suites)) {
		KUNIT_TEST_SUITE_INIT(*suites);
		suites++;
	}
}

static void __teardown_kunit_test(void)
{
	struct kunit_suite * const * const *suites;

	suites = pablo_kunit_test_suites;
	while (!pablo_kunit_test_suite_is_null(*suites)) {
		KUNIT_TEST_SUITE_EXIT((struct kunit_suite **)*suites);
		suites++;
	}
}

static int __init pablo_kunit_test_suites_init(void)
{
	pr_info("pablo_kunit_test module init");
	return 0;
}
module_init(pablo_kunit_test_suites_init);

static void __exit pablo_kunit_test_suites_exit(void)
{
	pr_info("pablo_kunit_test module exit");
}
module_exit(pablo_kunit_test_suites_exit)

static int __run_pablo_kunit_test(const char *val, const struct kernel_param *kp)
{
	if (val[0] == '1') {
		__run_kunit_test();
		__teardown_kunit_test();
	}

	return 0;
}

static const struct kernel_param_ops __run_kunit_ops = {
	.set = __run_pablo_kunit_test,
	.get = NULL,
};

module_param_cb(run_kunit_test, &__run_kunit_ops, NULL, 0644);

MODULE_IMPORT_NS(DMA_BUF);
MODULE_LICENSE("GPL");
