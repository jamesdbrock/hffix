/*******************************************************************************************
Copyright 2011, T3 IP, LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY T3 IP, LLC ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL T3 IP, LLC OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of T3 IP, LLC.
*******************************************************************************************/




/* High Frequency FIX Parser library v1.4.1, by James Dawson Brock <james.brock@t3trading.com> */

#ifndef HFFIX_HPP
#define HFFIX_HPP

/*!
\file
\brief The High Frequency FIX Parser library.

Just include "hffix.hpp" to use High
Frequency FIX in your project.
*/

/*! \mainpage High Frequency FIX Parser (Financial Information Exchange)

The High Frequency FIX Parser library is an open source implementation of
<a href="http://www.fixtradingcommunity.org/pg/structure/tech-specs/fix-protocol">Financial Information Exchange protocol versions 4.2, 4.3, 4.4, and 5.0 SP2.</a>
intended for use by developers of high frequency, low latency financial software.
The purpose of the library is to do fast, efficient encoding and decoding of FIX in place,
at the location of the I/O buffer, without using intermediate objects or allocating any memory on the free store.

High Frequency FIX Parser tries to follow the
<a href="http://www.boost.org/development/requirements.html">Boost Library Requirements and Guidelines</a>.
It is modern platform-independent C++98 and depends only on the C++ Standard Library.
It is patterned after the C++ Standard Template Library, and it models each FIX message with
Container and Iterator concepts. It employs compile-time generic templates but does not
employ object-oriented inheritance.

High Frequency FIX Parser is a one-header-only library, so there are no binaries to link. Just
<tt>#include <hffix.hpp> </tt> to use High Frequency FIX Parser in your application.
It also plays well with Boost. If you are using
<a href="http://www.boost.org/doc/html/date_time.html">Boost Date_Time</a> in your
application, High Frequency FIX Parser will support conversion between FIX fields
and Boost Date_Time types.

The design criteria for High Frequency FIX Parser are based on our experience passing messages to various FIX hosts
for high frequency quantitative trading at <a href="http://www.t3live.com">T3 Trading Group, LLC</a>.

All of the Financial Information Exchange (FIX) protocol specification versions supported by the library are bundled into the the distribution, in the <tt>spec</tt> directory. As a convenience for the developer, the High Frequency FIX Parser library includes a Python script which parses all of the FIX protocol specification documents and generates the <tt>include/hffix_fields.hpp</tt> file. That file has <tt>enum</tt> definitions in a tag namspace and an hffix::dictionary_init_field function which allows fields to be referred to by name instead of number both during development, and at run-time.

High Frequency FIX Parser is distributed under the open source FreeBSD License, also known as the Simplified BSD License.

The main repository is at https://github.com/jamesdbrock/hffix

\section difference Features

\subsection others Other FIX Implementations

Typical FIX implementations model a FIX message either as an associative key-value container of
strongly typed objects that inherit from some field superclass,
or as a class type for each message type with member variables for every possible field in the message. Both of
these approaches are inflexible and inefficient. The advantage of doing it this way is that you get
random access reads and writes to the fields of the FIX message.

Every high frequency trading application already has an object model
in the application that provides random access to properties of trading objects, however.
A class for an order,
and a class for an execution, and a class for a quote, et cetera. If your application has that,
then it doesn't need random access to message fields, it needs only serial access.

\subsection thisone High Frequency FIX Parser Implementation

High Frequency FIX Parser models reading fields serially with an
<a href="http://www.sgi.com/tech/stl/ForwardIterator.html">immutable Forward Iterator</a>
and
writing fields serially to a
<a href="http://www.sgi.com/tech/stl/BackInsertionSequence.html">Back Insertion Sequence Container</a>.
It performs reading and writing directly on the I/O buffer,
without any intermediate objects and without calling new or malloc or
doing any free store allocations of any kind.

Field values are weakly typed. They are exposed to the developer
as <tt>char const* begin(), char const* end()</tt>, and High Frequency FIX Parser provides
a complete set of conversion functions for ints,
decimal floats, dates and times, et cetera for the developer to call at his discretion.

\section session FIX Sessions

Managing sessions requires making choices about sockets and threads.
High Frequency FIX Parser does not manage sessions.
It is intended for developers who want a FIX parser with which to build a session manager
for a high-performance trading system that already has a carefully crafted socket and
threading architecture.

FIX has transport-layer features mixed in with the messages, and most FIX hosts
have various quirks in the way they employ the administrative messages. To manage a FIX
session your application will need to match the the transport-layer and administrative features
of the other FIX host. High Frequency FIX Parser has the flexibility to express any subset or
proprietary superset of FIX.

\subsection encryption Encryption

High Frequency FIX Parser supports the binary data field types such as SecureData, but it does not
implement any of the EncryptMethods suggested by the FIX specifications. If you want to encrypt or decrypt some data you'll have to do the encryption or decryption yourself.

\subsection checksum CheckSum

High Frequency FIX Parser can calculate CheckSums and add the CheckSum field for all messages that you encode.
It does not validate the CheckSum of messages decoded.

\subsection sequence Sequence Numbers

The MsgSeqNum field in the FIX Standard Header is exposed for reading and writing.

\subsection administrative Administrative Messages

The administrative messages Logon, Logout, ResendRequest,
Heartbeat, TestRequest, SeqReset-Reset and SeqReset-GapFill don't get special treatment in High Frequency FIX Parser.
Any administrative message
can be encoded or decoded like any other message.

\subsection customtags User-Defined Fields and Custom Tags

High Frequency FIX Parser does not enforce the data type ("Format") of the Field Definitions for
content fields in the FIX spec, so the developer is free to read or write any field data type with any tag number.

\section usage Using High Frequency FIX Parser

\subsection Exceptions

All High Frequency FIX Parser methods, functions, constructors, and destructors provide the No-Throw exception guarantee
unless they are documented to throw exceptions, in which case they provide the Basic exception guarantee.

\subsection Threads

Hight Frequency FIX Parser is not thread-aware at all and has no threads, mutexes, locks, or atomic operations.

All const methods of the hffix::message_reader are thread safe.

The hffix::message_writer is not thread safe.

hffix::message_reader and hffix::message_writer have no storage of their own, they read and write fields directly
on the buffer with which they are constructed. You must be sure the buffer endures until they are destroyed.

\subsection reading Reading a FIX Message

Here is a simple example of how use hffix::message_reader
to read a FIX message from a buffer and print each field to \c std::cout.

During construction, hffix::message_reader checks to make sure there is a complete,
valid FIX message in the buffer. It looks only at the header and trailer transport fields in the message,
not at the content fields, so construction is O(1).

If hffix::message_reader is complete and valid after construction,
hffix::message_reader::begin() returns an iterator that points to the MsgType field
in the FIX Standard Message Header, and
hffix::message_reader::end() returns an iterator that points to the CheckSum field in the
FIX Standard Message Trailer.

\code
#include "hffix.hpp"

char buffer[512]; // Suppose we just read a 75-byte message into this buffer.

hffix::tag_name_dictionary dictionary; // For looking up the string field name of the tag.

hffix::message_reader reader(buffer, buffer + 75);

if (reader.is_complete() && reader.is_valid())
{
    std::cout << "Read " << reader.message_size() << " bytes from buffer." << '\n';

    for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
    {
        std::cout << dictionary[i->tag()];
        std::cout << "=";
        std::cout.write(i->value().begin(), i->value().size());
        std::cout << " ";
    }
}
\endcode

Example Output:

\code
Read 75 bytes from buffer.
MsgType=0 SenderCompID=AAAA TargetCompID=BBBB MsgSeqNum=1 SendingTime=20110130-19:14:21.056
\endcode

\subsection writing Writing a FIX Message

Here is a simple example of how to use hffix::message_writer to write a FIX Heartbeat message to a buffer.
The hffix::message_writer::push_back_header() and hffix::message_writer::push_back_trailer() methods will
write the BeginString, BodyLength, and CheckSum transport fields for us.

\code
#include "hffix.hpp"

char buffer[512];

hffix::message_writer writer(buffer, buffer + 512);

writer.push_back_header();
writer.push_back_string(hffix::tag::MsgType, "0");
writer.push_back_string(hffix::tag::SenderCompID, "AAAA");
writer.push_back_string(hffix::tag::TargetCompID, "BBBB");
writer.push_back_int(hffix::tag::MsgSeqNum, 1);
writer.push_back_timestamp(hffix::tag::SendingTime, boost::posix_time::microsec_clock::universal_time());
writer.push_back_trailer();

std::cout.write(writer.message_begin(), writer.message_size()) << '\n';
std::cout << "Wrote " << writer.message_size() << " bytes to buffer." << '\n';
\endcode

Example Output:

\code
8=FIX.4.2☺9=0051☺35=0☺49=AAAA☺56=BBBB☺34=1☺52=20110130-19:14:21.056☺10=049☺
Wrote 75 bytes to buffer.
\endcode

\subsection example A Complete Example

Here is a more complete example that shows reading, writing, and type conversions.

\code

#include "hffix.hpp"

#include <iostream>
#include <string>

// Here is a data type for an order that we'll use for the parsing example, below.
struct order
{
    boost::posix_time::ptime transaction_time;
    char cl_ord_id[21];
    std::string symbol; // we use std::string for this example, though you'll probably use some non-allocating string type.
    char side;
    int order_quantity;
    int price_in_tenth_pennies; // our pricing system will use tenths of pennies = 1 dollar / 1000.
    char order_type;
    char time_in_force;
};

int main(int argc, char const** argv)
{
    long long sequence_number_send(0); // can be any integer type
    long long sequence_number_recv(0); // can be any integer type

    // Create a buffer.
    char buffer[512];


    // We'll put a FIX Logon message in the buffer.
    hffix::message_writer logon_message(buffer, buffer + 512);

    logon_message.push_back_header(); // Write BeginString and BodyLength.
    logon_message.push_back_string(hffix::tag::MsgType, "A"); // Logon MsgType.
    logon_message.push_back_string(hffix::tag::SenderCompID, "AAAA"); // Required Standard Header field.
    logon_message.push_back_string(hffix::tag::TargetCompID, "BBBB"); // Required Standard Header field.
    logon_message.push_back_int(hffix::tag::MsgSeqNum, ++sequence_number_send); // Required Standard Header field.
    logon_message.push_back_timestamp(hffix::tag::SendingTime, boost::posix_time::microsec_clock::universal_time()); // Required Standard Header field.
    logon_message.push_back_int(hffix::tag::EncryptMethod, 0); // no encryption
    logon_message.push_back_int(hffix::tag::HeartBtInt, 10); // 10 second heartbeat interval

    char encryption_key[16]; // Pretend there's a binary public encryption key in here.
    // If we were using encryption, we could write the binary encryption key into RawData like so
    logon_message.push_back_data(hffix::tag::RawDataLength, hffix::tag::RawData, encryption_key, encryption_key + 16);

    logon_message.push_back_trailer(); // write CheckSum

    // Now the Logon message is ready to send. It occupies the buffer.
    std::cout << "Wrote Logon to the buffer, " << logon_message.message_size() << " bytes." << '\n';

    // We can print the FIX Logon message to std::cout. It contains lots of non-printing
    // characters, so it'll look strange. At this point we could also send it out a network socket.
    std::cout.write(logon_message.message_begin(), logon_message.message_size());





    // Add a FIX New Order - Single message to the buffer, after the Logon message.
    hffix::message_writer new_order_message(logon_message.message_end(), buffer + 512);

    new_order_message.push_back_header();
    new_order_message.push_back_string(hffix::tag::MsgType, "D"); // New Order - Single
    new_order_message.push_back_string(hffix::tag::SenderCompID, "AAAA"); // Required Standard Header field.
    new_order_message.push_back_string(hffix::tag::TargetCompID, "BBBB"); // Required Standard Header field.
    new_order_message.push_back_int(hffix::tag::MsgSeqNum, ++sequence_number_send); // Required Standard Header field.
    new_order_message.push_back_timestamp(hffix::tag::SendingTime, boost::posix_time::microsec_clock::universal_time()); // Required Standard Header field.
    new_order_message.push_back_string(hffix::tag::ClOrdID, "A1");
    new_order_message.push_back_char(hffix::tag::HandlInst, '1'); // Automated execution.
    new_order_message.push_back_string(hffix::tag::Symbol, "OIH");
    new_order_message.push_back_char(hffix::tag::Side, '1'); // Buy.
    new_order_message.push_back_timestamp(hffix::tag::TransactTime, boost::posix_time::microsec_clock::universal_time());
    new_order_message.push_back_int(hffix::tag::OrderQty, 100); // 100 shares.
    new_order_message.push_back_char(hffix::tag::OrdType, '2'); // Limit order.

    // Limit price $500.01 = 50001*(10^-2). The push_back_decimal() method takes a decimal floating point
    // number of the form mantissa*(10^exponent).
    new_order_message.push_back_decimal(hffix::tag::Price, 50001, -2);

    new_order_message.push_back_char(hffix::tag::TimeInForce, '1'); // Good Till Cancel.
    new_order_message.push_back_trailer();

    //Now the New Order message is in the buffer after the Logon message.
    std::cout << "Wrote New Order to the buffer, " << new_order_message.message_size() << " bytes." << '\n';

    // Print the FIX New Order message to std::cout.
    std::cout.write(new_order_message.message_begin(), new_order_message.message_size());





    //Now we'll parse the FIX messages out of the buffer that we've just written.

    hffix::message_reader reader(buffer, new_order_message.message_end());

    // Try to read as many complete and valid messages as are in the buffer.
    for (; reader.is_complete(); ++reader)
    {
        if (reader.is_valid())
        {
            std::cout << "Reading message of size " << reader.message_size() << " bytes." << '\n';

            // Get a peek at this message before we parse it by printing the raw FIX to std::cout.
            std::cout << std::string(reader.message_begin(), reader.message_end()) << '\n';

            // Check if this is a New Order MsgType.
            // We can't use switch(reader.message_type()->value().as_char()) here because
            // some FIX message types have multiple characters.
            // Use the overloaded hffix::operator==(field_value const&, char const*) instead.
            if (reader.message_type()->value() == "D")
            {
                // Make an instance of our order struct, see above, which will store fields that we read from this message.
                order o;

                // Iterate over each field in the message.
                for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
                {
                    switch(i->tag())
                    {
                    case hffix::tag::MsgSeqNum:
                        sequence_number_recv = i->value().as_int<long long>();
                        break;
                    case hffix::tag::ClOrdID:
                        memcpy(o.cl_ord_id, i->value().begin(), i->value().size());
                        o.cl_ord_id[i->value().size()] = 0; // null-terminate the cl_ord_id.
                        break;
                    case hffix::tag::Symbol:
                        o.symbol = i->value().as_string<char>();
                        break;
                    case hffix::tag::Side:
                        o.side = i->value().as_char();
                        break;
                    case hffix::tag::TransactTime:
                        o.transaction_time = i->value().as_timestamp();
                        break;
                    case hffix::tag::OrdType:
                        o.order_type = i->value().as_char();
                        break;
                    case hffix::tag::Price:
                        int exponent;
                        // as_decimal() is an ascii-to-decimal conversion function that returns the value of
                        // the field as mantissa*(10^exponent), where exponent is always <= 0.
                        i->value().as_decimal(o.price_in_tenth_pennies, exponent);
                        o.price_in_tenth_pennies *= 1000; // Because this price is represented as tenths of pennies.
                        while(exponent++) o.price_in_tenth_pennies /= 10; // Now scale to 10^exponent, where exponent <= 0.
                        break;
                    case hffix::tag::TimeInForce:
                        o.time_in_force = i->value().as_char();
                        break;
                    }
                }

                //Now the order o has been populated with values from the message.
            }

            else if (reader.message_type()->value() == "A") // Logon MsgType
            {
                std::string sender_comp_id;

                // Iterate over each field in the message.
                for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
                {
                    switch(i->tag())
                    {
                    case hffix::tag::MsgSeqNum:
                        sequence_number_recv = i->value().as_int<long long>();
                        break;
                    case hffix::tag::SenderCompID:
                        sender_comp_id = i->value().as_string<char>();
                        break;
                    }
                }

                std::cout << sender_comp_id << " has sent a Logon message." << '\n';
            }

            else // This is a MsgType that we weren't expecting to see.
            {
                std::cout << "Unexpected message of type " << reader.message_type()->value().as_string<char>() << '\n';

                for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
                    std::cout << hffix::field_name(i->tag()) << " = " << std::string(i->value().begin(), i->value().end()) << '\n';
            }
        }
        else
        {
            std::cout << "Invalid message." << '\n';
        }
    }

    return 0;
}
\endcode

*/


