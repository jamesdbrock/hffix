#include <iostream>

// We want Boost Date_Time support, so include these before hffix.hpp.
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>

#include <hffix.hpp>

int main(int argc, char** argv)
{
    int sequence_number_send(0);

    char buffer[1 << 13];

    // We'll put a FIX Logon message in the buffer.
    hffix::message_writer logon(buffer, buffer + sizeof(buffer));

    logon.push_back_header("FIX.4.2"); // Write BeginString and BodyLength.

    logon.push_back_string    (hffix::tag::MsgType, "A");                      // Logon MsgType.
    logon.push_back_string    (hffix::tag::SenderCompID, "AAAA");              // Required Standard Header field.
    logon.push_back_string    (hffix::tag::TargetCompID, "BBBB");              // Required Standard Header field.
    logon.push_back_int       (hffix::tag::MsgSeqNum, ++sequence_number_send); // Required Standard Header field.
    logon.push_back_timestamp (hffix::tag::SendingTime,
            boost::posix_time::microsec_clock::universal_time());              // Required Standard Header field.
    logon.push_back_int       (hffix::tag::EncryptMethod, 0);                  // no encryption
    logon.push_back_int       (hffix::tag::HeartBtInt, 10);                    // 10 second heartbeat interval

    logon.push_back_trailer(); // write CheckSum

    // Now the Logon message is written to the buffer. 

    // Add a FIX New Order - Single message to the buffer, after the Logon message.
    hffix::message_writer new_order(logon.message_end(), buffer + sizeof(buffer));

    new_order.push_back_header("FIX.4.2");

    new_order.push_back_string    (hffix::tag::MsgType, "D");                      // New Order - Single
    new_order.push_back_string    (hffix::tag::SenderCompID, "AAAA");              // Required Standard Header field.
    new_order.push_back_string    (hffix::tag::TargetCompID, "BBBB");              // Required Standard Header field.
    new_order.push_back_int       (hffix::tag::MsgSeqNum, ++sequence_number_send); // Required Standard Header field.
    new_order.push_back_timestamp (hffix::tag::SendingTime,
            boost::posix_time::microsec_clock::universal_time());                  // Required Standard Header field.
    new_order.push_back_string    (hffix::tag::ClOrdID, "A1");
    new_order.push_back_char      (hffix::tag::HandlInst, '1');                    // Automated execution.
    new_order.push_back_string    (hffix::tag::Symbol, "OIH");
    new_order.push_back_char      (hffix::tag::Side, '1');                         // Buy.
    new_order.push_back_timestamp (hffix::tag::TransactTime,
            boost::posix_time::microsec_clock::universal_time());
    new_order.push_back_int       (hffix::tag::OrderQty, 100);                     // 100 shares.
    new_order.push_back_char      (hffix::tag::OrdType, '2');                      // Limit order.

    // Limit price $500.01 = 50001*(10^-2). The push_back_decimal() method takes a decimal floating point
    // number of the form mantissa*(10^exponent).
    new_order.push_back_decimal   (hffix::tag::Price, 50001, -2);

    new_order.push_back_char      (hffix::tag::TimeInForce, '1'); // Good Till Cancel.

    new_order.push_back_trailer();

    //Now the New Order message is in the buffer after the Logon message.

    // Write both messages to stdout.
    std::cout.write(buffer, new_order.message_end() - buffer);

    return 0;
}
