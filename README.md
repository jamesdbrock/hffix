High Frequency FIX C++ Library for Financial Information Exchange Protocol {#mainpage}
==========================================================================

## Introduction

The High Frequency FIX Parser library is an open source implementation of <a href="http://www.fixtradingcommunity.org/pg/structure/tech-specs/fix-protocol">Financial Information Exchange protocol versions 4.2, 4.3, 4.4, and 5.0 SP2.</a> intended for use by developers of high frequency, low latency financial software.  The purpose of the library is to do fast, efficient encoding and decoding of FIX in place, at the location of the I/O buffer, without using intermediate objects or allocating any memory on the free store.

Hffix library is not certified by any industry-leading committees. It is not an "engine." It is not an "adaptor." It has no threading, no I/O, no object-oriented inheritance. It does **no memory allocation** on the free store (the "heap"). It is just a superfast parser and serializer in plain modern generic-iterator-style C++03.

## Hello, FIX! Quick Start

The main repository is at <https://github.com/jamesdbrock/hffix>

### fixprint
To see an example of the library in action, enter these four commands at your shell prompt. This example uses the `fixprint` utility which comes with the hffix library. The result will be a colorized and pretty-printed FIX 5.0 test data set.

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    make fixprint
    util/bin/fixprint --color < test/data/fix.5.0.set.2 | less -R


### Development

The library is header-only, so there is nothing to link. To use the `hffix.hpp` library for C++ FIX development, place the two header files in your include path and `#include <hffix.hpp>`.

    git clone https://github.com/jamesdbrock/hffix.git
    cp hffix/include/hffix.hpp /usr/include/hffix.hpp
    cp hffix/include/hffix_fields.hpp /usr/include/hffix_fields.hpp

### Documentation

Full Doxygen is on the internet at <http://jamesdbrock.github.com/hffix>

To build the Doxygen html documentation in the `doc/html` directory and view it:

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    make doc
    xdg-open doc/html/index.html

## Library Design

High Frequency FIX Parser tries to follow the
<a href="http://www.boost.org/development/requirements.html">Boost Library Requirements and Guidelines</a>.  It is modern platform-independent C++98 and depends only on the C++ Standard Library.  It is patterned after the C++ Standard Template Library, and it models each FIX message with Container and Iterator concepts. It employs compile-time generic templates but does not employ object-oriented inheritance.

High Frequency FIX Parser is a one-header-only library, so there are no binaries to link. It also plays well with Boost. If you are using <a href="http://www.boost.org/doc/html/date_time.html">Boost Date_Time</a> in your application, High Frequency FIX Parser will support conversion between FIX fields and Boost Date_Time types.

The design criteria for High Frequency FIX Parser are based on our experience passing messages to various FIX hosts for high frequency quantitative trading at <a href="http://www.t3live.com">T3 Trading Group, LLC</a>.

All of the Financial Information Exchange (FIX) protocol specification versions supported by the library are bundled into the the distribution, in the `spec` directory. As a convenience for the developer, the High Frequency FIX Parser library includes a Python script which parses all of the FIX protocol specification documents and generates the `include/hffix_fields.hpp` file. That file has `enum` definitions in a tag namspace and an `hffix::dictionary_init_field` function which allows fields to be referred to by name instead of number both during development, and at run-time.

### Platforms

The main library `hffix.hpp` should be platform-independent C++03, but is only tested on Linux. 

The `spec/codegen` script for re-generating the `hffix_fields.hpp` file requires Python 2.7.

### License

The main High Frequency FIX Parser Library is distributed under the open source FreeBSD License, also known as the Simplified BSD License. 

Some extra components are under the Boost License.

Included FIX specs are copyright FIX Protocol, Limited.

## Features

### Other FIX Implementations

Typical FIX implementations model a FIX message either as an associative key-value container of strongly typed objects that inherit from some field superclass, or as a class type for each message type with member variables for every possible field in the message. Both of these approaches are inflexible and inefficient. The advantage of doing it this way is that you get random access reads and writes to the fields of the FIX message.

Every high frequency trading application already has an object model in the application that provides random access to properties of trading objects, however. A class for an order, and a class for an execution, and a class for a quote, et cetera. If your application has that, then it doesn't need random access to message fields, it needs only serial access.

### High Frequency FIX Parser Implementation

High Frequency FIX Parser models reading fields serially with an <a href="http://www.sgi.com/tech/stl/ForwardIterator.html">immutable Forward Iterator</a> and writing fields serially to a <a href="http://www.sgi.com/tech/stl/BackInsertionSequence.html">Back Insertion Sequence Container</a>.  It performs reading and writing directly on the I/O buffer, without any intermediate objects and without calling new or malloc or doing any free store allocations of any kind.

Field values are weakly typed. They are exposed to the developer as `char const* begin(), char const* end()`, and High Frequency FIX Parser provides a complete set of conversion functions for *ints*, *decimal floats*, *dates* and *times*, et cetera for the developer to call at his discretion.

### Exceptions

All High Frequency FIX Parser methods, functions, constructors, and destructors provide the No-Throw exception guarantee unless they are documented to throw exceptions, in which case they provide the Basic exception guarantee.

### Thread Safety

High Frequency FIX Parser is not thread-aware at all and has no threads, mutexes, locks, or atomic operations.

All `const` methods of the `hffix::message_reader` are safe for concurrent calls.

The `hffix::message_writer` is not safe for concurrent calls.

`hffix::message_reader` and `hffix::message_writer` have no storage of their own, they read and write fields directly on the buffer with which they are constructed. You must be sure the buffer endures until they are destroyed.

## FIX

### FIX Sessions

Managing sessions requires making choices about sockets and threads.  High Frequency FIX Parser does not manage sessions.  It is intended for developers who want a FIX parser with which to build a session manager for a high-performance trading system that already has a carefully crafted socket and threading architecture.

FIX has transport-layer features mixed in with the messages, and most FIX hosts have various quirks in the way they employ the administrative messages. To manage a FIX session your application will need to match the the transport-layer and administrative features of the other FIX host. High Frequency FIX Parser has the flexibility to express any subset or proprietary superset of FIX.

### Encryption

High Frequency FIX Parser supports the binary data field types such as *SecureData*, but it does not implement any of the *EncryptMethods* suggested by the FIX specifications. If you want to encrypt or decrypt some data you'll have to do the encryption or decryption yourself.

### CheckSum

High Frequency FIX Parser can calculate CheckSums and add the *CheckSum* field for all messages that you encode.  It does not validate the *CheckSum* of messages decoded.

### Sequence Numbers

The *MsgSeqNum* field in the FIX Standard Header is exposed for reading and writing.

### Administrative Messages

The administrative messages *Logon*, *Logout*, *ResendRequest*, *Heartbeat*, *TestRequest*, *SeqReset-Reset* and *SeqReset-GapFill* don't get special treatment in High Frequency FIX Parser. Any administrative message can be encoded or decoded like any other message.

### User-Defined Fields and Custom Tags

High Frequency FIX Parser does not enforce the data type ("Format") of the Field Definitions for content fields in the FIX spec, so the developer is free to read or write any field data type with any tag number.

### FIX Sessions

#### Implementing

Consult [FIX Session-level Test Cases and Expected Behaviors](http://www.fixtradingcommunity.org/pg/file/fplpo/read/30489/fix-sessionlevel-test-cases-and-expected-behaviors)

#### Problems with FIX

A FIX session When a FIX client connects to a FIX server, the client doesn't know what sequence number to use for the *Logon* message. 

Either the client can choose to reset both sequence numbers, in which case the client may miss messages, or not, in which case the client is subject to the *Resend Request* race condition.

After *Logon* response, the client may begin sending messages, but the client has to wait some amount of time because the server may send *Request Resend*.

*MessageLength* determination.

Repeating Groups, when do they end? After a field which is not part of the group. By convention, repeating groups are at the end of the message. Find each group by the "NoXXX" field.


## Using High Frequency FIX Parser

### Reading a FIX Message

Here is a simple example of how use `hffix::message_reader` to read a FIX message from a buffer and print each field to `std::cout`.

~~~
#include <hffix.hpp>

char buffer[512]; // Suppose we just read 75 bytes of FIX into this buffer.

std::map<int, std::string> field_dictionary;
hffix::dictionary_init_field(field_dictionary);

hffix::message_reader reader(buffer, buffer + 75);

if (reader.is_complete() && reader.is_valid())
{
    std::cout << "Message size is " << reader.message_size() << '.\n';

    for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i)
    {
        std::cout << hffix::field_name(i->tag(), field_dictionary);
        std::cout << "=";
        std::cout.write(i->value().begin(), i->value().size());
        std::cout << " ";
    }
}
~~~

Example Output:

~~~
Message size is 75.
MsgType=0 SenderCompID=AAAA TargetCompID=BBBB MsgSeqNum=1 SendingTime=20110130-19:14:21.056
~~~

### Writing a FIX Message

Here is a simple example of how to use hffix::message_writer to write a FIX Heartbeat message to a buffer.  The hffix::message_writer::push_back_header() and hffix::message_writer::push_back_trailer() methods will write the *BeginString*, *BodyLength*, and *CheckSum* transport fields for us.

~~~
#include <hffix.hpp>

char buffer[512];

hffix::message_writer writer(buffer, buffer + 512);

writer.push_back_header("FIX.4.2");

writer.push_back_string   (hffix::tag::MsgType, "0");
writer.push_back_string   (hffix::tag::SenderCompID, "AAAA");
writer.push_back_string   (hffix::tag::TargetCompID, "BBBB");
writer.push_back_int      (hffix::tag::MsgSeqNum , 1);
writer.push_back_timestamp(hffix::tag::SendingTime, boost::posix_time::microsec_clock::universal_time());

writer.push_back_trailer();

std::cout.write(writer.message_begin(), writer.message_size()) << '\n';
std::cout << "Wrote " << writer.message_size() << " bytes to buffer." << '\n';
~~~

Example Output:

~~~
8=FIX.4.2<SOH>9=0051<SOH>35=0<SOH>49=AAAA<SOH>56=BBBB<SOH>34=1<SOH>52=20110130-19:14:21.056<SOH>10=049<SOH>
Wrote 75 bytes to buffer.
~~~

### Reading From a Stream

This example is a simplified version of the `util/src/fixprint.cpp` program included in the repository. This basic pattern can be used to read FIX messages from any streaming source, such as a file or a TCP socket.

~~~

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <map>
#include <hffix.hpp>

enum { chunksize = 4096 }; // Choose a preferred I/O chunk size.

char buffer[chunksize * 4]; // Must be larger than the maximum FIX message size.

int main(int argc, char** argv)
{
    std::map<int, std::string> field_dictionary;
    hffix::dictionary_init_field(field_dictionary);

    size_t buffer_length(0); // The number of bytes read in buffer[].

    size_t fred; // Number of bytes read from fread().

    // Read chunks from stdin until 0 is read or the buffer fills up without finding a complete message.
    while((fred = std::fread(buffer + buffer_length, 1, std::min(sizeof(buffer) - buffer_length, size_t(chunksize)), stdin))) {

        buffer_length += fred;
        hffix::message_reader reader(buffer, buffer + buffer_length);

        // Try to read as many complete messages as there are in the buffer.
        for (; reader.is_complete(); reader = reader.next_message_reader()) {
            if (reader.is_valid()) {

                // Here is a complete message. Read fields out of the reader.
                try {
                    for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i) {

                        std::map<int, std::string>::iterator fname = field_dictionary.find(i->tag());
                        if (fname != field_dictionary.end())
                            std::cout << fname->second << '_'; // Print the name of the field, if it's known.
                        std::cout << i->tag();
                        std::cout << '=';
                        std::cout.write(i->value().begin(), i->value().size());
                        std::cout << ' ';
                    }

                    std::cout << '\n';
                } catch(std::exception& ex) {
                    std::cerr << "Error reading fields: " << ex.what() << '\n';
                }

            } else {
                // An invalid, corrupted FIX message. Do not try to read fields out of this reader.
                // The beginning of the invalid message is at location reader.message_begin() in the buffer,
                // but the end of the invalid message is unknown (because it's invalid).
                // Stay in this for loop, because the messager_reader::next_message_reader() function
                // will see that this message is invalid and it will search the remainder of the buffer
                // for the text "8=FIX", to see if there might be a complete or partial valid message
                // anywhere else in the remainder of the buffer.
                std::cerr << "Error Corrupt FIX message: ";
                std::cerr.write(reader.message_begin(), std::min(ssize_t(64), buffer + buffer_length - reader.message_begin()));
                std::cerr << "...\n";
            }
        }
        buffer_length = reader.buffer_end() - reader.buffer_begin();

        if (buffer_length > 0) // Then there is an incomplete message at the end of the buffer.
            std::memmove(buffer, reader.buffer_begin(), buffer_length); // Move the partial portion of the incomplete message to buffer[0].
    }
    return 0;
}
~~~