#include <cstring>          // for memcpy
#include <string>           // for tag_names
#include <map>              // for tag_names
#include <sstream>          // for tag_names
#include <algorithm>        // for is_tag_a_data_length
#include <iostream>         // for message_reader::value_type::operator<<()
#include <limits>           // for numeric_limits<>::is_signed
#include <stdexcept>        // for exceptions
#include <ctime>            // for std::tm

//!
// \def HFFIX_NO_BOOST_DATETIME
//
// \brief If defined, High Frequency FIX Parser will not include Boost Date_Time support.
//
// If the Boost Date_Time library is available in your build environment, boost::posix_time::ptime,
// boost::posix_time::time_duration, and boost::gregorian::date
// conversions will be automatically be supported for the various FIX date and time field types.
//
// To include the Boost Date_Time library types and enable High Frequency FIX Parser support, do:
// \code
// #include <boost/date_time/posix_time/posix_time_types.hpp>
// #include <boost/date_time/gregorian/gregorian_types.hpp>
// \endcode
//
// To prevent High Frequency FIX Parser support for the Boost Date_Time library, define HFFIX_NO_BOOST_DATETIME before including hffix.hpp:
// \code
// #define HFFIX_NO_BOOST_DATETIME
// #include <hffix.hpp>
// \endcode
#ifndef HFFIX_NO_BOOST_DATETIME
#ifdef DATE_TIME_TIME_HPP_ // The header include guard from boost/date_time/time.hpp
#ifdef DATE_TIME_DATE_HPP_ // The header include guard from boost/date_time/date.hpp
#define HFFIX_BOOST_DATETIME
#endif
#endif
#endif

