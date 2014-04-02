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
\brief The entire High Frequency FIX Parser library. 

Just include "hffix.hpp" to use High
Frequency FIX in your project.
*/

/*! \mainpage High Frequency FIX Parser (Financial Information Exchange 4.2)

The High Frequency FIX Parser library is an open source implementation of 
<a href="http://www.fixprotocol.org/specifications/FIX.4.2">Financial Information Exchange protocol version 4.2</a> 
intended for use by developers of high frequency, low latency financial software. 
The purpose of the library is to do fast, efficient encoding and decoding of FIX 4.2 in place, 
at the location of the I/O buffer, without using intermediate objects or allocating on the free store.

High Frequency FIX Parser follows the 
<a href="http://www.boost.org/development/requirements.html">Boost Library Requirements and Guidelines</a>. 
It is modern platform-independent C++98 and depends only on the C++ Standard Library. 
It is patterned after the C++ Standard Template Library, and it models each FIX 4.2 message with 
Container and Iterator concepts. It employs compile-time polymorphism (templates) but not
run-time polymorphism (object-oriented inheritance).

High Frequency FIX Parser is a one-header-only library, so there are no binaries to link. Just 
<tt>#include "hffix.hpp"</tt> to use High Frequency FIX Parser in your application.
It also plays well with Boost. If you are using 
<a href="http://www.boost.org/doc/html/date_time.html">Boost Date_Time</a> in your 
application, High Frequency FIX Parser will support conversion between FIX 4.2 fields
and Boost Date_Time types.

The design criteria for High Frequency FIX Parser are based on our experience passing messages to various FIX 4.2 hosts
for high frequency quantitative trading at <a href="http://www.t3live.com">T3 Trading Group, LLC</a>.

High Frequency FIX Parser was developed with \em fix-42_with_errata_20010501.pdf. A copy of that document can be obtained
after registering at the <a href="http://www.fixprotocol.org">FIX Protocol Limited</a> website.

High Frequency FIX Parser is distributed under the open source FreeBSD License, also known as the Simplified BSD License.

The <a href="http://www.t3tradinggroup.com/hffix">High Frequency FIX home page</a> is here.

For support and discussion of High Frequency FIX Parser, see the
<a href="http://groups.google.com/group/high-frequency-fix-support">high-frequency-fix-support Google Group</a>.

\section difference Features

\subsection others Other FIX 4.2 Implementations

Typical FIX 4.2 implementations model a FIX 4.2 message either as an associative key-value container of
strongly typed objects that inherit from some field superclass, 
or as a class type for each message type with member variables for every possible field in the message. Both of
these approaches are inflexible and inefficient. The advantage of doing it this way is that you get 
random access reads and writes to the fields of the FIX 4.2 message.

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
as <tt>const char* begin(), const char* end()</tt>, and High Frequency FIX Parser provides 
a complete set of conversion functions for ints, 
decimal floats, dates and times, et cetera for the developer to call at his discretion.

\section session FIX 4.2 Sessions

Managing sessions requires making choices about sockets and threads. 
High Frequency FIX Parser does not manage sessions. 
It is intended for developers who want a FIX 4.2 parser with which to build a session manager
for a high-performance trading system that already has a carefully crafted socket and 
threading architecture.

FIX 4.2 has transport-layer features mixed in with the messages, and most FIX 4.2 hosts
have various quirks in the way they employ the administrative messages. To manage a FIX 4.2 
session your application will need to match the the transport-layer and administrative features 
of the other FIX 4.2 host. High Frequency FIX Parser has the flexibility to express any subset or
proprietary superset of FIX 4.2.

\subsection encryption Encryption

High Frequency FIX Parser supports the binary data field types such as SecureData, but it does not
implement any of the EncryptMethods suggested by \em fix-42_with_errata_20010501.pdf p.162.

If you want to encrypt or decrypt some data you'll have to do the encryption or decryption yourself. Once you've
created the functions for encryption and decryption, it is easy to
use High Frequency FIX Parser to read and write encrypted data within FIX 4.2 messages.

\subsection checksum CheckSum

High Frequency FIX Parser can calculate CheckSums and add the CheckSum field for all messages that you encode. 
It does not validate the CheckSum of messages decoded.

\subsection sequence Sequence Numbers

The MsgSeqNum field in the FIX 4.2 Standard Header is exposed for reading and writing.

\subsection administrative Administrative Messages

The administrative messages Logon, Logout, ResendRequest, 
Heartbeat, TestRequest, SeqReset-Reset and SeqReset-GapFill don't get special treatment in High Frequency FIX Parser. 
Any administrative message
can be encoded or decoded like any other message.

\subsection customtags User-Defined Fields and Custom Tags

High Frequency FIX Parser does not enforce the data type ("Format") of the Field Definitions for 
content fields in the FIX 4.2 spec, so the developer is free to read or write any field data type with any tag number.

\section usage Using High Frequency FIX Parser

\subsection Exceptions

All High Frequency FIX Parser methods, functions, constructors, and destructors provide the No-Throw exception guarantee
unless they are documented to throw exceptions, in which case they provide the Basic exception guarantee.

\subsection Threads

Hight Frequency FIX Parser is not thread-aware at all and has no threads, mutexes, or atomic operations.

All const methods of the hffix::message_reader are thread safe. The hffix::message_reader::operator++(),
which moves the hffix::message_reader to the next message in the buffer, is not const and is not thread safe.

The hffix::message_writer is not thread safe. 

The hffix::tag_name_dictionary::lookup() method and hffix::tag_name_dictionary::operator[] operator are const 
and are thread safe, 
unless they are used concurrently with any other hffix::tag_name_dictionary methods, which might modify
the collection and are not thread safe.

hffix::message_reader and hffix::message_writer have no storage of their own, they read and write fields directly
on the buffer with which they are constructed. You must be sure the buffer endures until they are destroyed.

\subsection reading Reading a FIX 4.2 Message

Here is a simple example of how use hffix::message_reader 
to read a FIX 4.2 message from a buffer and print each field to \c std::cout.

During construction, hffix::message_reader checks to make sure there is a complete,
valid FIX 4.2 message in the buffer. It looks only at the header and trailer transport fields in the message,
not at the content fields, so construction is O(1).

If hffix::message_reader is complete and valid after construction,
hffix::message_reader::begin() returns an iterator that points to the MsgType field
in the FIX 4.2 Standard Message Header, and 
hffix::message_reader::end() returns an iterator that points to the CheckSum field in the 
FIX 4.2 Standard Message Trailer.

\code
#include "hffix.hpp"

char buffer[512]; // Suppose we just read a 75-byte message into this buffer.

hffix::tag_name_dictionary dictionary; // For looking up the string field name of the tag.

hffix::message_reader reader(buffer, buffer + 75);

if (reader.is_complete() && reader.is_valid())
{
    std::cout << "Read " << reader.message_size() << " bytes from buffer." << std::endl;

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

\subsection writing Writing a FIX 4.2 Message

Here is a simple example of how to use hffix::message_writer to write a FIX 4.2 Heartbeat message to a buffer.
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

std::cout.write(writer.message_begin(), writer.message_size()) << std::endl;
std::cout << "Wrote " << writer.message_size() << " bytes to buffer." << std::endl;
\endcode

Example Output:

\code
8=FIX.4.2☺9=0051☺35=0☺49=AAAA☺56=BBBB☺34=1☺52=20110130-19:14:21.056☺10=049☺
Wrote 75 bytes to buffer.
\endcode

\subsection example A Complete Example

Here is a more complete example that shows reading, writing, and type conversions.

\code

// #define HFFIX_NO_BOOST_DATETIME
#include "hffix.hpp"

#include <iostream>
#include <string>

// For convenience, make a Meyers' Singleton instance of an hffix::tag_name_dictionary, 
// so that we can look up the names of FIX tags from anywhere in the program.
hffix::tag_name_dictionary& dictionary_singleton()
{
    static hffix::tag_name_dictionary instance;
    return instance;
}

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

int main(int argc, const char** argv)
{
    long long sequence_number_send(0); // can be any integer type
    long long sequence_number_recv(0); // can be any integer type
    
    // Create a buffer.
    char buffer[512];


    // We'll put a FIX 4.2 Logon message in the buffer.
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
    std::cout << "Wrote Logon to the buffer, " << logon_message.message_size() << " bytes." << std::endl;

    // We can print the FIX 4.2 Logon message to std::cout. It contains lots of non-printing
    // characters, so it'll look strange. At this point we could also send it out a network socket.
    std::cout << std::string(logon_message.message_begin(), logon_message.message_end()) << std::endl;

    



    // Add a FIX 4.2 New Order - Single message to the buffer, after the Logon message.
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
    std::cout << "Wrote New Order to the buffer, " << new_order_message.message_size() << " bytes." << std::endl;

    // Print the FIX 4.2 New Order message to std::cout. 
    std::cout << std::string(new_order_message.message_begin(), new_order_message.message_end()) << std::endl;





    //Now we'll parse the FIX 4.2 messages out of the buffer that we've just written.

    hffix::message_reader reader(buffer, new_order_message.message_end());

    // Try to read as many complete and valid messages as are in the buffer.
    for (; reader.is_complete(); ++reader)
    {
        if (reader.is_valid())
        {
            std::cout << "Reading message of size " << reader.message_size() << " bytes." << std::endl;
        
            // Get a peek at this message before we parse it by printing the raw FIX 4.2 to std::cout.
            std::cout << std::string(reader.message_begin(), reader.message_end()) << std::endl;

            // Check if this is a New Order MsgType. 
            // We can't use switch(reader.message_type()->value().as_char()) here because 
            // some FIX 4.2 message types have multiple characters.
            // Use the overloaded hffix::operator==(const message_reader_value_type_value &, const char *) instead.
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

                std::cout << sender_comp_id << " has sent a Logon message." << std::endl;
            }

            else // This is a MsgType that we weren't expecting to see.
            {
                std::cout << "Unexpected message of type " << reader.message_type()->value().as_string<char>() << std::endl;

                for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
                    std::cout << dictionary_singleton()(i->tag()) << " = " << std::string(i->value().begin(), i->value().end()) << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid message." << std::endl;
        }
    }

    return 0;
}
\endcode


\subsection example2 TCP Example

Here is an example that shows how to read incomplete messages, 
such as you might expect to encounter when reading fragmented packets out of a TCP socket. 
This example also shows how to handle invalid, corrupt messages.

\code
    char buffer[512];
    size_t buffer_length(0);
    
    while(true)
    {
        // Read (512 - buffer_length) bytes from I/O into buffer[buffer_length].
        // buffer_length += the number of bytes actually read.
        buffer_length += read(TCP_SOCKET_DESCRIPTOR, buffer + buffer_length, 512 - buffer_length);

        hffix::message_reader reader(buffer, buffer + buffer_length);

        // Try to read as many complete messages as there are in the buffer.
        for (; reader.is_complete(); ++reader)
        {
            if (reader.is_valid())
            {
                // Here is a complete message. Read fields out of the reader. 
                // Perhaps call a callback function and pass it a const reference to the reader as an argument.
            }
            else
            {
                // An invalid, corrupted FIX message. Do not try to read fields out of this reader.
                // The beginning of the invalid message is at location reader.message_begin() in the buffer,
                // but the end of the invalid message is unknown (because it's invalid).
                // Stay in this for loop, because the messager_reader::operator++()
                // will see that this message is invalid and it will search the remainder of the buffer
                // for the text "8=FIX.4.2\x01", to see if there might be a complete or partial valid message
                // anywhere else in the remainder of the buffer.
            }
        }
        buffer_length = reader.buffer_end() - reader.buffer_begin();

        if (buffer_length > 0) // Then there is an incomplete message at the end of the buffer.
            std::memmove(buffer, reader.buffer_begin(), buffer_length); // Move the partial portion of the incomplete message to buffer[0].
    }
\endcode

*/