### A Complete Example

Here is a more complete example that shows reading, writing, and type conversions.

~~~

#include <iostream>
#include <string>
#include <hffix.hpp>

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

    logon_message.push_back_header("FIX.4.2"); // Write BeginString and BodyLength.
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
~~~


## Test

Sample data sources, discovered by Googling. If you own one of these data sets and you don't want me to use it, please send me an email. 

* `test/data/fix.4.1.set.1`    http://fixparser.targetcompid.com/
* `test/data/fix.5.0.set.1`    https://www.jse.co.za/content/JSETechnologyDocumentItems/03.%20JSE%20Indices.recv.log.txt
* `test/data/fix.5.0.set.2`    http://blablastreet.com/workshop/TestSocketServer/TestSocketServer/bin/Debug/store/FIXT.1.1-ATP1CMEMY-OMSCMEMY.body


## FAQ

Q: I have a bunch of different threads serializing and sending FIX messages out one socket. When each message is sent it needs a *MsgSeqNum*, but at serialization time I don't know what the *MsgSeqNum* will be, I only know that at sending time.

A: That multi-threading model is not a good choice for your software. The performance penalty for that threading model is much greater than the performance advantage of this non-allocating parser library. You should consider redesigning to use a single-threaded simultaneous-wait event loop like *libev* or *Boost Asio*. If you insist on multi-threading, then you could do something like this code example. 