#ifdef HFFIX_BOOST_DATETIME
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#endif // HFFIX_BOOST_DATETIME

#include <hffix_fields.hpp>

/*!
\brief Namespace for all types and functions of High Frequency FIX Parser.
*/
namespace hffix {

/* @cond EXCLUDE */

namespace details {

/*
\brief Internal ascii-to-integer conversion.

Parses ascii and returns a (possibly negative) integer.

\tparam Int_type The type of integer to be returned.
\param begin Pointer to the beginning of the ascii string.
\param end Pointer to past-the-end of the ascii string.
\return The ascii string represented as an integer of type Int_type.
*/
template<typename Int_type> Int_type atoi(char const* begin, char const* end)
{
    Int_type val(0);
    bool isnegative(false);

    if (begin < end && *begin == '-') {
        isnegative = true;
        ++begin;
    }

    for(; begin<end; ++begin) {
        val *= 10;
        val += (Int_type)(*begin - '0');
    }

    return isnegative ? -val : val;
}


/*
\brief Internal ascii-to-unsigned-integer conversion.

Parses ascii and returns an unsigned integer.

\tparam Uint_type The type of unsigned integer to be returned.
\param begin Pointer to the beginning of the ascii string.
\param end Pointer to past-the-end of the ascii string.
\return The ascii string represented as an unsigned integer of type Uint_type.
*/
template<typename Uint_type> Uint_type atou(char const* begin, char const* end)
{
    Uint_type val(0);

    for(; begin<end; ++begin) {
        val *= 10u;
        val += (Uint_type)(*begin - '0');
    }

    return val;
}


/*
\brief Internal integer-to-ascii conversion.

Writes an integer out as ascii.

\tparam Int_type Type of integer to be converted.
\param number Value of the integer to be converted.
\param buffer Pointer to location for the ascii to be written.
\return Pointer to past-the-end of the ascii that was written.
*/
template<typename Int_type> char* itoa(Int_type number, char* buffer)
{
    bool isnegative(false);
    if (number < 0) {
        isnegative = true;
        number = -number;
    }

    char*b = buffer;
    do {
        *b++ = '0' + (number % 10);
        number /= 10;
    } while(number);

    if (isnegative) {
        *b++ = '-';
    }

    char* e = b - 1;

    while(e > buffer) { // Reverse the digits in-place.
        char tmp = *e;
        *e = *buffer;
        *buffer = tmp;
        ++buffer;
        --e;
    }

    return b;
}


/*
\brief Internal unsigned-integer-to-ascii conversion.

Writes an unsigned integer out as ascii.

\tparam Int_type Type of integer to be converted.
\param number Value of the integer to be converted.
\param buffer Pointer to location for the ascii to be written.
\return Pointer to past-the-end of the ascii that was written.
*/
template<typename Uint_type> char* utoa(Uint_type number, char* buffer)
{
    char*b = buffer;
    do {
        *b++ = '0' + (number % 10);
        number /= 10;
    } while(number);

    char* e = b - 1;

    while(e > buffer) { // Do the reversal in-place instead of making temp buffer. Better cache locality, and we don't have to guess how big to make the temp buffer.
        char tmp = *e;
        *e = *buffer;
        *buffer = tmp;
        ++buffer;
        --e;
    }

    return b;
}

/*
\brief Internal ascii-to-decimal conversion.

Converts an ascii string to a decimal float, of the form \htmlonly mantissa&times;10<sup>exponent</sup>\endhtmlonly.

Non-normalized. The exponent will never be  greater than zero.
If the decimal float is an integer, the exponent will be zero.

\tparam Int_type Integer type for the mantissa and exponent.
\param begin Pointer to the beginning of the ascii string.
\param end Pointer to past-the-end of the ascii string.
\param mantissa Reference to storage for the mantissa of the decimal float to be returned.
\param exponent Reference to storage for the exponent of the decimal float to be returned.
*/
template<typename Int_type> void atod(char const* begin, char const* end, Int_type& mantissa, Int_type& exponent)
{
    mantissa = 0;
    exponent = 0;
    bool isdecimal(false);
    bool isnegative(false);

    if (begin < end && *begin == '-') {
        isnegative = true;
        ++begin;
    }

    for(; begin<end; begin++) {
        if (*begin == '.') {
            isdecimal = true;
        } else {
            mantissa *= 10;
            mantissa += (*begin - '0');
            if (isdecimal) --exponent;
        }
    }

    if (isnegative) mantissa = -mantissa;
}

/*
\brief Internal decimal-to-ascii conversion.

Converts a decimal float of the form \htmlonly mantissa&times;10<sup>exponent</sup>\endhtmlonly to ascii.

Non-normalized. The exponent parameter must be less than or equal to zero.

\tparam Int_type Integer type for the mantissa and exponent.
\param mantissa The mantissa of the decimal float.
\param exponent The exponent of the decimal float. Must be less than or equal to zero.
\param buffer Pointer to location for the ascii to be written.
\return Pointer to past-the-end of the ascii that was written.
*/
template<typename Int_type> char* dtoa(Int_type mantissa, Int_type exponent, char* buffer)
{
    bool isnegative(false);
    if (mantissa < 0) {
        isnegative = true;
        mantissa = -mantissa;
    }

    char*b = buffer;
    do {
        *b++ = '0' + (mantissa % 10);
        mantissa /= 10;
        if (++exponent == 0) {
            *b++ = '.';
        }
    } while(mantissa > 0 || exponent < 1);

    if (isnegative) {
        *b++ = '-';
    }

    char* e = b - 1;

    while(e > buffer) { // Do the reversal in-place instead of making temp buffer. Better cache locality, and we don't have to guess how big to make the temp buffer.
        char tmp = *e;
        *e = *buffer;
        *buffer = tmp;
        ++buffer;
        --e;
    }

    return b;
}


/*
\brief Internal ascii-to-date conversion.

Parses ascii and returns a LocalMktDate or UTCDate.

\param begin Pointer to the beginning of the ascii string.
\param end Pointer to past-the-end of the ascii string.
\param[out] year Year.
\param[out] month Month.
\param[out] day Day.
\return True if successful and the out arguments were set.
*/
inline bool atodate(
    char const* begin,
    char const* end,
    int& year,
    int& month,
    int& day
)
{
    if (end - begin != 8) return false;

    year = details::atoi<int>(begin, begin + 4);
    month = details::atoi<int>(begin + 4, begin + 6);
    day = details::atoi<int>(begin + 6, begin + 8);

    return true;
}


/*
\brief Internal ascii-to-time conversion.

Parses ascii and returns a time.

\param begin Pointer to the beginning of the ascii string.
\param end Pointer to past-the-end of the ascii string.
\param[out] hour Hour.
\param[out] minute Minute.
\param[out] second Second.
\param[out] millisecond Millisecond.
\return True if successful and the out arguments were set.
*/
inline bool atotime(
    char const* begin,
    char const* end,
    int& hour,
    int& minute,
    int& second,
    int& millisecond
)
{
    if (end - begin != 8 && end - begin != 12) return false;

    hour = details::atoi<int>(begin, begin + 2);
    minute = details::atoi<int>(begin + 3, begin + 5);
    second = details::atoi<int>(begin + 6, begin + 8);

    if (end - begin == 12)
        millisecond = details::atoi<int>(begin + 9, begin + 12);
    else
        millisecond = 0;

    return true;
}

} // namespace details

/* @endcond*/

class message_reader;
class message_reader_const_iterator;

/*!
\brief FIX field value, weakly-typed as an array of chars, with type conversion methods.
*/
class field_value {
public:

