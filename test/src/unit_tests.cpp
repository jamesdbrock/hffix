#define BOOST_TEST_MODULE hffix Unit Tests
#include <boost/test/included/unit_test.hpp>

#include <hffix.hpp>
#include <hffix_fields.hpp>

using namespace hffix;

BOOST_AUTO_TEST_CASE(firstdraft)
{
    char b[1024];
    message_writer w(b, b + sizeof(b));
    w.push_back_header("FIX.4.2");
    w.push_back_string(tag::MsgType, "A");
    w.push_back_trailer();
    message_reader r(b, b + sizeof(b));

    BOOST_TEST(w.message_size() == r.message_size());

    message_reader::const_iterator i = r.begin();
    BOOST_TEST(r.find_with_hint(tag::MsgType, i));
    char const* a = "A";
    BOOST_TEST(i->value() == a);
    BOOST_TEST(i->value() == "A");
    // BOOST_TEST(i->value() != "B");
}