~~~
hffix::message_writer m;
m.push_back_string(hffix::tag::MsgSeqNum, "00000000"); // Make a placeholder value over which you can later paste your sequence number.

// This thread_safe_send() function will correctly sequence FIX messages if two threads are racing to call thread_safe_send().
void thread_safe_send(hffix::message_writer const& w) {
  lock l(send_mutex_); // Serialize access to this function.
  hffix::message_reader r(w); // Construct a reader from the writer.
  hffix::message_reader::const_iterator i = std::find(r.begin(), r.end(), hffix::tag_predicate(hffix::tag::MsgSeqNum)); // Find the MsgSeqNum field.
  if (i != r.end()) {
    std::snprintf(const_cast<char*>(i->begin()), i->size(), "%.8i", next_sequence_number++); // Overwrite the "00000000" string with the next_sequence_number.
  write(fd, w.message_begin(), w.message_size()); // Send the message to the socket.
}
~~~
  
## Support

Want to talk? Email me at <jamesbrock@gmail.com>.

## Contributing

Pull requests welcome.

### TODO

* `dictionary_init_mdentrytype()` function.

* More support for BlockRepeating NoXXXX field types for FIX 5.0 SP2.

* Checks for buffer overflow in hffix::message_writer.

* Conversion validation for string-to-number.

#### C++14
* Use std::range for everything.