    /*! \brief Pointer to the beginning of the field value. */
    char const* begin() const {
        return begin_;
    }

    /*! \brief Pointer to past-the-end of the field value. */
    char const* end() const {
        return end_;
    }

    /*! \brief Size of the field value, in bytes. */
    size_t size() const {
        return end_ - begin_;
    }


    /*!
    \brief True if the value of the field is equal to the C-string argument.
    */
    friend bool operator==(field_value const& that, char const* cstring) {
        return !strncmp(that.begin(), cstring, that.size()) && !cstring[that.size()];
    }


    /*!
    \brief True if the value of the field is equal to the C-string argument.
    */
    friend bool operator==(char const* cstring, field_value const& that) {
        return !strncmp(that.begin(), cstring, that.size()) && !cstring[that.size()];
    }

    /*!
    \brief True if the value of the field is equal to the string argument.
    */
    template<typename Char_type> friend bool operator==(field_value const& that, std::basic_string<Char_type> const& s) {
        return that.size() == s.size() && !strncmp(that.begin(), s.data(), that.size());
    }


    /*!
    \brief True if the value of the field is equal to the string argument.
    */
    template<typename Char_type> friend bool operator==(std::basic_string<Char_type> const& s, field_value const& that) {
        return that.size() == s.size() && !strncmp(that.begin(), s.data(), that.size());
    }

    /*! \name String Conversion Methods */
//@{

    /*!
    \brief Ascii value as std::string.

    \tparam Char_type Character type for the string.
    \return An std::string that contains a copy of the ascii value of the field.
    \throw std::bad_alloc
    */
    template<typename Char_type> std::basic_string<Char_type> as_string() const {
        return std::basic_string<Char_type>(begin(), end()); // the return value optimization
    }

    /*!
    \brief Ascii value as char.

    \return The first char of the ascii value of the field.
    */
    char as_char() const {
        return *begin();
    }

//@}


    /*! \name Decimal Float Conversion Methods */
//@{

    /*!
    \brief Ascii-to-decimal conversion.

    Converts an ascii string to a decimal float, of the form \htmlonly mantissa&times;10<sup>exponent</sup>\endhtmlonly.

    Non-normalized. The exponent will never be  greater than zero.
    If the decimal float is an integer, the exponent will be zero.

    \tparam Int_type Integer type for the mantissa and exponent.
    \param[out] mantissa Reference to storage for the mantissa of the decimal float to be returned.
    \param[out] exponent Reference to storage for the exponent of the decimal float to be returned.
    */
    template<typename Int_type> void as_decimal(Int_type& mantissa, Int_type& exponent) const {
        details::atod<Int_type>(begin(), end(), mantissa, exponent);
    }
//@}

private:
    template <typename Int_type, bool Is_signed_integer>
    struct as_int_selector {
    };

    template <typename Int_type>
    struct as_int_selector<Int_type, true> {
        static inline Int_type call_as_int(char const* begin, char const* end) {
            return details::atoi<Int_type>(begin, end);
        }
    };

    template <typename Int_type>
    struct as_int_selector<Int_type, false> {
        static inline Int_type call_as_int(char const* begin, char const* end) {
            return details::atou<Int_type>(begin, end);
        }
    };

public:

    /*! \name Integer Conversion Methods */
//@{

    /*!
    \brief Ascii-to-integer conversion.

    Parses ascii and returns an integer.

    \tparam Int_type The type of integer to be returned. May be an unsigned integer.
    \return The ascii field value represented as an integer of type Int_type.
    */
    template<typename Int_type> Int_type as_int() const {
        return as_int_selector<Int_type, std::numeric_limits<Int_type>::is_signed>::call_as_int(begin(), end());
    }
//@}

    /*! \name Date and Time Conversion Methods */
//@{


    /*!
    \brief Ascii-to-date conversion.

    Parses ascii and returns a LocalMktDate or UTCDate.

    \param[out] year Year.
    \param[out] month Month.
    \param[out] day Day.
    \return True if successful and the out arguments were set.
    */
    bool as_date(
        int& year,
        int& month,
        int& day
    ) const {
        return details::atodate(begin(), end(), year, month, day);
    }


    /*!
    \brief Ascii-to-month-year conversion.

    Parses ascii and returns a month-year.

    \param[out] year Year.
    \param[out] month Month.
    \return True if successful and the out arguments were set.
    */
    bool as_monthyear(
        int& year,
        int& month
    ) const {
        if (end() - begin() != 6) return false;

        year = details::atoi<int>(begin(), begin() + 4);
        month = details::atoi<int>(begin() + 4, begin() + 6);

        return true;
    }


    /*!
    \brief Ascii-to-time conversion.

    Parses ascii and returns a time.

    \param[out] hour Hour.
    \param[out] minute Minute.
    \param[out] second Second.
    \param[out] millisecond Millisecond.
    \return True if successful and the out arguments were set.
    */
    bool as_timeonly(
        int& hour,
        int& minute,
        int& second,
        int& millisecond
    ) const {
        return details::atotime(begin(), end(), hour, minute, second, millisecond);
    }

    /*!
    \brief Ascii-to-timestamp conversion.

    Parses ascii and returns a timestamp.

    \param[out] year Year.
    \param[out] month Month.
    \param[out] day Day.
    \param[out] hour Hour.
    \param[out] minute Minute.
    \param[out] second Second.
    \param[out] millisecond Millisecond.
    \return True if successful and the out arguments were set.
    */
    bool as_timestamp(
        int& year,
        int& month,
        int& day,
        int& hour,
        int& minute,
        int& second,
        int& millisecond
    ) const {
        return
            details::atotime(begin() + 9, end(), hour, minute, second, millisecond)
            && details::atodate(begin(), begin() + 8, year, month, day); // take advantage of short-circuit && to check field length
    }

//@}

#ifdef HFFIX_BOOST_DATETIME

    /*! \name Boost Date and Time Conversion Methods */
//@{

    /*!
    \brief Ascii-to-date conversion.

    Parses ascii and returns a LocalMktDate or UTCDate.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \return Date if parsing was successful, else boost::posix_time::not_a_date_time.
    */
    boost::gregorian::date as_date() const {
        int year, month, day;
        if (as_date(year, month, day)) {
            try {
                return boost::gregorian::date(year, month, day);
            } catch(std::exception& ex) {
                return boost::gregorian::date(boost::posix_time::not_a_date_time);
            }
        } else
            return boost::gregorian::date(boost::posix_time::not_a_date_time);
    }

    /*!
    \brief Ascii-to-time conversion.

    Parses ascii and returns a time.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \return Time if parsing was successful, else boost::posix_time::not_a_date_time.
    */
    boost::posix_time::time_duration as_timeonly() const {
        int hour, minute, second, millisecond;
        if (as_timeonly(hour, minute, second, millisecond)) {
            try {
                return boost::posix_time::time_duration(hour, minute, second, boost::posix_time::time_duration::ticks_per_second() * millisecond / 1000);
            } catch(std::exception& ex) {
                return boost::posix_time::time_duration(boost::posix_time::not_a_date_time);
            }
        } else
            return boost::posix_time::time_duration(boost::posix_time::not_a_date_time);
    }