/*! 
\def HFFIX_NO_BOOST_DATETIME

\brief If defined, High Frequency FIX Parser will not include Boost Date_Time support.

If the Boost Date_Time library is available in your build environment, boost::posix_time::ptime,
boost::posix_time::time_duration, and boost::gregorian::date
conversions will be supported for the various FIX date and time field types.
*/

#include <cstring> /* for memcpy */
#include <string>  /* for tag_name_dictionary */
#include <map> /* for tag_name_dictionary */
#include <sstream> /* for tag_name_dictionary */
#include <algorithm> /* for is_tag_a_data_length */
#include <iostream> /* for message_reader::value_type::operator<<() */
#include <limits> /* for numeric_limits<>::is_signed */
#include <stdexcept> /* for exceptions */

#ifndef HFFIX_NO_BOOST_DATETIME
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#endif // HFFIX_NO_BOOST_DATETIME

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
    template<typename Int_type> Int_type atoi(const char* begin, const char* end)
    {
        Int_type val(0);
        bool isnegative(false);

        if (begin < end && *begin == '-')
        {
            isnegative = true;
            ++begin;
        }

        for(;begin<end;++begin)
        {
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
    template<typename Uint_type> Uint_type atou(const char* begin, const char* end)
    {    
        Uint_type val(0);

        for(;begin<end;++begin)
        {
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
        if (number < 0)
        {
            isnegative = true;
            number = -number;
        }

        char*b = buffer;
        do
        {
            *b++ = '0' + (number % 10);
            number /= 10;
        }
        while(number);

        if (isnegative)
        {
            *b++ = '-';
        }

        char* e = b - 1;

        while(e > buffer) // Do the reversal in-place instead of making temp buffer. Better cache locality, and we don't have to guess how big to make the temp buffer.
        {
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
        do
        {
            *b++ = '0' + (number % 10);
            number /= 10;
        }
        while(number);

        char* e = b - 1;

        while(e > buffer) // Do the reversal in-place instead of making temp buffer. Better cache locality, and we don't have to guess how big to make the temp buffer.
        {
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
    template<typename Int_type> void atod(const char* begin, const char* end, Int_type& mantissa, Int_type& exponent)
    {
        mantissa = 0;
        exponent = 0;
        bool isdecimal(false);
        bool isnegative(false);

        if (begin < end && *begin == '-')
        {
            isnegative = true;
            ++begin;
        }

        for(;begin<end;begin++)
        {
            if (*begin == '.')
            {
                isdecimal = true;
            }
            else
            {
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
        if (mantissa < 0)
        {
            isnegative = true;
            mantissa = -mantissa;
        }

        char*b = buffer;
        do
        {
            *b++ = '0' + (mantissa % 10);
            mantissa /= 10;
            if (++exponent == 0)
            {
                *b++ = '.';
            }
        }
        while(mantissa > 0 || exponent < 1);

        if (isnegative)
        {
            *b++ = '-';
        }

        char* e = b - 1;

        while(e > buffer) // Do the reversal in-place instead of making temp buffer. Better cache locality, and we don't have to guess how big to make the temp buffer.
        {
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
        const char* begin,
        const char* end,
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
        const char* begin,
        const char* end,
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
\brief FIX 4.2 field value, weakly-typed as an array of chars, with type conversion methods.
*/
    class message_reader_value_type_value
    {
    public:

/*! \brief Pointer to the beginning of the field value. */
        const char* begin() const { return begin_; }

/*! \brief Pointer to past-the-end of the field value. */
        const char* end() const { return end_; }

/*! \brief Size of the field value, in bytes. */
        size_t size() const { return end_ - begin_; }


/*! 
\brief True if the value of the field is equal to the C-string argument.
*/
        friend bool operator==(const message_reader_value_type_value& that, const char* cstring)
        {
            return !strncmp(that.begin(), cstring, that.size()) && !cstring[that.size()];
        }


/*! 
\brief True if the value of the field is equal to the C-string argument.
*/
        friend bool operator==(const char* cstring, const message_reader_value_type_value& that)
        {
            return !strncmp(that.begin(), cstring, that.size()) && !cstring[that.size()];
        }

/*! 
\brief True if the value of the field is equal to the string argument.
*/
        template<typename Char_type> friend bool operator==(const message_reader_value_type_value& that, const std::basic_string<Char_type>& s)
        {
            return that.size() == s.size() && !strncmp(that.begin(), s.data(), that.size());
        }


/*! 
\brief True if the value of the field is equal to the string argument.
*/
        template<typename Char_type> friend bool operator==(const std::basic_string<Char_type>& s, const message_reader_value_type_value& that)
        {
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
        template<typename Char_type> std::basic_string<Char_type> as_string() const
        {
            return std::basic_string<Char_type>(begin(), end()); // the return value optimization
        }

/*!
\brief Ascii value as char.

\return The first char of the ascii value of the field.
*/
        char as_char() const
        {
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
        template<typename Int_type> void as_decimal(Int_type& mantissa, Int_type& exponent) const
        {
            details::atod<Int_type>(begin(), end(), mantissa, exponent);
        }
//@}

    private:
        template <typename Int_type, bool Is_signed_integer>
        struct as_int_selector
        {
        };

        template <typename Int_type>
        struct as_int_selector<Int_type, true>
        {
            static inline Int_type call_as_int(const char* begin, const char * end) { return details::atoi<Int_type>(begin, end); }
        };

        template <typename Int_type>
        struct as_int_selector<Int_type, false>
        {
            static inline Int_type call_as_int(const char* begin, const char * end) { return details::atou<Int_type>(begin, end); }
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
        template<typename Int_type> Int_type as_int() const
        {
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
            ) const
        {
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
            ) const
        {
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
            ) const
        {
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
            ) const
        {
            return 
                details::atotime(begin() + 9, end(), hour, minute, second, millisecond)
                && details::atodate(begin(), begin() + 8, year, month, day); // take advantage of short-circuit && to check field length
        }

//@}

#ifndef HFFIX_NO_BOOST_DATETIME

/*! \name Boost Date and Time Conversion Methods */
//@{

/*!
\brief Ascii-to-date conversion.

Parses ascii and returns a LocalMktDate or UTCDate.

Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

\return Date if parsing was successful, else boost::posix_time::not_a_date_time.
*/
        boost::gregorian::date as_date() const
        {
            int year, month, day;
            if (as_date(year, month, day))
            {
                try
                {
                    return boost::gregorian::date(year, month, day);
                }
                catch(std::exception& ex)
                {
                    return boost::gregorian::date(boost::posix_time::not_a_date_time);
                }
            }
            else
                return boost::gregorian::date(boost::posix_time::not_a_date_time);
        }

/*!
\brief Ascii-to-time conversion.

Parses ascii and returns a time.

Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

\return Time if parsing was successful, else boost::posix_time::not_a_date_time.
*/
        boost::posix_time::time_duration as_timeonly() const
        {
            int hour, minute, second, millisecond;
            if (as_timeonly(hour, minute, second, millisecond))
            {
                try
                {
                    return boost::posix_time::time_duration(hour, minute, second, boost::posix_time::time_duration::ticks_per_second() * millisecond / 1000);
                }
                catch(std::exception& ex)
                {
                    return boost::posix_time::time_duration(boost::posix_time::not_a_date_time);
                }
            }
            else
                return boost::posix_time::time_duration(boost::posix_time::not_a_date_time);
        }

/*!
\brief Ascii-to-timestamp conversion.

Parses ascii and returns a timestamp.

Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

\return Date and Time if parsing was successful, else boost::posix_time::not_a_date_time.
*/
        boost::posix_time::ptime as_timestamp() const
        {
            int year, month, day, hour, minute, second, millisecond;

            if (details::atotime(begin() + 9, end(), hour, minute, second, millisecond)
                && details::atodate(begin(), begin() + 8, year, month, day))
            {
                try
                {
                    return boost::posix_time::ptime(
                        boost::gregorian::date(year, month, day),
                        boost::posix_time::time_duration(hour, minute, second, boost::posix_time::time_duration::ticks_per_second() * millisecond / 1000)
                        );
                }
                catch(std::exception& ex)
                {
                    return boost::posix_time::not_a_date_time;
                }
            }
            else
                return boost::posix_time::not_a_date_time;
        }
//@}


#endif // HFFIX_NO_BOOST_DATETIME


    private:
        friend class message_reader_value_type;
        friend class message_reader_const_iterator;
        friend class message_reader;
        const char* begin_;
        const char* end_;
    };

/*!
\brief A FIX 4.2 field for reading, with tag and field value. This class is the hffix::message_reader::value_type for the hffix::message_reader Container.
*/
    class message_reader_value_type
    {
    public:

/*! \brief Tag of the field. */
        int tag() const { return tag_; } 

/*! \brief Weakly-typed value of the field. */
        const message_reader_value_type_value& value() const { return value_; }

/*! \brief Output stream operator. Output format is "[tag number]=[value]". */
        friend std::ostream& operator<<(std::ostream& os, const message_reader_value_type& that)
        {
            os << that.tag_ << "=";
            return os.write(that.value_.begin(), that.value_.size());
        }

    private:
        friend class message_reader_const_iterator;
        friend class message_reader;
        int tag_;
        message_reader_value_type_value value_;
    };

/*!
\brief hffix::message_reader::const_iterator. 

Satisfies the const Input Iterator Concept for an immutable hffix::message_reader container of fields.
*/
    class message_reader_const_iterator
    {
    public:

/*! 
\brief Copy Constructor. 
*/
        message_reader_const_iterator(const message_reader_const_iterator& that) :
            message_reader_(that.message_reader_),
            buffer_(that.buffer_),
            current_(that.current_)
        {
        }        
        
    private:
        // there cannot be a default constructor because of the message_reader_ member.

        message_reader_const_iterator(message_reader& container, const char* buffer) : 
            message_reader_(container),
            buffer_(buffer),
            current_()
          {
          }

    public:

        typedef ::std::input_iterator_tag iterator_category;
        typedef message_reader_value_type value_type;
        typedef ptrdiff_t difference_type;
        typedef message_reader_value_type* pointer;
        typedef message_reader_value_type& reference;


/*! 
\brief Returns a hffix::message_reader::const_reference to a field. 
*/
        const message_reader_value_type& operator*() const { return current_; }

/*! 
\brief Returns a hffix::message_reader::const_pointer to a field. 
*/
        const message_reader_value_type* operator->() const { return &current_; }



        friend bool operator==(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ == b.buffer_;
        }

        friend bool operator!=(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ != b.buffer_;
        }

        friend bool operator<(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ < b.buffer_;
        }

        friend bool operator>(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ > b.buffer_;
        }

        friend bool operator<=(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ <= b.buffer_;
        }

        friend bool operator>=(const message_reader_const_iterator& a, const message_reader_const_iterator& b)
        {
            return a.buffer_ >= b.buffer_;
        }

        message_reader_const_iterator operator++(int) //postfix
        {
            message_reader_const_iterator i(*this);
            ++(*this);
            return i;
        }        
        
        message_reader_const_iterator& operator++() //prefix
        {
            increment();
            return *this;
        }

    private:
        friend class message_reader;
        message_reader& message_reader_;
        const char* buffer_;
        message_reader_value_type current_;

        void increment();
    };



/*!
\brief One FIX 4.2 message for reading. 

An immutable Forward Container of FIX fields. Given a buffer containing a FIX 4.2 message, the hffix::message_reader
will provide an Iterator for iterating over the fields in the message without modifying the buffer. The buffer
used to construct the hffix::message_reader must outlive the hffix::message_reader.

The message_reader will only iterate over content fields of the message, and will skip over all of the meta-fields
for transport that are mixed in with the content fields in FIX 4.2. Here is the list of skipped fields:

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

Fields of binary data type are content fields, and will be iterated over like any other field. The special FIX 4.2 
binary data length fields will be skipped, but the length of the binary data is accessible from 
hffix::message_reader::value_type::value().size().
*/
    class message_reader
    {

    public:

        typedef message_reader_value_type value_type;
        typedef const message_reader_value_type& const_reference;
        typedef message_reader_const_iterator const_iterator;
        typedef const message_reader_value_type* const_pointer;
        typedef size_t size_type;

/*!
\brief Construct by buffer size.
\param buffer Pointer to the buffer to be read.
\param size Number of bytes in the buffer to be read.
*/
        message_reader(const char* buffer, size_t size) :
            buffer_(buffer),
            buffer_end_(buffer + size),
            begin_(*this, 0),
            end_(*this, 0),
            is_complete_(false),
            is_valid_(true)
        {
            init();
        }

/*!
\brief Construct by buffer begin and end.
\param begin Pointer to the buffer to be read.
\param end Pointer to past-the-end of the buffer to be read.
*/
        message_reader(const char* begin, const char* end) :
            buffer_(begin),
            buffer_end_(end),
            begin_(*this, 0),
            end_(*this, 0),
            is_complete_(false),
            is_valid_(true)
        {
            init();
        }
/*! 
\brief Copy constructor. The hffix::message_reader is immutable, so copying it is fine.
*/
        message_reader(const message_reader& that) :
            buffer_(that.buffer_),
            buffer_end_(that.buffer_end_),
            begin_(that.begin_),
            end_(that.end_),
            is_complete_(that.is_complete_),
            is_valid_(that.is_valid_)
        {
        }

        ~message_reader()
        {
        }

/*!
\brief An iterator to the MsgType field in the FIX 4.2 message. Same as hffix::message_reader::message_type().
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/
        const const_iterator& begin() const
        {    // return iterator for beginning of nonmutable sequence
            if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
            return begin_;
        }

/*!
\brief An iterator to the CheckSum field in the FIX 4.2 message. Same as hffix::message_reader::check_sum().
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/
        const const_iterator& end() const
        {    // return iterator for end of nonmutable sequence
            if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
            return end_;
        }

/*!
\brief An iterator to the MsgType field in the FIX 4.2 message. Same as hffix::message_reader::begin().
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/
        const const_iterator& message_type() const
        {    // return iterator for beginning of nonmutable sequence
            if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
            return begin_;
        }

/*!
\brief An iterator to the CheckSum field in the FIX 4.2 message. Same as hffix::message_reader::end().
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/        
        const const_iterator& check_sum() const
        {    // return iterator for end of nonmutable sequence
            if (!is_valid_) throw std::logic_error("hffix Cannot return iterator for an invalid message.");
            return end_;
        }

/*!
\brief A pointer to the begining of the buffer.
buffer_begin() == message_begin()
*/
        const char* buffer_begin() const
        {
            return buffer_;
        }
/*!
\brief A pointer to past-the-end of the buffer.
*/
        const char* buffer_end() const
        {
            return buffer_end_;
        }

/*!
\brief The size of the buffer in bytes.
*/
        size_t buffer_size() const
        {
            return buffer_end_ - buffer_;
        }

/*!
\brief A pointer to the beginning of the FIX 4.2 message in the buffer.
 buffer_begin() == message_begin()
*/
        const char* message_begin() const
        {
            return buffer_;
        }
/*!
\brief A pointer to past-the-end of the FIX 4.2 message in the buffer.
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/
        const char* message_end() const
        {
            if (!is_valid_) throw std::logic_error("hffix Cannot determine size of an invalid message.");
            return end_.current_.value_.end_ + 1;
        }

/*!
\brief The entire size of the FIX 4.1 message in bytes.
\throw std::logic_error if called on an invalid message. This exception is preventable by program logic. You should always check if a message is_valid() before reading.
*/
        size_t message_size() const
        {
            if (!is_valid_) throw std::logic_error("hffix Cannot determine size of an invalid message.");
            return end_.current_.value_.end_ - buffer_ + 1;
        }

/*!
\brief True if the buffer contains a complete FIX 4.2 message.
*/
        bool is_complete() const
        {
            return is_complete_;
        }
/*!
\brief True if the message is correct FIX 4.2.

If false, there was an error parsing the FIX 4.2 message and the message cannot be read.

fix-42_with_errata_20010501.pdf p.17:
"Valid FIX Message is a message that is properly formed according to this specification and contains a
valid body length and checksum field"
*/
        bool is_valid() const
        {
            return is_valid_;
        }


/*!
\brief Moves this message_reader to the next FIX 4.2 message in the buffer.

If this message is_valid() and is_complete(), assume that the next message comes immediately after this one.

If this message !is_valid(), will search the remainder of the buffer
for the text "8=FIX.4.2\x01", to see if there might be a complete or partial valid message
anywhere else in the remainder of the buffer.

If this message !is_complete(), no-op.

\return *this
*/
        message_reader& operator++()
        {
            if (!is_complete_)
            {
                return *this; // can't increment on an incomplete message.
            }

            if (!is_valid_) // this message isn't valid, so we have to try to search for the beginning of the next message.
            {
                const char* b = buffer_ + 1;
                while(b < buffer_end_ - 10)
                {
                    if (!std::memcmp(b, "8=FIX.4.2\x01", 10))
                        break;
                    ++b;
                }
                new (this) message_reader(b, buffer_end_); // whether or not we found the next message, reconstruct.
                return *this;
            }

            new (this) message_reader(end_.current_.value_.end_ + 1, buffer_end_);
            return *this;
        }

    private:
        friend class message_reader_const_iterator;
    
        void init()
        {
            if (buffer_end_ - buffer_ < 21) // 8=FIX.4.2 9=1 10=000 
            {
                is_complete_ = false; 
                return;
            }

            const char* b = buffer_ + 12;

            if (*b == '\x01')
            {
                malformed();
                return;
            }

            size_t bodylength(0);

            while(*b != '\x01')
            {
                if (*b < '0' || *b > '9') // this is the only time we need to check for numeric ascii.
                {
                    malformed();
                    return;
                }
                bodylength *= 10;
                bodylength += *b++ - '0';
                if (b > buffer_end_)
                {
                    is_complete_ = false;
                    return;
                }
            }
            ++b;

            if (bodylength > 1 << 12) // max body size allowed is 4KB, otherwise we assume message corruption.
            {
                malformed();
                return;
            }

            if (*b != '3' || b[1] != '5')
            {
                malformed();
                return;
            }

            const char* checksum = b + bodylength; // be careful that b + bodylength is legit before we try to dereference it.

            if (checksum + 7 > buffer_end_)
            {
                is_complete_ = false;
                return;
            }

            if (*(checksum + 6) != '\x01') // check for trailing SOH
            {
                malformed();
                return;
            }            
            
            begin_.buffer_ = b;
            begin_.current_.tag_ = 35; // MsgType
            b += 3;
            begin_.current_.value_.begin_ = b;
            while(*++b != '\x01') {}
            begin_.current_.value_.end_ = b;

            end_.buffer_ = checksum;
            end_.current_.tag_ = 10; //CheckSum
            end_.current_.value_.begin_ = checksum + 3;
            end_.current_.value_.end_ = checksum + 6;
            
            is_complete_ = true;
        }

        const char* buffer_;
        const char* buffer_end_;
        const_iterator begin_;
        const_iterator end_;
        value_type message_type_;
        bool is_complete_;
        bool is_valid_;

        void malformed()
        {
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

        while(*current_.value_.begin_ != '=')
        {
            current_.tag_ *= 10;
            current_.tag_ += (*current_.value_.begin_ - '0');
            ++current_.value_.begin_;
        }

        current_.value_.end_ = ++current_.value_.begin_;

        while(*(++current_.value_.end_ ) != '\x01') {}

        if (details::is_tag_a_data_length(current_.tag_))
        {
            size_t data_len = details::atou<size_t>(current_.value_.begin_, current_.value_.end_);

            buffer_ = current_.value_.end_ + 1;
            current_.value_.begin_ = buffer_;
            current_.tag_ = 0;

            while(*current_.value_.begin_ != '=')
            {
                current_.tag_ *= 10;
                current_.tag_ += (*current_.value_.begin_ - '0');
                ++current_.value_.begin_;
            }

            current_.value_.end_ = ++current_.value_.begin_ + data_len;

            if (current_.value_.end_ >= message_reader_.end_.buffer_)
            {
                message_reader_.malformed(); // in theory this can never happen.
            }
        }
    };


/*! 
\brief One FIX 4.2 message for writing.

Given a buffer, the message_writer will write a FIX 4.2 message to the buffer. The interface is patterned after
Back Insertion Sequence Containers, with overloads of push_back for different FIX 4.2 field data types.

The push_back_header() method will write the BeginString and BodyLength fields to the message, 
but the FIX 4.2 Standard Message Header requires also MsgType, SenderCompID, TargetCompID, MsgSeqNum and SendingTime. 
You must write those fields yourself, starting with MsgType.

After calling all other push_back methods and before sending the message, you must call push_back_trailer(), 
which will write the CheckSum field for you.

*/
    class message_writer
    {
    public:

/*!
\brief Construct by buffer size.
\param buffer Pointer to the buffer to be written to.
\param size Size of the buffer in bytes.
*/        message_writer(char* buffer, size_t size):
            buffer_(buffer),
            buffer_end_(buffer + size),
            next_(buffer)
        {
        }

/*!
\brief Construct by buffer begin and end.
\param begin Pointer to the buffer to be written to.
\param end Pointer to past-the-end of the buffer to be written to.
*/        
        message_writer(char* begin, char* end) :
            buffer_(begin),
            buffer_end_(end),
            next_(begin)
        {
        }

/*!
\brief Write the BeginString and BodyLength fields to the buffer.

This method must be called before any other push_back() method. It may only be called once for each message_writer.

\pre No other push_back method has yet been called.
*/
        void push_back_header()
        {
            memcpy(next_, "8=FIX.4.2\x01" "9=", 12);
            next_ += 16; // 12 + 4 characters reserved for BodyLength. &BodyLength = buffer_ + 12.
            *next_++ = '\x01';
        }
/*!
\brief Write the CheckSum field to the buffer.

This function must be called after all other push_back functions. It may only be called once for each message_writer.

\post There is a complete and valid FIX 4.2 message in the buffer.

\param calculate_checksum If this flag is set to false, then instead of iterating over the entire message and 
calculating the CheckSum, the standard trailer will simply write CheckSum=000. This is fine if you're sending
the message to a FIX 4.2 parser that, like High Frequency FIX Parser, doesn't care about the CheckSum.
*/
        void push_back_trailer(bool calculate_checksum = true)
        {
            // calculate and write out the BodyLength
            int bodylength = next_ - (buffer_ + 17);
            for(char* b = buffer_ + 15; b > buffer_ + 11; --b)
            {
                *b = '0' + (bodylength % 10);
                bodylength /= 10;
            }

            // write out the checksum after optionally calculating it
            if (calculate_checksum)
            {
                int checksum(0);
                char* b = buffer_;
                while(b < next_) checksum += *b++;
                checksum = checksum % 256;

                memcpy(next_, "10=", 3);
                next_ += 3;

                for (char* b = next_ + 2; b >= next_; --b)
                {
                    *b = '0' + (checksum % 10);
                    checksum /= 10;
                }
                next_ += 3;
                *next_++ = '\x01';
            }
            else
            {
                memcpy(next_, "10=000\x01", 7);
                next_ += 7;
            }

        }

/*!
\brief Size of the message in bytes.
*/
        size_t message_size() const
        {
            return next_ - buffer_;
        }

/*!
\brief Pointer to beginning of the message.
*/
        char* message_begin() const
        {
            return buffer_;
        }
/*!
\brief Pointer to past-the-end of the message.
*/
        char* message_end() const
        {
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
        void push_back_string(int tag, const char* begin, const char* end)
        {
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
        void push_back_string(int tag, const char* cstring)
        {
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
        template<typename Char_type> void push_back_string(int tag, const std::basic_string<Char_type>& s)
        {
            push_back_string(tag, s.data(), s.data() + s.length());
        }


/*!
\brief Append a char field to the message.
\param tag FIX tag.
\param character An ascii character.
*/
        void push_back_char(int tag, char character)
        {
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
        template<typename Int_type> void push_back_int(int tag, Int_type number)
        {
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
        template<typename Int_type> void push_back_decimal(int tag, Int_type mantissa, Int_type exponent)
        {
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
        void push_back_date(int tag, int year, int month, int day)
        {
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
        void push_back_monthyear(int tag, int year, int month)
        {
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
        void push_back_timeonly(int tag, int hour, int minute, int second)
        {
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
        void push_back_timeonly(int tag, int hour, int minute, int second, int millisecond)
        {
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
        void push_back_timestamp(int tag, int year, int month, int day, int hour, int minute, int second)
        {
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
        void push_back_timestamp(int tag, int year, int month, int day, int hour, int minute, int second, int millisecond)
        {
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

#ifndef HFFIX_NO_BOOST_DATETIME

/*! \name Boost Date and Time Methods */
//@{

/*! 
\brief Append a LocalMktDate or UTCDate field to the message.

Requires Boost Date_Time library support, can be disabled by #define HFFIX_NO_BOOST_DATETIME.

\param tag FIX tag.
\param date Date.
\see HFFIX_NO_BOOST_DATETIME
*/
        void push_back_date(int tag, boost::gregorian::date date)
        {
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
        void push_back_timeonly(int tag, boost::posix_time::time_duration timeonly)
        {
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
        void push_back_timestamp(int tag, boost::posix_time::ptime timestamp)
        {
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
#endif // HFFIX_NO_BOOST_DATETIME


/*! \name Data Methods */
//@{
        
/*!
\brief Append a data length field and a data field to the message.

Note that this method will append two fields to the message. The first field is an integer equal to 
the content length of the second field. FIX 4.2 does this so that the content of the second field may contain 
Ascii NULL or SOH or other strange characters. 

High Frequency FIX Parser calculates the content length for you
and writes out both fields, you just have to provide both tags and pointers to the data.
For most of the data fields in FIX 4.2, it is true that tag_data = tag_data_length + 1, but we daren't assume that.

\param tag_data_length FIX tag for the data length field.
\param tag_data FIX tag for the data field.
\param begin Pointer to the beginning of the data.
\param end Pointer to after-the-end of the data.
*/
        void push_back_data(int tag_data_length, int tag_data, const char* begin, const char* end)
        {
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
        static void itoa_padded(int x, char* b, char* e)
        {
            while (e > b)
            {
                *--e = '0' + (x % 10);
                x /= 10;
            }
        }

        message_writer() {} // no default construction allowed
        message_writer(const message_writer& that) {} // no copying allowed
        message_writer& operator=(const message_writer& that) { return *this; } // no assignment allowed

        char* buffer_;
        char* buffer_end_;
        char* next_;
    };

/*!
\brief Namespace for all of the integer codes for the FIX 4.2 tags. Convenient for use in C++ code.
*/
    namespace tag
    {
        const int Account = 1;
        const int AdvId = 2;
        const int AdvRefID = 3;
        const int AdvSide = 4;
        const int AdvTransType = 5;
        const int AvgPx = 6;
        const int BeginSeqNo = 7;
        const int BeginString = 8;
        const int BodyLength = 9;
        const int CheckSum = 10;
        const int ClOrdID = 11;
        const int Commission = 12;
        const int CommType = 13;
        const int CumQty = 14;
        const int Currency = 15;
        const int EndSeqNo = 16;
        const int ExecID = 17;
        const int ExecInst = 18;
        const int ExecRefID = 19;
        const int ExecTransType = 20;
        const int HandlInst = 21;
        const int IDSource = 22;
        const int IOIid = 23;
        const int IOIOthSvc = 24;
        const int IOIQltyInd = 25;
        const int IOIRefID = 26;
        const int IOIShares = 27;
        const int IOITransType = 28;
        const int LastCapacity = 29;
        const int LastMkt = 30;
        const int LastPx = 31;
        const int LastShares = 32;
        const int LinesOfText = 33;
        const int MsgSeqNum = 34;
        const int MsgType = 35;
        const int NewSeqNo = 36;
        const int OrderID = 37;
        const int OrderQty = 38;
        const int OrdStatus = 39;
        const int OrdType = 40;
        const int OrigClOrdID = 41;
        const int OrigTime = 42;
        const int PossDupFlag = 43;
        const int Price = 44;
        const int RefSeqNum = 45;
        const int RelatdSym = 46;
        const int Rule80A = 47;
        const int SecurityID = 48;
        const int SenderCompID = 49;
        const int SenderSubID = 50;
        const int SendingDate = 51;
        const int SendingTime = 52;
        const int Shares = 53;
        const int Side = 54;
        const int Symbol = 55;
        const int TargetCompID = 56;
        const int TargetSubID = 57;
        const int Text = 58;
        const int TimeInForce = 59;
        const int TransactTime = 60;
        const int Urgency = 61;
        const int ValidUntilTime = 62;
        const int SettlmntTyp = 63;
        const int FutSettDate = 64;
        const int SymbolSfx = 65;
        const int ListID = 66;
        const int ListSeqNo = 67;
        const int TotNoOrders = 68;
        const int ListExecInst = 69;
        const int AllocID = 70;
        const int AllocTransType = 71;
        const int RefAllocID = 72;
        const int NoOrders = 73;
        const int AvgPrxPrecision = 74;
        const int TradeDate = 75;
        const int ExecBroker = 76;
        const int OpenClose = 77;
        const int NoAllocs = 78;
        const int AllocAccount = 79;
        const int AllocShares = 80;
        const int ProcessCode = 81;
        const int NoRpts = 82;
        const int RptSeq = 83;
        const int CxlQty = 84;
        const int NoDlvyInst = 85;
        const int DlvyInst = 86;
        const int AllocStatus = 87;
        const int AllocRejCode = 88;
        const int Signature = 89;
        const int SecureDataLen = 90;
        const int SecureData = 91;
        const int BrokerOfCredit = 92;
        const int SignatureLength = 93;
        const int EmailType = 94;
        const int RawDataLength = 95; //sic
        const int RawData = 96;
        const int PossResend = 97;
        const int EncryptMethod = 98;
        const int StopPx = 99;
        const int ExDestination = 100;
        const int CxlRejReason = 102;
        const int OrdRejReason = 103;
        const int IOIQualifier = 104;
        const int WaveNo = 105;
        const int Issuer = 106;
        const int SecurityDesc = 107;
        const int HeartBtInt = 108;
        const int ClientID = 109;
        const int MinQty = 110;
        const int MaxFloor = 111;
        const int TestReqID = 112;
        const int ReportToExch = 113;
        const int LocateReqd = 114;
        const int OnBehalfOfCompID = 115;
        const int OnBehalfOfSubID = 116;
        const int QuoteID = 117;
        const int NetMoney = 118;
        const int SettlCurrAmt = 119;
        const int SettlCurrency = 120;
        const int ForexReq = 121;
        const int OrigSendingTime = 122;
        const int GapFillFlag = 123;
        const int NoExecs = 124;
        const int CxlType = 125;
        const int ExpireTime = 126;
        const int DKReason = 127;
        const int DeliverToCompID = 128;
        const int DeliverToSubID = 129;
        const int IOINaturalFlag = 130;
        const int QuoteReqID = 131;
        const int BidPx = 132;
        const int OfferPx = 133;
        const int BidSize = 134;
        const int OfferSize = 135;
        const int NoMiscFees = 136;
        const int MiscFeeAmt = 137;
        const int MiscFeeCurr = 138;
        const int MiscFeeType = 139;
        const int PrevClosePx = 140;
        const int ResetSeqNumFlag = 141;
        const int SenderLocationID = 142;
        const int TargetLocationID = 143;
        const int OnBehalfOfLocationID = 144;
        const int DeliverToLocationID = 145;
        const int NoRelatedSym = 146;
        const int Subject = 147;
        const int Headline = 148;
        const int URLLink = 149;
        const int ExecType = 150;
        const int LeavesQty = 151;
        const int CashOrderQty = 152;
        const int AllocAvgPx = 153;
        const int AllocNetMoney = 154;
        const int SettlCurrFxRate = 155;
        const int SettlCurrFxRateCalc = 156;
        const int NumDaysInterest = 157;
        const int AccruedInterestRate = 158;
        const int AccruedInterestAmt = 159;
        const int SettlInstMode = 160;
        const int AllocText = 161;
        const int SettlInstID = 162;
        const int SettlInstTransType = 163;
        const int EmailThreadID = 164;
        const int SettlInstSource = 165;
        const int SettlLocation = 166;
        const int SecurityType = 167;
        const int EffectiveTime = 168;
        const int StandInstDbType = 169;
        const int StandInstDbName = 170;
        const int StandInstDbID = 171;
        const int SettlDeliveryType = 172;
        const int SettlDepositoryCode = 173;
        const int SettlBrkrCode = 174;
        const int SettlInstCode = 175;
        const int SecuritySettlAgentName = 176;
        const int SecuritySettlAgentCode = 177;
        const int SecuritySettlAgentAcctNum = 178;
        const int SecuritySettlAgentAcctName = 179;
        const int SecuritySettlAgentContactName = 180;
        const int SecuritySettlAgentContactPhone = 181;
        const int CashSettlAgentName = 182;
        const int CashSettlAgentCode = 183;
        const int CashSettlAgentAcctNum = 184;
        const int CashSettlAgentAcctName = 185;
        const int CashSettlAgentContactName = 186;
        const int CashSettlAgentContactPhone = 187;
        const int BidSpotRate = 188;
        const int BidForwardPoints = 189;
        const int OfferSpotRate = 190;
        const int OfferForwardPoints = 191;
        const int OrderQty2 = 192;
        const int FutSettDate2 = 193;
        const int LastSpotRate = 194;
        const int LastForwardPoints = 195;
        const int AllocLinkID = 196;
        const int AllocLinkType = 197;
        const int SecondaryOrderID = 198;
        const int NoIOIQualifiers = 199;
        const int MaturityMonthYear = 200;
        const int PutOrCall = 201;
        const int StrikePrice = 202;
        const int CoveredOrUncovered = 203;
        const int CustomerOrFirm = 204;
        const int MaturityDay = 205;
        const int OptAttribute = 206;
        const int SecurityExchange = 207;
        const int NotifyBrokerOfCredit = 208;
        const int AllocHandlInst = 209;
        const int MaxShow = 210;
        const int PegDifference = 211;
        const int XmlDataLen = 212;
        const int XmlData = 213;
        const int SettlInstRefID = 214;
        const int NoRoutingIDs = 215;
        const int RoutingType = 216;
        const int RoutingID = 217;
        const int SpreadToBenchmark = 218;
        const int Benchmark = 219;
        const int CouponRate = 223;
        const int ContractMultiplier = 231;
        const int MDReqID = 262;
        const int SubscriptionRequestType = 263;
        const int MarketDepth = 264;
        const int MDUpdateType = 265;
        const int AggregatedBook = 266;
        const int NoMDEntryTypes = 267;
        const int NoMDEntries = 268;
        const int MDEntryType = 269;
        const int MDEntryPx = 270;
        const int MDEntrySize = 271;
        const int MDEntryDate = 272;
        const int MDEntryTime = 273;
        const int TickDirection = 274;
        const int MDMkt = 275;
        const int QuoteCondition = 276;
        const int TradeCondition = 277;
        const int MDEntryID = 278;
        const int MDUpdateAction = 279;
        const int MDEntryRefID = 280;
        const int MDReqRejReason = 281;
        const int MDEntryOriginator = 282;
        const int LocationID = 283;
        const int DeskID = 284;
        const int DeleteReason = 285;
        const int OpenCloseSettleFlag = 286;
        const int SellerDays = 287;
        const int MDEntryBuyer = 288;
        const int MDEntrySeller = 289;
        const int MDEntryPositionNo = 290;
        const int FinancialStatus = 291;
        const int CorporateAction = 292;
        const int DefBidSize = 293;
        const int DefOfferSize = 294;
        const int NoQuoteEntries = 295;
        const int NoQuoteSets = 296;
        const int QuoteAckStatus = 297;
        const int QuoteCancelType = 298;
        const int QuoteEntryID = 299;
        const int QuoteRejectReason = 300;
        const int QuoteResponseLevel = 301;
        const int QuoteSetID = 302;
        const int QuoteRequestType = 303;
        const int TotQuoteEntries = 304;
        const int UnderlyingIDSource = 305;
        const int UnderlyingIssuer = 306;
        const int UnderlyingSecurityDesc = 307;
        const int UnderlyingSecurityExchange = 308;
        const int UnderlyingSecurityID = 309;
        const int UnderlyingSecurityType = 310;
        const int UnderlyingSymbol = 311;
        const int UnderlyingSymbolSfx = 312;
        const int UnderlyingMaturityMonthYear = 313;
        const int UnderlyingMaturityDay = 314;
        const int UnderlyingPutOrCall = 315;
        const int UnderlyingStrikePrice = 316;
        const int UnderlyingOptAttribute = 317;
        const int UnderlyingCurrency = 318;
        const int RatioQty = 319;
        const int SecurityReqID = 320;
        const int SecurityRequestType = 321;
        const int SecurityResponseID = 322;
        const int SecurityResponseType = 323;
        const int SecurityStatusReqID = 324;
        const int UnsolicitedIndicator = 325;
        const int SecurityTradingStatus = 326;
        const int HaltReason = 327;
        const int InViewOfCommon = 328;
        const int DueToRelated = 329;
        const int BuyVolume = 330;
        const int SellVolume = 331;
        const int HighPx = 332;
        const int LowPx = 333;
        const int Adjustment = 334;
        const int TradSesReqID = 335;
        const int TradingSessionID = 336;
        const int ContraTrader = 337;
        const int TradSesMethod = 338;
        const int TradSesMode = 339;
        const int TradSesStatus = 340;
        const int TradSesStartTime = 341;
        const int TradSesOpenTime = 342;
        const int TradSesPreCloseTime = 343;
        const int TradSesCloseTime = 344;
        const int TradSesEndTime = 345;
        const int NumberOfOrders = 346;
        const int MessageEncoding = 347;
        const int EncodedIssuerLen = 348;
        const int EncodedIssuer = 349;
        const int EncodedSecurityDescLen = 350;
        const int EncodedSecurityDesc = 351;
        const int EncodedListExecInstLen = 352;
        const int EncodedListExecInst = 353;
        const int EncodedTextLen = 354;
        const int EncodedText = 355;
        const int EncodedSubjectLen = 356;
        const int EncodedSubject = 357;
        const int EncodedHeadlineLen = 358;
        const int EncodedHeadline = 359;
        const int EncodedAllocTextLen = 360;
        const int EncodedAllocText = 361;
        const int EncodedUnderlyingIssuerLen = 362;
        const int EncodedUnderlyingIssuer = 363;
        const int EncodedUnderlyingSecurityDescLen = 364;
        const int EncodedUnderlyingSecurityDesc = 365;
        const int AllocPrice = 366;
        const int QuoteSetValidUntilTime = 367;
        const int QuoteEntryRejectReason = 368;
        const int LastMsgSeqNumProcessed = 369;
        const int OnBehalfOfSendingTime = 370;
        const int RefTagID = 371;
        const int RefMsgType = 372;
        const int SessionRejectReason = 373;
        const int BidRequestTransType = 374;
        const int ContraBroker = 375;
        const int ComplianceID = 376;
        const int SolicitedFlag = 377;
        const int ExecRestatementReason = 378;
        const int BusinessRejectRefID = 379;
        const int BusinessRejectReason = 380;
        const int GrossTradeAmt = 381;
        const int NoContraBrokers = 382;
        const int MaxMessageSize = 383;
        const int NoMsgTypes = 384;
        const int MsgDirection = 385;
        const int NoTradingSessions = 386;
        const int TotalVolumeTraded = 387;
        const int DiscretionInst = 388;
        const int DiscretionOffset = 389;
        const int BidID = 390;
        const int ClientBidID = 391;
        const int ListName = 392;
        const int TotalNumSecurities = 393;
        const int BidType = 394;
        const int NumTickets = 395;
        const int SideValue1 = 396;
        const int SideValue2 = 397;
        const int NoBidDescriptors = 398;
        const int BidDescriptorType = 399;
        const int BidDescriptor = 400;
        const int SideValueInd = 401;
        const int LiquidityPctLow = 402;
        const int LiquidityPctHigh = 403;
        const int LiquidityValue = 404;
        const int EFPTrackingError = 405;
        const int FairValue = 406;
        const int OutsideIndexPct = 407;
        const int ValueOfFutures = 408;
        const int LiquidityIndType = 409;
        const int WtAverageLiquidity = 410;
        const int ExchangeForPhysical = 411;
        const int OutMainCntryUIndex = 412;
        const int CrossPercent = 413;
        const int ProgRptReqs = 414;
        const int ProgPeriodInterval = 415;
        const int IncTaxInd = 416;
        const int NumBidders = 417;
        const int TradeType = 418;
        const int BasisPxType = 419;
        const int NoBidComponents = 420;
        const int Country = 421;
        const int TotNoStrikes = 422;
        const int PriceType = 423;
        const int DayOrderQty = 424;
        const int DayCumQty = 425;
        const int DayAvgPx = 426;
        const int GTBookingInst = 427;
        const int NoStrikes = 428;
        const int ListStatusType = 429;
        const int NetGrossInd = 430;
        const int ListOrderStatus = 431;
        const int ExpireDate = 432;
        const int ListExecInstType = 433;
        const int CxlRejResponseTo = 434;
        const int UnderlyingCouponRate = 435;
        const int UnderlyingContractMultiplier = 436;
        const int ContraTradeQty = 437;
        const int ContraTradeTime = 438;
        const int ClearingFirm = 439;
        const int ClearingAccount = 440;
        const int LiquidityNumSecurities = 441;
        const int MultiLegReportingType = 442;
        const int StrikeTime = 443;
        const int ListStatusText = 444;
        const int EncodedListStatusTextLen = 445;
        const int EncodedListStatusText = 446;
    } // namespace tag

/* @cond EXCLUDE */

namespace details {
    inline bool is_tag_a_data_length(int tag)
    {
        const static int tag_data_length_[13] = {
            tag::SecureDataLen,
            tag::RawDataLength, 
            tag::XmlDataLen,
            tag::EncodedIssuerLen,
            tag::EncodedSecurityDescLen,
            tag::EncodedListExecInstLen,
            tag::EncodedTextLen,
            tag::EncodedSubjectLen,
            tag::EncodedHeadlineLen,
            tag::EncodedAllocTextLen,
            tag::EncodedUnderlyingIssuerLen,
            tag::EncodedUnderlyingSecurityDescLen,
            tag::EncodedListStatusTextLen
        };

        return std::binary_search(tag_data_length_, tag_data_length_ + 13, tag); // is this faster than a linear search over 13 elements?
    }
}
/* @endcond */


/*!
\brief Dictionary to look up FIX 4.2 field names by tag at run-time. Convenient for debugging.

Constructing an instance of the tag_name_dictionary is expensive. If you want a global tag_name_dictionary 
that you can use from all over your program, consider using Scott Meyers' Singleton pattern:

\code
#include "hffix.hpp"

hffix::tag_name_dictionary& dictionary() // Meyers' Singleton
{
    static hffix::tag_name_dictionary instance;
    return instance;
}

int main(int argc, const char **argv)
{
    std::cout << dictionary()(hffix::tag::MsgType) << std::endl; // Will output "MsgType".
    return 0;
}
\endcode
*/
    class tag_name_dictionary
    {
    public:
        tag_name_dictionary()
        {
            init();
        }

/*!
\brief Look up a tag name, return tag number string if not found.

Not safe for concurrent calls.

\return Reference to a tag name. If the tag name is not in the dictionary, will return the string representation of the tag integer.
\throw std::bad_alloc
*/
        const std::string& operator()(int tag)
        {
            std::map<int, std::string>::iterator i = dictionary_.find(tag);
            if (i != dictionary_.end())
            {
                return i->second;
            }
            else
            {
                std::stringstream ss;
                ss << tag;

                std::pair<std::map<int, std::string>::iterator, bool> result = dictionary_.insert(make_pair(tag, ss.str()));
                return result.first->second;
            }
        }
/*!
\brief Look up a tag name, return empty string if not found.

Safe for concurrent calls.

\return Reference to a tag name. If the tag name is not in the dictionary, will return a reference to an empty string.
*/
        const std::string& operator[](int tag) const
        {
            static std::string empty_string;

            std::map<int, std::string>::const_iterator i = dictionary_.find(tag);
            if (i != dictionary_.end())
            {
                return i->second;
            }
            else
            {
                return empty_string;
            }
        }

/*!
\brief Look up a tag name, return null if not found.

Safe for concurrent calls.

\return Pointer to a tag name, or null pointer if the tag is not in the dictionary.
*/
        const std::string* lookup(int tag) const
        {
            std::map<int, std::string>::const_iterator i = dictionary_.find(tag);
            if (i != dictionary_.end())
            {
                return &(i->second);
            }
            else
            {
                return 0;
            }
        }

/*!
\brief Insert a tag name in the dictionary.

Not safe for concurrent calls.

If the tag is already present, the tag name for that tag will be overwritten.
\throw std::bad_alloc
*/
        void insert(int tag, const std::string& tagname)
        {
            dictionary_[tag] = tagname;
        }

    private:
        tag_name_dictionary(const tag_name_dictionary& that) {}
        std::map<int, std::string> dictionary_;

        void init()
        {
            dictionary_[1] = "Account";
            dictionary_[2] = "AdvId";
            dictionary_[3] = "AdvRefID";
            dictionary_[4] = "AdvSide";
            dictionary_[5] = "AdvTransType";
            dictionary_[6] = "AvgPx";
            dictionary_[7] = "BeginSeqNo";
            dictionary_[8] = "BeginString";
            dictionary_[9] = "BodyLength";
            dictionary_[10] = "CheckSum";
            dictionary_[11] = "ClOrdID";
            dictionary_[12] = "Commission";
            dictionary_[13] = "CommType";
            dictionary_[14] = "CumQty";
            dictionary_[15] = "Currency";
            dictionary_[16] = "EndSeqNo";
            dictionary_[17] = "ExecID";
            dictionary_[18] = "ExecInst";
            dictionary_[19] = "ExecRefID";
            dictionary_[20] = "ExecTransType";
            dictionary_[21] = "HandlInst";
            dictionary_[22] = "IDSource";
            dictionary_[23] = "IOIid";
            dictionary_[24] = "IOIOthSvc";
            dictionary_[25] = "IOIQltyInd";
            dictionary_[26] = "IOIRefID";
            dictionary_[27] = "IOIShares";
            dictionary_[28] = "IOITransType";
            dictionary_[29] = "LastCapacity";
            dictionary_[30] = "LastMkt";
            dictionary_[31] = "LastPx";
            dictionary_[32] = "LastShares";
            dictionary_[33] = "LinesOfText";
            dictionary_[34] = "MsgSeqNum";
            dictionary_[35] = "MsgType";
            dictionary_[36] = "NewSeqNo";
            dictionary_[37] = "OrderID";
            dictionary_[38] = "OrderQty";
            dictionary_[39] = "OrdStatus";
            dictionary_[40] = "OrdType";
            dictionary_[41] = "OrigClOrdID";
            dictionary_[42] = "OrigTime";
            dictionary_[43] = "PossDupFlag";
            dictionary_[44] = "Price";
            dictionary_[45] = "RefSeqNum";
            dictionary_[46] = "RelatdSym";
            dictionary_[47] = "Rule80A";
            dictionary_[48] = "SecurityID";
            dictionary_[49] = "SenderCompID";
            dictionary_[50] = "SenderSubID";
            dictionary_[51] = "SendingDate";
            dictionary_[52] = "SendingTime";
            dictionary_[53] = "Shares";
            dictionary_[54] = "Side";
            dictionary_[55] = "Symbol";
            dictionary_[56] = "TargetCompID";
            dictionary_[57] = "TargetSubID";
            dictionary_[58] = "Text";
            dictionary_[59] = "TimeInForce";
            dictionary_[60] = "TransactTime";
            dictionary_[61] = "Urgency";
            dictionary_[62] = "ValidUntilTime";
            dictionary_[63] = "SettlmntTyp";
            dictionary_[64] = "FutSettDate";
            dictionary_[65] = "SymbolSfx";
            dictionary_[66] = "ListID";
            dictionary_[67] = "ListSeqNo";
            dictionary_[68] = "TotNoOrders";
            dictionary_[69] = "ListExecInst";
            dictionary_[70] = "AllocID";
            dictionary_[71] = "AllocTransType";
            dictionary_[72] = "RefAllocID";
            dictionary_[73] = "NoOrders";
            dictionary_[74] = "AvgPrxPrecision";
            dictionary_[75] = "TradeDate";
            dictionary_[76] = "ExecBroker";
            dictionary_[77] = "OpenClose";
            dictionary_[78] = "NoAllocs";
            dictionary_[79] = "AllocAccount";
            dictionary_[80] = "AllocShares";
            dictionary_[81] = "ProcessCode";
            dictionary_[82] = "NoRpts";
            dictionary_[83] = "RptSeq";
            dictionary_[84] = "CxlQty";
            dictionary_[85] = "NoDlvyInst";
            dictionary_[86] = "DlvyInst";
            dictionary_[87] = "AllocStatus";
            dictionary_[88] = "AllocRejCode";
            dictionary_[89] = "Signature";
            dictionary_[90] = "SecureDataLen";
            dictionary_[91] = "SecureData";
            dictionary_[92] = "BrokerOfCredit";
            dictionary_[93] = "SignatureLength";
            dictionary_[94] = "EmailType";
            dictionary_[95] = "RawDataLength";
            dictionary_[96] = "RawData";
            dictionary_[97] = "PossResend";
            dictionary_[98] = "EncryptMethod";
            dictionary_[99] = "StopPx";
            dictionary_[100] = "ExDestination";
            dictionary_[102] = "CxlRejReason";
            dictionary_[103] = "OrdRejReason";
            dictionary_[104] = "IOIQualifier";
            dictionary_[105] = "WaveNo";
            dictionary_[106] = "Issuer";
            dictionary_[107] = "SecurityDesc";
            dictionary_[108] = "HeartBtInt";
            dictionary_[109] = "ClientID";
            dictionary_[110] = "MinQty";
            dictionary_[111] = "MaxFloor";
            dictionary_[112] = "TestReqID";
            dictionary_[113] = "ReportToExch";
            dictionary_[114] = "LocateReqd";
            dictionary_[115] = "OnBehalfOfCompID";
            dictionary_[116] = "OnBehalfOfSubID";
            dictionary_[117] = "QuoteID";
            dictionary_[118] = "NetMoney";
            dictionary_[119] = "SettlCurrAmt";
            dictionary_[120] = "SettlCurrency";
            dictionary_[121] = "ForexReq";
            dictionary_[122] = "OrigSendingTime";
            dictionary_[123] = "GapFillFlag";
            dictionary_[124] = "NoExecs";
            dictionary_[125] = "CxlType";
            dictionary_[126] = "ExpireTime";
            dictionary_[127] = "DKReason";
            dictionary_[128] = "DeliverToCompID";
            dictionary_[129] = "DeliverToSubID";
            dictionary_[130] = "IOINaturalFlag";
            dictionary_[131] = "QuoteReqID";
            dictionary_[132] = "BidPx";
            dictionary_[133] = "OfferPx";
            dictionary_[134] = "BidSize";
            dictionary_[135] = "OfferSize";
            dictionary_[136] = "NoMiscFees";
            dictionary_[137] = "MiscFeeAmt";
            dictionary_[138] = "MiscFeeCurr";
            dictionary_[139] = "MiscFeeType";
            dictionary_[140] = "PrevClosePx";
            dictionary_[141] = "ResetSeqNumFlag";
            dictionary_[142] = "SenderLocationID";
            dictionary_[143] = "TargetLocationID";
            dictionary_[144] = "OnBehalfOfLocationID";
            dictionary_[145] = "DeliverToLocationID";
            dictionary_[146] = "NoRelatedSym";
            dictionary_[147] = "Subject";
            dictionary_[148] = "Headline";
            dictionary_[149] = "URLLink";
            dictionary_[150] = "ExecType";
            dictionary_[151] = "LeavesQty";
            dictionary_[152] = "CashOrderQty";
            dictionary_[153] = "AllocAvgPx";
            dictionary_[154] = "AllocNetMoney";
            dictionary_[155] = "SettlCurrFxRate";
            dictionary_[156] = "SettlCurrFxRateCalc";
            dictionary_[157] = "NumDaysInterest";
            dictionary_[158] = "AccruedInterestRate";
            dictionary_[159] = "AccruedInterestAmt";
            dictionary_[160] = "SettlInstMode";
            dictionary_[161] = "AllocText";
            dictionary_[162] = "SettlInstID";
            dictionary_[163] = "SettlInstTransType";
            dictionary_[164] = "EmailThreadID";
            dictionary_[165] = "SettlInstSource";
            dictionary_[166] = "SettlLocation";
            dictionary_[167] = "SecurityType";
            dictionary_[168] = "EffectiveTime";
            dictionary_[169] = "StandInstDbType";
            dictionary_[170] = "StandInstDbName";
            dictionary_[171] = "StandInstDbID";
            dictionary_[172] = "SettlDeliveryType";
            dictionary_[173] = "SettlDepositoryCode";
            dictionary_[174] = "SettlBrkrCode";
            dictionary_[175] = "SettlInstCode";
            dictionary_[176] = "SecuritySettlAgentName";
            dictionary_[177] = "SecuritySettlAgentCode";
            dictionary_[178] = "SecuritySettlAgentAcctNum";
            dictionary_[179] = "SecuritySettlAgentAcctName";
            dictionary_[180] = "SecuritySettlAgentContactName";
            dictionary_[181] = "SecuritySettlAgentContactPhone";
            dictionary_[182] = "CashSettlAgentName";
            dictionary_[183] = "CashSettlAgentCode";
            dictionary_[184] = "CashSettlAgentAcctNum";
            dictionary_[185] = "CashSettlAgentAcctName";
            dictionary_[186] = "CashSettlAgentContactName";
            dictionary_[187] = "CashSettlAgentContactPhone";
            dictionary_[188] = "BidSpotRate";
            dictionary_[189] = "BidForwardPoints";
            dictionary_[190] = "OfferSpotRate";
            dictionary_[191] = "OfferForwardPoints";
            dictionary_[192] = "OrderQty2";
            dictionary_[193] = "FutSettDate2";
            dictionary_[194] = "LastSpotRate";
            dictionary_[195] = "LastForwardPoints";
            dictionary_[196] = "AllocLinkID";
            dictionary_[197] = "AllocLinkType";
            dictionary_[198] = "SecondaryOrderID";
            dictionary_[199] = "NoIOIQualifiers";
            dictionary_[200] = "MaturityMonthYear";
            dictionary_[201] = "PutOrCall";
            dictionary_[202] = "StrikePrice";
            dictionary_[203] = "CoveredOrUncovered";
            dictionary_[204] = "CustomerOrFirm";
            dictionary_[205] = "MaturityDay";
            dictionary_[206] = "OptAttribute";
            dictionary_[207] = "SecurityExchange";
            dictionary_[208] = "NotifyBrokerOfCredit";
            dictionary_[209] = "AllocHandlInst";
            dictionary_[210] = "MaxShow";
            dictionary_[211] = "PegDifference";
            dictionary_[212] = "XmlDataLen";
            dictionary_[213] = "XmlData";
            dictionary_[214] = "SettlInstRefID";
            dictionary_[215] = "NoRoutingIDs";
            dictionary_[216] = "RoutingType";
            dictionary_[217] = "RoutingID";
            dictionary_[218] = "SpreadToBenchmark";
            dictionary_[219] = "Benchmark";
            dictionary_[223] = "CouponRate";
            dictionary_[231] = "ContractMultiplier";
            dictionary_[262] = "MDReqID";
            dictionary_[263] = "SubscriptionRequestType";
            dictionary_[264] = "MarketDepth";
            dictionary_[265] = "MDUpdateType";
            dictionary_[266] = "AggregatedBook";
            dictionary_[267] = "NoMDEntryTypes";
            dictionary_[268] = "NoMDEntries";
            dictionary_[269] = "MDEntryType";
            dictionary_[270] = "MDEntryPx";
            dictionary_[271] = "MDEntrySize";
            dictionary_[272] = "MDEntryDate";
            dictionary_[273] = "MDEntryTime";
            dictionary_[274] = "TickDirection";
            dictionary_[275] = "MDMkt";
            dictionary_[276] = "QuoteCondition";
            dictionary_[277] = "TradeCondition";
            dictionary_[278] = "MDEntryID";
            dictionary_[279] = "MDUpdateAction";
            dictionary_[280] = "MDEntryRefID";
            dictionary_[281] = "MDReqRejReason";
            dictionary_[282] = "MDEntryOriginator";
            dictionary_[283] = "LocationID";
            dictionary_[284] = "DeskID";
            dictionary_[285] = "DeleteReason";
            dictionary_[286] = "OpenCloseSettleFlag";
            dictionary_[287] = "SellerDays";
            dictionary_[288] = "MDEntryBuyer";
            dictionary_[289] = "MDEntrySeller";
            dictionary_[290] = "MDEntryPositionNo";
            dictionary_[291] = "FinancialStatus";
            dictionary_[292] = "CorporateAction";
            dictionary_[293] = "DefBidSize";
            dictionary_[294] = "DefOfferSize";
            dictionary_[295] = "NoQuoteEntries";
            dictionary_[296] = "NoQuoteSets";
            dictionary_[297] = "QuoteAckStatus";
            dictionary_[298] = "QuoteCancelType";
            dictionary_[299] = "QuoteEntryID";
            dictionary_[300] = "QuoteRejectReason";
            dictionary_[301] = "QuoteResponseLevel";
            dictionary_[302] = "QuoteSetID";
            dictionary_[303] = "QuoteRequestType";
            dictionary_[304] = "TotQuoteEntries";
            dictionary_[305] = "UnderlyingIDSource";
            dictionary_[306] = "UnderlyingIssuer";
            dictionary_[307] = "UnderlyingSecurityDesc";
            dictionary_[308] = "UnderlyingSecurityExchange";
            dictionary_[309] = "UnderlyingSecurityID";
            dictionary_[310] = "UnderlyingSecurityType";
            dictionary_[311] = "UnderlyingSymbol";
            dictionary_[312] = "UnderlyingSymbolSfx";
            dictionary_[313] = "UnderlyingMaturityMonthYear";
            dictionary_[314] = "UnderlyingMaturityDay";
            dictionary_[315] = "UnderlyingPutOrCall";
            dictionary_[316] = "UnderlyingStrikePrice";
            dictionary_[317] = "UnderlyingOptAttribute";
            dictionary_[318] = "UnderlyingCurrency";
            dictionary_[319] = "RatioQty";
            dictionary_[320] = "SecurityReqID";
            dictionary_[321] = "SecurityRequestType";
            dictionary_[322] = "SecurityResponseID";
            dictionary_[323] = "SecurityResponseType";
            dictionary_[324] = "SecurityStatusReqID";
            dictionary_[325] = "UnsolicitedIndicator";
            dictionary_[326] = "SecurityTradingStatus";
            dictionary_[327] = "HaltReason";
            dictionary_[328] = "InViewOfCommon";
            dictionary_[329] = "DueToRelated";
            dictionary_[330] = "BuyVolume";
            dictionary_[331] = "SellVolume";
            dictionary_[332] = "HighPx";
            dictionary_[333] = "LowPx";
            dictionary_[334] = "Adjustment";
            dictionary_[335] = "TradSesReqID";
            dictionary_[336] = "TradingSessionID";
            dictionary_[337] = "ContraTrader";
            dictionary_[338] = "TradSesMethod";
            dictionary_[339] = "TradSesMode";
            dictionary_[340] = "TradSesStatus";
            dictionary_[341] = "TradSesStartTime";
            dictionary_[342] = "TradSesOpenTime";
            dictionary_[343] = "TradSesPreCloseTime";
            dictionary_[344] = "TradSesCloseTime";
            dictionary_[345] = "TradSesEndTime";
            dictionary_[346] = "NumberOfOrders";
            dictionary_[347] = "MessageEncoding";
            dictionary_[348] = "EncodedIssuerLen";
            dictionary_[349] = "EncodedIssuer";
            dictionary_[350] = "EncodedSecurityDescLen";
            dictionary_[351] = "EncodedSecurityDesc";
            dictionary_[352] = "EncodedListExecInstLen";
            dictionary_[353] = "EncodedListExecInst";
            dictionary_[354] = "EncodedTextLen";
            dictionary_[355] = "EncodedText";
            dictionary_[356] = "EncodedSubjectLen";
            dictionary_[357] = "EncodedSubject";
            dictionary_[358] = "EncodedHeadlineLen";
            dictionary_[359] = "EncodedHeadline";
            dictionary_[360] = "EncodedAllocTextLen";
            dictionary_[361] = "EncodedAllocText";
            dictionary_[362] = "EncodedUnderlyingIssuerLen";
            dictionary_[363] = "EncodedUnderlyingIssuer";
            dictionary_[364] = "EncodedUnderlyingSecurityDescLen";
            dictionary_[365] = "EncodedUnderlyingSecurityDesc";
            dictionary_[366] = "AllocPrice";
            dictionary_[367] = "QuoteSetValidUntilTime";
            dictionary_[368] = "QuoteEntryRejectReason";
            dictionary_[369] = "LastMsgSeqNumProcessed";
            dictionary_[370] = "OnBehalfOfSendingTime";
            dictionary_[371] = "RefTagID";
            dictionary_[372] = "RefMsgType";
            dictionary_[373] = "SessionRejectReason";
            dictionary_[374] = "BidRequestTransType";
            dictionary_[375] = "ContraBroker";
            dictionary_[376] = "ComplianceID";
            dictionary_[377] = "SolicitedFlag";
            dictionary_[378] = "ExecRestatementReason";
            dictionary_[379] = "BusinessRejectRefID";
            dictionary_[380] = "BusinessRejectReason";
            dictionary_[381] = "GrossTradeAmt";
            dictionary_[382] = "NoContraBrokers";
            dictionary_[383] = "MaxMessageSize";
            dictionary_[384] = "NoMsgTypes";
            dictionary_[385] = "MsgDirection";
            dictionary_[386] = "NoTradingSessions";
            dictionary_[387] = "TotalVolumeTraded";
            dictionary_[388] = "DiscretionInst";
            dictionary_[389] = "DiscretionOffset";
            dictionary_[390] = "BidID";
            dictionary_[391] = "ClientBidID";
            dictionary_[392] = "ListName";
            dictionary_[393] = "TotalNumSecurities";
            dictionary_[394] = "BidType";
            dictionary_[395] = "NumTickets";
            dictionary_[396] = "SideValue1";
            dictionary_[397] = "SideValue2";
            dictionary_[398] = "NoBidDescriptors";
            dictionary_[399] = "BidDescriptorType";
            dictionary_[400] = "BidDescriptor";
            dictionary_[401] = "SideValueInd";
            dictionary_[402] = "LiquidityPctLow";
            dictionary_[403] = "LiquidityPctHigh";
            dictionary_[404] = "LiquidityValue";
            dictionary_[405] = "EFPTrackingError";
            dictionary_[406] = "FairValue";
            dictionary_[407] = "OutsideIndexPct";
            dictionary_[408] = "ValueOfFutures";
            dictionary_[409] = "LiquidityIndType";
            dictionary_[410] = "WtAverageLiquidity";
            dictionary_[411] = "ExchangeForPhysical";
            dictionary_[412] = "OutMainCntryUIndex";
            dictionary_[413] = "CrossPercent";
            dictionary_[414] = "ProgRptReqs";
            dictionary_[415] = "ProgPeriodInterval";
            dictionary_[416] = "IncTaxInd";
            dictionary_[417] = "NumBidders";
            dictionary_[418] = "TradeType";
            dictionary_[419] = "BasisPxType";
            dictionary_[420] = "NoBidComponents";
            dictionary_[421] = "Country";
            dictionary_[422] = "TotNoStrikes";
            dictionary_[423] = "PriceType";
            dictionary_[424] = "DayOrderQty";
            dictionary_[425] = "DayCumQty";
            dictionary_[426] = "DayAvgPx";
            dictionary_[427] = "GTBookingInst";
            dictionary_[428] = "NoStrikes";
            dictionary_[429] = "ListStatusType";
            dictionary_[430] = "NetGrossInd";
            dictionary_[431] = "ListOrderStatus";
            dictionary_[432] = "ExpireDate";
            dictionary_[433] = "ListExecInstType";
            dictionary_[434] = "CxlRejResponseTo";
            dictionary_[435] = "UnderlyingCouponRate";
            dictionary_[436] = "UnderlyingContractMultiplier";
            dictionary_[437] = "ContraTradeQty";
            dictionary_[438] = "ContraTradeTime";
            dictionary_[439] = "ClearingFirm";
            dictionary_[440] = "ClearingAccount";
            dictionary_[441] = "LiquidityNumSecurities";
            dictionary_[442] = "MultiLegReportingType";
            dictionary_[443] = "StrikeTime";
            dictionary_[444] = "ListStatusText";
            dictionary_[445] = "EncodedListStatusTextLen";
            dictionary_[446] = "EncodedListStatusText";
        }
    };

} // namespace hffix

#endif
