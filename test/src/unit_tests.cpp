#define BOOST_TEST_MODULE hffix Unit Tests
#include <boost/test/included/unit_test.hpp>

#include <hffix.hpp>
#include <hffix_fields.hpp>

#if __cplusplus >= 201703L
using namespace std::literals::string_view_literals;
#endif
using namespace hffix;

BOOST_AUTO_TEST_CASE(basic)
{
    // Print the BOOST version, mostly so we can see it in Travis.
    std::cout << "BOOST version " << BOOST_LIB_VERSION << std::endl;

    char b[1024];
    message_writer w(b);
    w.push_back_header("FIX.4.2");
    w.push_back_string(tag::MsgType, "A");
    w.push_back_trailer();
    message_reader r(b);

    // A reader constructed from a writer should have the same size.
    BOOST_CHECK(w.message_size() == r.message_size());

    // Construct an invalid iterator.
    message_reader::const_iterator j;

    // Field value comparisons.
    message_reader::const_iterator i = r.begin();
    BOOST_CHECK(r.find_with_hint(tag::MsgType, i));
    BOOST_CHECK(i->value() == "A");
    BOOST_CHECK(i->value() == (const char*)"A");
    BOOST_CHECK(i->value() != "B");
    BOOST_CHECK(i->value() == std::string("A"));
    BOOST_CHECK(i->value() != std::string("B"));
#if __cplusplus >= 201703L
    BOOST_CHECK(i->value() == "A"sv); // string_view_literals
    BOOST_CHECK(i->value() != "B"sv); // string_view_literals
#endif

    // message_writer preconditions
    {
        message_writer w(b);
        BOOST_CHECK_THROW(w.push_back_trailer(false), std::logic_error);
    }
    {
        message_writer w(b);
        w.push_back_header("FIX.4.2");
        BOOST_CHECK_THROW(w.push_back_header("FIX.4.2"), std::logic_error);
    }
}

// find the minimum size buffer the message printed by f() will fit
// call the function with a buffer ranging from 100 bytes to 0 bytes
// record the smallest size where it does not throw an exception, make
// sure there's a single point where it starts throwing and that it keeps
// throwing for smaller buffer sizes
template <typename F>
int test_bound_checking(F f)
{
	char buffer[101];
	int minimum_size = 101;
	for (int i = 100; i >= 0; --i) {
		message_writer writer(buffer, buffer + i);
		try {
			buffer[i] = '\x55';
			f(writer);
			BOOST_CHECK(minimum_size == i + 1);
			minimum_size = i;
		}
		catch (std::out_of_range const& e) {}
		// make sure the next byte was not clobbered
		BOOST_CHECK(buffer[i] == '\x55');
	}
	return minimum_size;
}

BOOST_AUTO_TEST_CASE(message_writer_bounds)
{
	using W = message_writer;
	BOOST_CHECK(test_bound_checking([](W& w) { w.push_back_header("FIX.4.2"); } ) == 19);
	BOOST_CHECK(test_bound_checking([](W& w) {
                w.push_back_header("FIX.4.2");
                w.push_back_trailer(false);
            } ) == 26);
	BOOST_CHECK(test_bound_checking([](W& w) {
                w.push_back_header("FIX.4.2");
                w.push_back_trailer(true);
            } ) == 26);

	// 14 characters
	auto const test_string = "string literal";
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_string(58, test_string, test_string + 14); } ) == 4 + 14);
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_string(58, test_string); } ) == 4 + 14);
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_string(58, std::string(test_string)); } ) == 4 + 14);
#if HFFIX_HAS_STRING_VIEW
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_string(58, std::string_view(test_string)); } ) == 4 + 14);
#endif
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_char(58, 'a'); } ) == 5);
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_int(58, 55); } ) == 6);

	// 58=123.456|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_decimal(58, 123456, -3); } ) == 11);

	// 58=123456|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_decimal(58, 123456, 0); } ) == 10);

	// 58=19700101|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_date(58, 1970, 1, 1); } ) == 12);

	// 58=197001|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_monthyear(58, 1970, 1); } ) == 10);

	// 58=HH:MM:SS|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_timeonly(58, 23, 59, 59, 999); } ) == 16);

	// 58=YYYYMMDD-HH:MM:SS|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_timestamp(58, 1970, 1, 1, 23, 59, 59); } ) == 21);

	// 58=YYYYMMDD-HH:MM:SS.sss|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_timestamp(58, 1970, 1, 1, 23, 59, 59, 999); } ) == 25);

	// 58=14|59=..............|
	BOOST_CHECK(test_bound_checking([&](W& w) { w.push_back_data(58, 59, test_string, test_string + 14); } ) == 24);
}