    /*!
    \brief Ascii-to-timestamp conversion.

    Parses ascii and returns a timestamp.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \return Date and Time if parsing was successful, else boost::posix_time::not_a_date_time.
    */
    boost::posix_time::ptime as_timestamp() const {
        int year, month, day, hour, minute, second, millisecond;

        if (details::atotime(begin() + 9, end(), hour, minute, second, millisecond)
                && details::atodate(begin(), begin() + 8, year, month, day)) {
            try {
                return boost::posix_time::ptime(
                           boost::gregorian::date(year, month, day),
                           boost::posix_time::time_duration(hour, minute, second, boost::posix_time::time_duration::ticks_per_second() * millisecond / 1000)
                       );
            } catch(std::exception& ex) {
                return boost::posix_time::not_a_date_time;
            }
        } else
            return boost::posix_time::not_a_date_time;
    }
//@}


#endif // HFFIX_BOOST_DATETIME


private:
    friend class field;
    friend class message_reader_const_iterator;
    friend class message_reader;
    char const* begin_;
    char const* end_;
};

/*!
\brief A FIX field for reading, with tag and field value. This class is the hffix::message_reader::value_type for the hffix::message_reader Container.
*/
class field {
public:

    /*! \brief Tag of the field. */
    int tag() const {
        return tag_;
    }

    /*! \brief Weakly-typed value of the field. */
    field_value const& value() const {
        return value_;
    }

    /*! \brief Output stream operator. Output format is "[tag number]=[value]". */
    friend std::ostream& operator<<(std::ostream& os, field const& that) {
        os << that.tag_ << "=";
        return os.write(that.value_.begin(), that.value_.size());
    }

private:
    friend class message_reader_const_iterator;
    friend class message_reader;
    int tag_;
    field_value value_;
};

/*!
\brief hffix::message_reader::const_iterator.

Satisfies the const Input Iterator Concept for an immutable hffix::message_reader container of fields.
*/
class message_reader_const_iterator {

private:
    // there cannot be a default constructor because of the message_reader_ member.
    message_reader_const_iterator() {}

    message_reader_const_iterator(message_reader const& container, char const* buffer) :
        message_reader_(&container),
        buffer_(buffer),
        current_() {
    }

public:

    typedef ::std::input_iterator_tag iterator_category;
    typedef field value_type;
    typedef std::ptrdiff_t difference_type;
    typedef field* pointer;
    typedef field& reference;


    /*!
    \brief Returns a hffix::message_reader::const_reference to a field.
    */
    field const& operator*() const {
        return current_;
    }

    /*!
    \brief Returns a hffix::message_reader::const_pointer to a field.
    */
    field const* operator->() const {
        return &current_;
    }



    friend bool operator==(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ == b.buffer_;
    }

    friend bool operator!=(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ != b.buffer_;
    }

    friend bool operator<(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ < b.buffer_;
    }

    friend bool operator>(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ > b.buffer_;
    }

    friend bool operator<=(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ <= b.buffer_;
    }

    friend bool operator>=(message_reader_const_iterator const& a, message_reader_const_iterator const& b) {
        return a.buffer_ >= b.buffer_;
    }

    message_reader_const_iterator operator++(int) { //postfix
        message_reader_const_iterator i(*this);
        ++(*this);
        return i;
    }

    message_reader_const_iterator& operator++() { //prefix
        increment();
        return *this;
    }

protected:
    friend class message_reader;
    message_reader const* message_reader_;
    char const* buffer_;
    field current_;

    void increment();
};



/*!
\brief One FIX message for reading.

An immutable Forward Container of FIX fields. Given a buffer containing a FIX message, the hffix::message_reader
will provide an Iterator for iterating over the fields in the message without modifying the buffer. The buffer
used to construct the hffix::message_reader must outlive the hffix::message_reader.

The message_reader will only iterate over content fields of the message, and will skip over all of the meta-fields
for transport that are mixed in with the content fields in FIX. Here is the list of skipped fields:

- BeginString
- BodyLength
- CheckSum
- SecureDataLen
- RawDataLength
- XmlDataLen
- EncodedIssuerLen
- EncodedSecurityDescLen
- EncodedListExecInstLen
- EncodedTextLen
- EncodedSubjectLen
- EncodedHeadlineLen
- EncodedAllocTextLen
- EncodedUnderlyingIssuerLen
- EncodedUnderlyingSecurityDescLen
- EncodedListStatusTextLen

Fields of binary data type are content fields, and will be iterated over like any other field. The special FIX
binary data length fields will be skipped, but the length of the binary data is accessible from
hffix::message_reader::value_type::value().size().
*/
class message_reader {

public:

    typedef field value_type;
    typedef field const& const_reference;
    typedef message_reader_const_iterator const_iterator;
    typedef field const* const_pointer;
    typedef size_t size_type;

    /*!
    \brief Construct by buffer size.
    \param buffer Pointer to the buffer to be read.
    \param size Number of bytes in the buffer to be read.
    */
    message_reader(char const* buffer, size_t size) :
        buffer_(buffer),
        buffer_end_(buffer + size),
        begin_(*this, 0),
        end_(*this, 0),
        is_complete_(false),
        is_valid_(true) {
        init();
    }

    /*!
    \brief Construct by buffer begin and end.
    \param begin Pointer to the buffer to be read.
    \param end Pointer to past-the-end of the buffer to be read.
    */
    message_reader(char const* begin, char const* end) :
        buffer_(begin),
        buffer_end_(end),
        begin_(*this, 0),
        end_(*this, 0),
        is_complete_(false),
        is_valid_(true) {
        init();
    }
    /*!
    \brief Copy constructor. The hffix::message_reader is immutable, so copying it is fine.
    */
    message_reader(message_reader const& that) :
        buffer_(that.buffer_),
        buffer_end_(that.buffer_end_),
        begin_(that.begin_),
        end_(that.end_),
        is_complete_(that.is_complete_),
        is_valid_(that.is_valid_) {
    }

    ~message_reader() {
    }

    /*!
    \brief An iterator to the MsgType field in the FIX message. Same as hffix::message_reader::message_type().
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    const_iterator begin() const {
        // return iterator for beginning of nonmutable sequence
        if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
        return begin_;
    }

    /*!
    \brief An iterator to the CheckSum field in the FIX message. Same as hffix::message_reader::check_sum().
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    const_iterator end() const {
        // return iterator for end of nonmutable sequence
        if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
        return end_;
    }

    /*!
    \brief An iterator to the MsgType field in the FIX message. Same as hffix::message_reader::begin().
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    const_iterator message_type() const {
        // return iterator for beginning of nonmutable sequence
        if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
        return begin_;
    }

    /*!
    \brief An iterator to the CheckSum field in the FIX message. Same as hffix::message_reader::end().
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    const_iterator check_sum() const {
        // return iterator for end of nonmutable sequence
        if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
        return end_;
    }

    /*!
    \brief A pointer to the begining of the buffer.
    buffer_begin() == message_begin()
    */
    char const* buffer_begin() const {
        return buffer_;
    }
    /*!
    \brief A pointer to past-the-end of the buffer.
    */
    char const* buffer_end() const {
        return buffer_end_;
    }

    /*!
    \brief The size of the buffer in bytes.
    */
    size_t buffer_size() const {
        return buffer_end_ - buffer_;
    }

    /*!
    \brief A pointer to the beginning of the FIX message in the buffer.
     buffer_begin() == message_begin()
    */
    char const* message_begin() const {
        return buffer_;
    }
    /*!
    \brief A pointer to past-the-end of the FIX message in the buffer.
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    char const* message_end() const {
        if (!is_valid_) throw std::logic_error("hffix Cannot determine size of an invalid message.");
        return end_.current_.value_.end_ + 1;
    }

    /*!
    \brief The entire size of the FIX 4.1 message in bytes.
    \throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
    */
    size_t message_size() const {
        if (!is_valid_) throw std::logic_error("hffix Cannot determine size of an invalid message.");
        return end_.current_.value_.end_ - buffer_ + 1;
    }

    /*!
    \brief True if the buffer contains a complete FIX message.
    */
    bool is_complete() const {
        return is_complete_;
    }
    /*!
    \brief True if the message is correct FIX.

    If false, there was an error parsing the FIX message and the message cannot be read.

    fix-42_with_errata_20010501.pdf p.17:
    "Valid FIX Message is a message that is properly formed according to this specification and contains a
    valid body length and checksum field"
    */
    bool is_valid() const {
        return is_valid_;
    }


