
TESTS_ENVIRONMENT = \
	JDB_TEST_URL="http://localhost:8080/rpc" \
	JDB_TEST_SERVICE="com.pengutronix.jdb.test|/" \
	JDB_TEST_VERBOSE="$(CONFIG_DEBUG)"

basic_TESTS = \
	basic_boolean_test.sh \
	basic_byte_test.sh \
	basic_int16_test.sh \
	basic_uint16_test.sh \
	basic_int32_test.sh \
	basic_uint32_test.sh \
	basic_int64_test.sh \
	basic_uint64_test.sh \
	basic_double_test.sh \
	basic_string_test.sh \
	basic_signature_test.sh

TESTS = \
	test_init.sh \
	$(basic_TESTS) \
	test_finish.sh

EXTRA_DIST = \
	dbus-test-service.py \
	libtest.sh \
	test_setup.sh \
	$(basic_TESTS)

CLEANFILES = \
	dbus-test-service.pid \
	lighttpd.pid \
	error.log