    /*!
    \brief Returns a new message_reader for the next FIX message in the buffer.

    If this message is_valid() and is_complete(), assume that the next message comes immediately after this one and return a new message_reader constructed at this->message_end().

    If this message !is_valid(), will search the remainder of the buffer
    for the text "8=FIX", to see if there might be a complete or partial valid message
    anywhere else in the remainder of the buffer, will return a new message_reader constructed at that location.

    If this message !is_complete(), will throw std::logic_error.

    */
    message_reader next_message_reader() const {
        if (!is_complete_) {
            throw std::logic_error("Can't call next_message_reader on an incomplete message.");
        }

        if (!is_valid_) { // this message isn't valid, so we have to try to search for the beginning of the next message.
            char const* b = buffer_ + 1;
            while(b < buffer_end_ - 10) {
                if (!std::memcmp(b, "8=FIX", 5))
                    break;
                ++b;
            }
            return message_reader(b, buffer_end_); 
        }

        return message_reader(end_.current_.value_.end_ + 1, buffer_end_);
    }

    /*!
     * \brief Returns the FIX version prefix string begin pointer. (Example: "FIX.4.4")
     */
    char const* prefix_begin() const {
        return buffer_ + 2;
    }

    /*!
     * \brief Returns the FIX version prefix string end pointer. (Example: "FIX.4.4")
     */
    char const* prefix_end() const {
        return prefix_end_;
    }

    /*!
     * \brief Returns the FIX version prefix string size. (Example: returns 7 for "FIX.4.4")
     */
    ssize_t prefix_size() const {
        return prefix_end_ - buffer_ - 2;
    }

private:
    friend class message_reader_const_iterator;

    void init() {

        // Skip the version prefix string "8=FIX.4.2" or "8=FIXT.1.1", et cetera.
        char const* b = buffer_ + 9; // look for the first '\x01'

        while(true) {
            if (b >= buffer_end_) {
                is_complete_ = false;
                return;
            }
            if (*b == '\x01') {
                prefix_end_ = b;
                break;
            }
            if (b - buffer_ > 11) {
                invalid();
                return;
            }
            ++b;
        }

        if (b + 1 >= buffer_end_) {
            is_complete_ = false;
            return;
        }
        if (b[1] != '9') { // next field must be tag 9 BodyLength
            invalid();
            return;
        }
        b += 3; // skip the " 9=" for tag 9 BodyLength

        size_t bodylength(0); // the value of tag 9 BodyLength

        while(true) {
            if (b >= buffer_end_) {
                is_complete_ = false;
                return;
            }
            if (*b == '\x01') break;
            if (*b < '0' || *b > '9') { // this is the only time we need to check for numeric ascii.
                invalid();
                return;
            }
            bodylength *= 10;
            bodylength += *b++ - '0'; // we know that 0 <= (*b - '0') <= 9, so rvalue will be positive.
        }

        ++b;
        if (b + 3 >= buffer_end_) {
            is_complete_ = false;
            return;
        }

        if (*b != '3' || b[1] != '5') { // next field must be tag 35 MsgType
            invalid();
            return;
        }

        char const* checksum = b + bodylength;

        if (checksum + 7 > buffer_end_) {
            is_complete_ = false;
            return;
        }

        if (*(checksum + 6) != '\x01') { // check for trailing SOH
            invalid();
            return;
        }

        begin_.buffer_ = b;
        begin_.current_.tag_ = 35; // MsgType
        b += 3;
        begin_.current_.value_.begin_ = b;
        while(*++b != '\x01') {
            if (b >= checksum) {
                invalid();
                return;
            }
        }
        begin_.current_.value_.end_ = b;

        end_.buffer_ = checksum;
        end_.current_.tag_ = 10; //CheckSum
        end_.current_.value_.begin_ = checksum + 3;
        end_.current_.value_.end_ = checksum + 6;

        is_complete_ = true;
    }

    char const* buffer_;
    char const* buffer_end_;
    const_iterator begin_;
    const_iterator end_;
    value_type message_type_;
    bool is_complete_;
    bool is_valid_;
    char const* prefix_end_; // Points after the 8=FIX... Prefix field.

    void invalid() {
        is_complete_ = true; // invalid messages are considered complete, for use of the message_reader::operator++()
        is_valid_ = false;
    }
};


/*! @cond EXCLUDE */
namespace details {
bool is_tag_a_data_length(int tag);
}
/*! @endcond */


inline void message_reader_const_iterator::increment()
{
    buffer_ = current_.value_.end_ + 1;
    current_.value_.begin_ = buffer_;
    current_.tag_ = 0;

    while(*current_.value_.begin_ != '=') {
        current_.tag_ *= 10;
        current_.tag_ += (*current_.value_.begin_ - '0');
        ++current_.value_.begin_;
    }

    current_.value_.end_ = ++current_.value_.begin_;

    while(*(++current_.value_.end_ ) != '\x01') {}

    if (details::is_tag_a_data_length(current_.tag_)) {
        size_t data_len = details::atou<size_t>(current_.value_.begin_, current_.value_.end_);

        buffer_ = current_.value_.end_ + 1;
        current_.value_.begin_ = buffer_;
        current_.tag_ = 0;

        while(*current_.value_.begin_ != '=') {
            current_.tag_ *= 10;
            current_.tag_ += (*current_.value_.begin_ - '0');
            ++current_.value_.begin_;
        }

        current_.value_.end_ = ++current_.value_.begin_ + data_len;

        // TODO: eliminate this check, along with the message_reader_ member?
        // if (current_.value_.end_ >= message_reader_->end_.buffer_) {
        //     message_reader_->invalid(); // in theory this can never happen.
        // }
    }
};


/*!
\brief One FIX message for writing.

Given a buffer, the message_writer will write a FIX message to the buffer. The interface is patterned after
Back Insertion Sequence Containers, with overloads of push_back for different FIX field data types.

The push_back_header() method will write the BeginString and BodyLength fields to the message,
but the FIX Standard Message Header requires also MsgType, SenderCompID, TargetCompID, MsgSeqNum and SendingTime.
You must write those fields yourself, starting with MsgType.

After calling all other push_back methods and before sending the message, you must call push_back_trailer(),
which will write the CheckSum field for you.

*/
class message_writer {
public:

    /*!
    \brief Construct by buffer size.
    \param buffer Pointer to the buffer to be written to.
    \param size Size of the buffer in bytes.
    */
    message_writer(char* buffer, size_t size):
        buffer_(buffer),
        buffer_end_(buffer + size),
        next_(buffer) {
    }

    /*!
    \brief Construct by buffer begin and end.
    \param begin Pointer to the buffer to be written to.
    \param end Pointer to past-the-end of the buffer to be written to.
    */
    message_writer(char* begin, char* end) :
        buffer_(begin),
        buffer_end_(end),
        next_(begin) {
    }

    /*!
    \brief Write the BeginString and BodyLength fields to the buffer.

    This method must be called before any other push_back() method. It may only be called once for each message_writer.

    \pre No other push_back method has yet been called.
    \param begin_string_version The value for the BeginString FIX field. Should probably be "FIX.4.2" or "FIX.4.3" or "FIX.4.4" or "FIXT.1.1" (for FIX 5.0).
    */
    void push_back_header(char const* begin_string_version) {
        memcpy(next_, "8=", 2);
        next_ += 2;
        memcpy(next_, begin_string_version, std::strlen(begin_string_version) - 1);
        next_ += std::strlen(begin_string_version) - 1;
        *(next_++) = '\x01';
        memcpy(next_, "9=", 2);
        next_ += 2;
        body_length_ = next_;
        next_ += 6; // 6 characters reserved for BodyLength.
        *next_++ = '\x01';
    }
    /*!
    \brief Write the CheckSum field to the buffer.

    This function must be called after all other push_back functions. It may only be called once for each message_writer.

    \post There is a complete and valid FIX message in the buffer.

    \param calculate_checksum If this flag is set to false, then instead of iterating over the entire message and
    calculating the CheckSum, the standard trailer will simply write CheckSum=000. This is fine if you're sending
    the message to a FIX parser that, like High Frequency FIX Parser, doesn't care about the CheckSum.
    */
    void push_back_trailer(bool calculate_checksum = true) {
        // calculate and write out the BodyLength
        size_t bodylength = next_ - (buffer_ + 17);
        for(char* b = body_length_ + 5; b >= body_length_; --b) {
            *b = '0' + (bodylength % 10);
            bodylength /= 10;
        }

        // write out the CheckSum after optionally calculating it
        if (calculate_checksum) {
            size_t checksum(0);
            char* b = buffer_;
            while(b < next_) checksum += *b++;
            checksum = checksum % 256;

            memcpy(next_, "10=", 3);
            next_ += 3;

            for (char* b = next_ + 2; b >= next_; --b) {
                *b = '0' + (checksum % 10);
                checksum /= 10;
            }
            next_ += 3;
            *next_++ = '\x01';
        } else {
            memcpy(next_, "10=000\x01", 7);
            next_ += 7;
        }

    }

    /*!
    \brief Size of the message in bytes.
    */
    size_t message_size() const {
        return next_ - buffer_;
    }

    /*!
    \brief Pointer to beginning of the message.
    */
    char* message_begin() const {
        return buffer_;
    }
    /*!
    \brief Pointer to past-the-end of the message.
    */
    char* message_end() const {
        return next_;
    }


    /*! \name String Methods */
//@{
    /*!
    \brief Append a string field to the message.
    \param tag FIX tag.
    \param begin Pointer to the beginning of the string.
    \param end Pointer to past-the-end of the string.
    */
    void push_back_string(int tag, char const* begin, char const* end) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        memcpy(next_, begin, end - begin);
        next_ += (end - begin);
        *next_++ = '\x01';
    }

    /*!
    \brief Append a string field to the message.
    \param tag FIX tag.
    \param cstring Pointer to the beginning of a C-style null-terminated string.
    */
    void push_back_string(int tag, char const* cstring) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        while(*cstring) *next_++ = *cstring++;
        *next_++ = '\x01';
    }

    /*!
    \brief Append a string field to the message.

    The entire, literal contents of s will be copied to the output buffer, so if you are using std::wstring
    you may need to first convert from UTF-16 to Ascii, or do some other encoding transformation.

    \tparam Char_type Character type for the string. Your complier will usually infer the Char_type, so you
    can elide this parameter.

    \param tag FIX tag.
    \param s String.
    */
    template<typename Char_type> void push_back_string(int tag, std::basic_string<Char_type> const& s) {
        push_back_string(tag, s.data(), s.data() + s.length());
    }


    /*!
    \brief Append a char field to the message.
    \param tag FIX tag.
    \param character An ascii character.
    */
    void push_back_char(int tag, char character) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        *next_++ = character;
        *next_++ = '\x01';
    }
//@}


    /*! \name Integer Methods */
//@{
    /*!
    \brief Append an integer field to the message.
    \tparam Int_type Type of integer.
    \param tag FIX tag.
    \param number Integer value.
    */
    template<typename Int_type> void push_back_int(int tag, Int_type number) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        next_ = details::itoa(number, next_);
        *next_++ = '\x01';
    }
    /*
    \brief Append an unsigned integer field to the message.
    \tparam Uint_type Type of unsigned integer.
    \param tag FIX tag.
    \param number Unsigned integer value.
    */
    //template<typename Uint_type> void push_back_uint(int tag, Uint_type number)
    //{
    //    next_ = details::itoa(tag, next_);
    //    *next_++ = '=';
    //    next_ = details::utoa(number, next_);
    //    *next_++ = '\x01';
    //}
//@}

    /*! \name Decimal Float Methods */
//@{

    /*!
    \brief Append a decimal float field to the message.

    The decimal float is of the form \htmlonly mantissa&times;10<sup>exponent</sup>\endhtmlonly.

    Non-normalized. The exponent parameter must be less than or equal to zero. If the exponent
    parameter is zero, no decimal point will be written to the ascii field.

    \tparam Int_type Integer type for the mantissa and exponent.
    \param tag FIX tag.
    \param mantissa The mantissa of the decimal float.
    \param exponent The exponent of the decimal float. Must be less than or equal to zero.
    */
    template<typename Int_type> void push_back_decimal(int tag, Int_type mantissa, Int_type exponent) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        next_ = details::dtoa(mantissa, exponent, next_);
        *next_++ = '\x01';
    }
//@}


    /*! \name Date and Time Methods */
//@{

    /*!
    \brief Append a LocalMktDate or UTCDate field to the message.
    \param tag FIX tag.
    \param year Year.
    \param month Month.
    \param day Day.
    */
    void push_back_date(int tag, int year, int month, int day) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(year, next_, next_ + 4);
        next_ += 4;
        itoa_padded(month, next_, next_ + 2);
        next_ += 2;
        itoa_padded(day, next_, next_ + 2);
        next_ += 2;
        *next_++ = '\x01';
    }
    /*!
    \brief Append a month-year field to the message.
    \param tag FIX tag.
    \param year Year.
    \param month Month.
    */
    void push_back_monthyear(int tag, int year, int month) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(year, next_, next_ + 4);
        next_ += 4;
        itoa_padded(month, next_, next_ + 2);
        next_ += 2;
        *next_++ = '\x01';
    }

    /*!
    \brief Append a UTCTimeOnly field to the message.

    No time zone or daylight savings time transformations are done to the time.

    No fractional seconds are written to the field.

    \param tag FIX tag.
    \param hour Hour.
    \param minute Minute.
    \param second Second.
    */
    void push_back_timeonly(int tag, int hour, int minute, int second) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(hour, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(minute, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(second, next_, next_ + 2);
        next_ += 2;
        *next_++ = '\x01';
    }

    /*!
    \brief Append a UTCTimeOnly field to the message.

    No time zone or daylight savings time transformations are done to the time.

    \param tag FIX tag.
    \param hour Hour.
    \param minute Minute.
    \param second Second.
    \param millisecond Millisecond.
    */
    void push_back_timeonly(int tag, int hour, int minute, int second, int millisecond) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(hour, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(minute, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(second, next_, next_ + 2);
        next_ += 2;
        *next_++ = '.';
        itoa_padded(millisecond, next_, next_ + 3);
        next_ += 3;
        *next_++ = '\x01';
    }

    /*!
    \brief Append a UTCTimestamp field to the message.

    No time zone or daylight savings time transformations are done to the timestamp.

    No fractional seconds are written to the field.

    \param tag FIX tag.
    \param year Year.
    \param month Month.
    \param day Day.
    \param hour Hour.
    \param minute Minute.
    \param second Second.
    */
    void push_back_timestamp(int tag, int year, int month, int day, int hour, int minute, int second) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(year, next_, next_ + 4);
        next_ += 4;
        itoa_padded(month, next_, next_ + 2);
        next_ += 2;
        itoa_padded(day, next_, next_ + 2);
        next_ += 2;
        *next_++ = '-';
        itoa_padded(hour, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(minute, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(second, next_, next_ + 2);
        next_ += 2;
        *next_++ = '\x01';
    }

    /*!
    \brief Append a UTCTimestamp field to the message.

    No time zone or daylight savings time transformations are done to the timestamp.

    \param tag FIX tag.
    \param year Year.
    \param month Month.
    \param day Day.
    \param hour Hour.
    \param minute Minute.
    \param second Second.
    \param millisecond Millisecond.
    */
    void push_back_timestamp(int tag, int year, int month, int day, int hour, int minute, int second, int millisecond) {
        next_ = details::itoa(tag, next_);
        *next_++ = '=';
        itoa_padded(year, next_, next_ + 4);
        next_ += 4;
        itoa_padded(month, next_, next_ + 2);
        next_ += 2;
        itoa_padded(day, next_, next_ + 2);
        next_ += 2;
        *next_++ = '-';
        itoa_padded(hour, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(minute, next_, next_ + 2);
        next_ += 2;
        *next_++ = ':';
        itoa_padded(second, next_, next_ + 2);
        next_ += 2;
        *next_++ = '.';
        itoa_padded(millisecond, next_, next_ + 3);
        next_ += 3;
        *next_++ = '\x01';
    }

//@}

#ifdef HFFIX_BOOST_DATETIME

    /*! \name Boost Date and Time Methods */
//@{

    /*!
    \brief Append a LocalMktDate or UTCDate field to the message.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \param tag FIX tag.
    \param date Date.
    \see HFFIX_NO_BOOST_DATETIME
    */
    void push_back_date(int tag, boost::gregorian::date date) {
        if (!date.is_not_a_date())
            push_back_date(tag, date.year(), date.month(), date.day());
    }

    /*!
    \brief Append a UTCTimeOnly field to the message.

    No time zone or daylight savings time transformations are done to the time.

    Fractional seconds will be written to the field, rounded to the millisecond.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \param tag FIX tag.
    \param timeonly Time.
    \see HFFIX_NO_BOOST_DATETIME
    */
    void push_back_timeonly(int tag, boost::posix_time::time_duration timeonly) {
        if (!timeonly.is_not_a_date_time())
            push_back_timeonly(
                tag,
                timeonly.hours(),
                timeonly.minutes(),
                timeonly.seconds(),
                int(timeonly.fractional_seconds() * 1000 / boost::posix_time::time_duration::ticks_per_second())
            );
    }

    /*!
    \brief Append a UTCTimestamp field to the message.

    No time zone or daylight savings time transformations are done to the timestamp.

    Fractional seconds will be written to the field, rounded to the millisecond.

    Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

    \param tag FIX tag.
    \param timestamp Date and time.
    \see HFFIX_NO_BOOST_DATETIME
    */
    void push_back_timestamp(int tag, boost::posix_time::ptime timestamp) {
        if (!timestamp.is_not_a_date_time())
            push_back_timestamp(
                tag,
                timestamp.date().year(),
                timestamp.date().month(),
                timestamp.date().day(),
                timestamp.time_of_day().hours(),
                timestamp.time_of_day().minutes(),
                timestamp.time_of_day().seconds(),
                int(timestamp.time_of_day().fractional_seconds() * 1000 / boost::posix_time::time_duration::ticks_per_second())
            );
    }
//@}
#endif // HFFIX_BOOST_DATETIME


    /*! \name Data Methods */
//@{

    /*!
    \brief Append a data length field and a data field to the message.

    Note that this method will append two fields to the message. The first field is an integer equal to
    the content length of the second field. FIX does this so that the content of the second field may contain
    Ascii NULL or SOH or other control characters.

    High Frequency FIX Parser calculates the content length for you
    and writes out both fields, you just have to provide both tags and pointers to the data.
    For most of the data fields in FIX, it is true that tag_data = tag_data_length + 1, but we daren't assume that.

    Example:
    \code
    hffix::message_writer r;
    std::string data("Some data.");
    r.push_back_data(hffix::tag::RawDataLength, hffix::tag::RawData, data.begin(), data.end());
    \endcode

    \param tag_data_length FIX tag for the data length field.
    \param tag_data FIX tag for the data field.
    \param begin Pointer to the beginning of the data.
    \param end Pointer to after-the-end of the data.
    */
    void push_back_data(int tag_data_length, int tag_data, char const* begin, char const* end) {
        next_ = details::itoa(tag_data_length, next_);
        *next_++ = '=';
        next_ = details::itoa(end - begin, next_);
        *next_++ = '\x01';
        next_ = details::itoa(tag_data, next_);
        *next_++ = '=';
        memcpy(next_, begin, end - begin);
        next_ += end - begin;
        *next_++ = '\x01';
    }


//@}
private:
    static void itoa_padded(int x, char* b, char* e) {
        while (e > b) {
            *--e = '0' + (x % 10);
            x /= 10;
        }
    }

    char* buffer_;
    char* buffer_end_; // TODO check in all methods for overflow of buffer and throw.
    char* next_;
    char* body_length_; // Pointer to the location at which the BodyLength should be written, once the length of the message is known. 6 chars, which allows for messagelength up to 999,999.
};

/* @cond EXCLUDE */

namespace details {
inline bool is_tag_a_data_length(int tag)
{
    int* length_fields_end = length_fields + (sizeof(length_fields)/sizeof(length_fields[0]));
    return std::find(length_fields, length_fields_end, tag) != length_fields_end; // fields are ordered, so this could be std::binary_search.
}

// \brief std::ostream-able type returned by hffix::field_name function.
template <typename AssociativeContainer> struct field_name_streamer {
    int tag;
    AssociativeContainer const& field_dictionary;
    bool number_alternative;

    field_name_streamer(int tag, AssociativeContainer const& field_dictionary, bool number_alternative) : tag(tag), field_dictionary(field_dictionary), number_alternative(number_alternative) {}

    friend std::ostream& operator<<(std::ostream& os, field_name_streamer that) {
        typename AssociativeContainer::const_iterator i = that.field_dictionary.find(that.tag);
        if (i != that.field_dictionary.end())
            os << i->second;
        else if (that.number_alternative)
            os << that.tag;
    }
};

}
/* @endcond */

/*!
 \brief Given a field tag number and a field name dictionary, returns a type which provides <tt>operator<< </tt> to write the name of the field to an std::ostream.
 \tparam AssociativeContainer The type of the field name dictionary. Must satisfy concept <tt>AssociativeContainer<int, std::string></tt>, for example <tt>std::map<int, std::string></tt> or <tt>std::unordered_map<int, std::string></tt>. See http://en.cppreference.com/w/cpp/concept/AssociativeContainer

 \param tag The field number.
 \param field_dictionary The field dictionary.
 \param or_number Specifies behavior if the tag is not found in the dictionary. If true, then the string representation of the flag will be written to the std::ostream. If false, then nothing will be written to the std::ostream. Default is false.

 Example usage:
 \code
 std::map<int, std::string> dictionary;
 hffix::field_dictionary_init(dictionary);
 std::cout << field_name(hffix::tag::SenderCompID, dictionary) << '\n'; // Will print "SenderCompID\n".
 std::cout << field_name(1000000, dictionary) << '\n'; // Will print "\n".
 std::cout << field_name(1000000, dictionary, true) << '\n'; // Will print "1000000\n".
 \endcode
*/
template <typename AssociativeContainer> details::field_name_streamer<AssociativeContainer> field_name(int tag, AssociativeContainer const& field_dictionary, bool or_number = false)
{
    return details::field_name_streamer<AssociativeContainer>(tag, field_dictionary, or_number);
};

/*!
 * \brief A predicate constructed with a FIX tag which returns true if the tag of the field passed to the predicate is equal.
 */
struct tag_predicate {
    tag_predicate(int tag) : tag(tag) {}
    int tag;
    inline bool operator()(message_reader::value_type const& v) const {
        return v.tag() == tag;
    }
};


/*!
 * \brief An algorithm for forward-searching over a range and finding items which match a predicate, when the expected ordering of the items is known. Searches from begin to end, but starts at i. If end is reached, then wraps around and finishes at i.
 *
 * This expression:
 * \code
 * find_with_hint(begin, end, i, predicate)
 * \endcode
 * will behave exactly the same as this expression:
 * \code
 * end != (i = std::find_if(begin, end, predicate))
 * \endcode
 * except for these two differences:
 * * In the first expression, i is not modifed if no item is found.
 * * The first expression is faster if the found item is a near successor of i.
 *
 * Example usage:
 * \code
 * hffix::message_reader::const_iterator i = reader.begin();
 * if (hffix::find_with_hint(reader.begin(), reader.end(), i, hffix::tag_predicate(MsgSeqNum)) {
 *   int seqnum = i->as_int<int>();
 * }
 * if (hffix::find_with_hint(reader.begin(), reader.end(), i, hffix::tag_predicate(TargetCompID)) {
 *   std::string targetcompid = i->as_string();
 * }
 * \endcode
 *
 * \param begin The beginning of the range to search.
 * \param end The end of the range to search.
 * \param i If an item is found which matches predicate, then i is modified to point to the found item. Else i is unmodified.
 * \param predicate A predicate which provides function <code>bool operator() (ForwardIterator::value_type const &v) const</code>.
 * \return True if a field was found which matched prediate, and i was modified to point to the found field.
 */
template <typename ForwardIterator, typename Predicate> 
bool find_with_hint(ForwardIterator begin, ForwardIterator end, ForwardIterator & i, Predicate predicate) {
    ForwardIterator j = std::find_if(i, end, predicate);
    if (j != end) {
        i = j;
        return true;
    }
    j = std::find_if(begin, i, predicate);
    if (j != i) {
        i = j;
        return true;
    }
    return false;
}

} // namespace hffix

#endif
