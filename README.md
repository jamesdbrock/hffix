High Frequency FIX &mdash; C++ Library for Financial Information Exchange Protocol {#mainpage}
==========================================================================

## Introduction

The High Frequency FIX Parser library is an open source implementation of <a href="http://www.fixtradingcommunity.org/pg/structure/tech-specs/fix-protocol">Financial Information Exchange protocol versions 4.2, 4.3, 4.4, and 5.0 SP2.</a> intended for use by developers of high frequency, low latency financial software.  The purpose of the library is to do fast, efficient encoding and decoding of FIX in place, at the location of the I/O buffer. The library does not use intermediate message objects, and it does **no memory allocation** on the free store (the &ldquo;heap&rdquo;).

Hffix library is not certified by any industry-leading committees. It is not an &ldquo;engine.&rdquo; It is not an &ldquo;adaptor.&rdquo; It has no threading, no I/O, no object-oriented inheritance.  It is just a superfast parser and serializer in plain modern generic-iterator-style C++03.

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
    cp hffix/include/hffix.hpp /usr/local/include/
    cp hffix/include/hffix_fields.hpp /usr/local/include/

### Documentation

Full Doxygen is on the internet at <http://jamesdbrock.github.com/hffix>

To build the Doxygen html documentation in the `doc/html` directory and view it:

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    make doc
    xdg-open doc/html/index.html

## Library Design

High Frequency FIX Parser tries to follow the
<a href="http://www.boost.org/development/requirements.html">Boost Library Requirements and Guidelines</a>.  It is modern platform-independent C++03 and depends only on the C++ Standard Library.  It is patterned after the C++ Standard Template Library, and it models each FIX message with Container and Iterator concepts. It employs compile-time generic templates but does not employ object-oriented inheritance.

High Frequency FIX Parser is a header-only library, so there are no binaries to link. It also plays well with Boost. If you are using <a href="http://www.boost.org/doc/html/date_time.html">Boost Date_Time</a> in your application, High Frequency FIX Parser will support conversion between FIX fields and Boost Date_Time types.

All of the Financial Information Exchange (FIX) protocol specification versions supported by the library are bundled into the the distribution, in the `spec` directory. As a convenience for the developer, the High Frequency FIX Parser library includes a Python script which parses all of the FIX protocol specification documents and generates the `include/hffix_fields.hpp` file. That file has `enum` definitions in a tag namspace and an `hffix::dictionary_init_field` function which allows fields to be referred to by name instead of number both during development, and at run-time.

The design criteria for High Frequency FIX Parser are based on our experience passing messages to various FIX hosts for high frequency quantitative trading at <a href="http://www.t3live.com">T3 Trading Group, LLC</a>.

### Platforms

The main library `hffix.hpp` should be platform-independent C++03, but is only tested on Linux and *gcc*. 

The `spec/codegen` script for re-generating the `hffix_fields.hpp` file requires Python 2.7.

### License

The main High Frequency FIX Parser Library is distributed under the open source FreeBSD License, also known as the Simplified BSD License. 

Some extra components are under the Boost License.

Included FIX specs are copyright FIX Protocol, Limited.

## Features

### Other FIX Implementations

Typical FIX implementations employ object-oriented-style programming to model a FIX message either as an associative key-value container of strongly typed objects that inherit from some field superclass, or as a class type for each message type with member variables for every possible field in the message. 

There are two disadvantages to this method.

1. Creating these message objects requires free-store memory allocation, which uses a lot of CPU time &mdash; typically more CPU time than all the rest of the parsing logic.
2. Declaring the classes for these objects requires a lot of boilerplate code, and makes it difficult to handle surprising messages at run-time. 

The advantage of object-oriented-style FIX parsers is that with the familiar object API, any field of a message object can be read or written randomly at any point in the program, which may simplify program logic. 

### High Frequency FIX Parser Implementation

For reading FIX messages, High Frequency FIX Parser presents an STL-style <a href="http://www.sgi.com/tech/stl/ForwardIterator.html">immutable Forward Iterator</a> interface. Writing fields is done serially with an interface similar to an STL-style <a href="http://www.sgi.com/tech/stl/BackInsertionSequence.html">Back Insertion Sequence Container</a>. Reading and writing are done directly on the I/O buffer, without any intermediate objects.  

The disadvantage of this implementation is that the message API provides serial access to fields, not random access.

The advantage is that this enables the High Frequency FIX Parser library to completely avoid free store memory allocation.
The library performs all memory allocation on the stack, and the library never requires developers using the library to allocate anything on the free store with `new` or `malloc`.

Field values in the FIX protocol are always encoded on the wire as ASCII, and High Frequency FIX Parser exposes field values to the developer as iterator range `char const* begin(), char const* end()`. High Frequency FIX Parser also provides a complete set of conversion functions to native C++ types for *ints*, *decimal floats*, *dates* and *times*, et cetera &mdash; see documentation for `hffix::message_writer` and `hffix::field_value`.

### Exceptions

All High Frequency FIX Parser methods, functions, constructors, and destructors provide the No-Throw exception guarantee unless they are documented to throw exceptions, in which case they provide the Basic exception guarantee. Generally, a function may throw `std::logic_error` if preconditions are not met by the programmer. See documentation for details.

### Thread Safety

High Frequency FIX Parser is not thread-aware at all and has no threads, mutexes, locks, or atomic operations.

All `const` methods of the `hffix::message_reader` are safe for concurrent calls.

The `hffix::message_writer` is not safe for concurrent calls.

`hffix::message_reader` and `hffix::message_writer` have no storage of their own, they read and write fields directly on an I/O buffer. The developer must guarantee that the buffer endures while fields are being read or written.

### FIX Sessions

Managing sessions requires making choices about sockets and threads.  High Frequency FIX Parser does not manage sessions.  It is intended for developers who want a FIX parser with which to build a session manager for a high-performance trading system that already has a socket and threading architecture.

FIX has transport-layer features mixed in with the messages, and most FIX hosts have various quirks in the way they employ the administrative messages. To manage a FIX session your application will need to match the the transport-layer and administrative features of the other FIX host. High Frequency FIX Parser has the flexibility to express any subset or proprietary superset of FIX.

Consult [FIX Session-level Test Cases and Expected Behaviors](http://www.fixtradingcommunity.org/pg/file/fplpo/read/30489/fix-sessionlevel-test-cases-and-expected-behaviors)

### Encryption

High Frequency FIX Parser supports the binary data field types such as *SecureData*, but it does not implement any of the *EncryptMethods* suggested by the FIX specifications. If you want to encrypt or decrypt some data you'll have to do the encryption or decryption yourself.

### CheckSum

High Frequency FIX Parser can calculate CheckSums and add the *CheckSum* field for all messages that you encode.  It does not validate the *CheckSum* of messages decoded.

### Sequence Numbers

The *MsgSeqNum* field in the FIX Standard Header is exposed for reading and writing.

### Administrative Messages

The administrative messages *Logon*, *Logout*, *ResendRequest*, *Heartbeat*, *TestRequest*, *SeqReset-Reset* and *SeqReset-GapFill* don't get special treatment in High Frequency FIX Parser. Any administrative message can be encoded or decoded like any other message.

### User-Defined Fields and Custom Tags

High Frequency FIX Parser does not enforce the data type of the Field Definitions for content fields in the FIX spec, so the developer is free to read or write any tag number with any field data type.




## Using High Frequency FIX Parser

### Writing a Message Example

This example program is in the _hffix_ repository at `test/src/writer01.cpp`.

It writes a _Logon_ message and a _New Order - Single_ message to `stdout`.

~~~cpp
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
~~~


### Reading a Message Example

This example program is in the _hffix_ repository at `test/src/reader01.cpp`.

It reads messages from `stdin`. If it finds a _Logon_ message or a _New Order - Single_ message, then it prints out some information about their fields.

~~~cpp 
#include <iostream>
#include <cstdio>
#include <map>

// We want Boost Date_Time support, so include these before hffix.hpp.
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <hffix.hpp>

const size_t chunksize = 4096; // Choose a preferred I/O chunk size.

char buffer[1 << 20]; // Must be larger than the largest FIX message size.

int main(int argc, char** argv)
{
    std::map<int, std::string> field_dictionary;
    hffix::dictionary_init_field(field_dictionary);

    size_t buffer_length(0); // The number of bytes read in buffer[].

    size_t fred; // Number of bytes read from fread().

    // Read chunks from stdin until 0 is read or the buffer fills up without finding a complete message.
    while((fred = std::fread(buffer + buffer_length, 1, std::min(sizeof(buffer) - buffer_length, chunksize), stdin))) {

        buffer_length += fred;
        hffix::message_reader reader(buffer, buffer + buffer_length);

        // Try to read as many complete messages as there are in the buffer.
        for (; reader.is_complete(); reader = reader.next_message_reader()) {
            if (reader.is_valid()) {

                // Here is a complete message. Read fields out of the reader.
                try {
                    if (reader.message_type()->value() == "A") { // Logon message
                        std::cout << "Logon message\n";

                        hffix::message_reader::const_iterator i = reader.begin();

                        if (reader.find_with_hint(hffix::tag::SenderCompID, i))
                            std::cout << "SenderCompID = " << i++->value() << '\n';

                        if (reader.find_with_hint(hffix::tag::MsgSeqNum, i))
                            std::cout << "MsgSeqNum    = " << i++->value().as_int<int>() << '\n';

                        if (reader.find_with_hint(hffix::tag::SendingTime, i))
                            std::cout << "SendingTime  = " << i++->value().as_timestamp() << '\n';

                        std::cout << "The next field is " << hffix::field_name(i->tag(), field_dictionary) << " = " << i->value() << '\n';

                        std::cout << '\n';
                    }
                    else if (reader.message_type()->value() == "D") { // New Order Single message
                        std::cout << "New Order Single message\n";

                        hffix::message_reader::const_iterator i = reader.begin();

                        if (reader.find_with_hint(hffix::tag::Side, i))
                            std::cout << (i++->value().as_char() == '1' ? "Buy " : "Sell ");

                        if (reader.find_with_hint(hffix::tag::Symbol, i))
                            std::cout << i++->value() << " ";

                        if (reader.find_with_hint(hffix::tag::OrderQty, i))
                            std::cout << i++->value().as_int<int>();

                        if (reader.find_with_hint(hffix::tag::Price, i)) {
                            int mantissa, exponent;
                            i->value().as_decimal(mantissa, exponent);
                            std::cout << " @ $" << mantissa << "E" << exponent;
                            ++i;
                        }

                        std::cout << "\n\n";
                    }
                        
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

### Running the Examples

The writer example can be piped to the reader example. Running these commands:

~~~
make examples
test/bin/writer01 | test/bin/reader01 
~~~

Should produce output like this:

<pre style="white-space:pre-wrap;background-color:#171717;padding:1em;">
<span style="color: #F5F1DE">Logon message
SenderCompID = AAAA
MsgSeqNum =    1
SendingTime =  2014-Sep-26 15:27:38.789000
The next field is EncryptMethod = 0<br/>
New Order Single message
Buy OIH 100 @ $50001E-2
</span>
</pre>


To examine the output from `test/bin/writer01` program, you can also use `util/bin/fixprint`, like this:

~~~
make examples
make fixprint
test/bin/writer01 | util/bin/fixprint --color
~~~

Which will produce output like this:

<pre style="white-space:pre-wrap;background-color:#171717;padding:1em;">
<span style="color: #F5F1DE">FIX.4.2 </span><span style="color: #aa5500">MsgType_35=</span><span style="color: #aa0000">A_Logon </span><span style="color: #aa5500">SenderCompID_49=</span><span style="color: #F5F1DE">AAAA </span><span style="color: #aa5500">TargetCompID_56=</span><span style="color: #F5F1DE">BBBB </span><span style="color: #aa5500">MsgSeqNum_34=</span><span style="color: #F5F1DE">1 </span><span style="color: #aa5500">SendingTime_52=</span><span style="color: #F5F1DE">20140928-07:12:06.000 </span><span style="color: #aa5500">EncryptMethod_98=</span><span style="color: #F5F1DE">0 </span><span style="color: #aa5500">HeartBtInt_108=</span><span style="color: #F5F1DE">10</span>
<span style="color: #F5F1DE">FIX.4.2 </span><span style="color: #aa5500">MsgType_35=</span><span style="color: #aa0000">D_NewOrderSingle </span><span style="color: #aa5500">SenderCompID_49=</span><span style="color: #F5F1DE">AAAA </span><span style="color: #aa5500">TargetCompID_56=</span><span style="color: #F5F1DE">BBBB </span><span style="color: #aa5500">MsgSeqNum_34=</span><span style="color: #F5F1DE">2 </span><span style="color: #aa5500">SendingTime_52=</span><span style="color: #F5F1DE">20140928-07:12:06.000 </span><span style="color: #aa5500">ClOrdID_11=</span><span style="color: #F5F1DE">A1 </span><span style="color: #aa5500">HandlInst_21=</span><span style="color: #F5F1DE">1 </span><span style="color: #aa5500">Symbol_55=</span><span style="color: #F5F1DE">OIH </span><span style="color: #aa5500">Side_54=</span><span style="color: #F5F1DE">1 </span><span style="color: #aa5500">TransactTime_60=</span><span style="color: #F5F1DE">20140928-07:12:06.000 </span><span style="color: #aa5500">OrderQty_38=</span><span style="color: #F5F1DE">100 </span><span style="color: #aa5500">OrdType_40=</span><span style="color: #F5F1DE">2 </span><span style="color: #aa5500">Price_44=</span><span style="color: #F5F1DE">500.01 </span><span style="color: #aa5500">TimeInForce_59=</span><span style="color: #F5F1DE">1</span>
</pre>



## Boost Date_Time

If the <a href="http://www.boost.org/doc/html/date_time.html">Boost Date_Time</a> library is available in your build environment, `boost::posix_time::ptime`, `boost::posix_time::time_duration`, and `boost::gregorian::date` will be automatically supported for the various FIX date and time field types.

To enable High Frequency FIX Parser support for the Boost Date_Time library types, include the Boost libraries before the hffix.hpp library, like this:

~~~cpp
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <hffix.hpp>
~~~

To prevent High Frequency FIX Parser support for the Boost Date_Time library, `#define HFFIX_NO_BOOST_DATETIME` before including `hffix.hpp`:

~~~cpp
#define HFFIX_NO_BOOST_DATETIME
#include <hffix.hpp>
~~~

## Test

Sample data sources, discovered by Googling.

* `test/data/fix.4.1.set.1`    http://fixparser.targetcompid.com/
* `test/data/fix.5.0.set.1`    https://www.jse.co.za/content/JSETechnologyDocumentItems/03.%20JSE%20Indices.recv.log.txt
* `test/data/fix.5.0.set.2`    http://blablastreet.com/workshop/TestSocketServer/TestSocketServer/bin/Debug/store/FIXT.1.1-ATP1CMEMY-OMSCMEMY.body


## Cookbook

### Multi-threaded Sending

Q: I have a bunch of different threads serializing and sending FIX messages out one socket. When each message is sent it needs a *MsgSeqNum*, but at serialization time I don't know what the *MsgSeqNum* will be, I only know that at sending time.

A: That multi-threading model is not a good choice for your software. The performance penalty for that threading model is much greater than the performance advantage of this non-allocating parser library. You should consider redesigning to use a single-threaded simultaneous-wait event loop like *libev* or *Boost Asio*. If you insist on multi-threading, then you could do something like this code example. 

~~~cpp
hffix::message_writer m;
m.push_back_string(hffix::tag::MsgSeqNum, "00000000"); // Make a placeholder value over which you can later paste your sequence number.

// This thread_safe_send() function will correctly sequence FIX messages if two threads are racing to call thread_safe_send().
void thread_safe_send(hffix::message_writer const& w) {
  lock l(send_mutex_); // Serialize access to this function.
  hffix::message_reader r(w); // Construct a reader from the writer.
  hffix::message_reader::const_iterator i = std::find(r.begin(), r.end(), hffix::tag_equal(hffix::tag::MsgSeqNum)); // Find the MsgSeqNum field.
  if (i != r.end()) {
    std::snprintf(const_cast<char*>(i->begin()), i->size(), "%.8i", next_sequence_number++); // Overwrite the "00000000" string with the next_sequence_number.
  write(fd, w.message_begin(), w.message_size()); // Send the message to the socket.
}
~~~
  
### FIX Repeating Groups

The beginning of each Repeating Group is marked by a field with a &ldquo;NoXXX&rdquo; field.
The end of a Repeating Group is at the first occurence of a field which is not part of the group. By convention, Repeating Groups are usually located at the end of the message, so the end of the message marks the end of the Repeating Group.

From *FIX-50_SP2_VOL-1_w_Errata_20110818.pdf* page 21:

<blockquote>If the repeating group is used, the first field of the repeating group is required. This allows
implementations of the protocol to use the first field as a "delimiter" indicating a new repeating group
entry. The first field listed after the NoXXX, then becomes conditionally required if the NoXXX field
is greater than zero.</blockquote>

This is an example of iterating over the nested Repeating Groups when reading a *Mass Quote* message.
The *Mass Quote* message has *QuoteSet* Repeating Groups, and nested inside those groups are *QuoteEntry* Repeating Groups, see *fix-42-with_errata_20010501.pdf* page 52.
In each repeated *QuoteSet* Group, `hffix::tag::QuoteSetID` is always the first field. In each repeated *QuoteEntry* Group, `hffix::tag::QuoteEntryID` is always the first field.

~~~cpp
hffix::message_reader r; 

hffix::message_reader::const_iterator group1_begin = std::find_if(r.begin(), r.end(), hffix::tag_equal(hffix::tag::QuoteSetID));

if (group1_begin != r.end()) { // There is at least one QuoteSet group.
    
    for (
        hffix::message_reader::const_iterator group1_end = std::find_if(group1_begin + 1, r.end(), hffix::tag_equal(hffix::tag::QuoteSetID));
        group1_begin != r.end();
        group1_begin = group1_end, group1_end = std::find_if(group1_begin + 1, r.end(), hffix::tag_equal(hffix::tag::QuoteSetID))
    ) {
        // This loop body will be entered once for each QuoteSet Repeating Group.
        // 
        // group1_begin will point to the first field in the QuoteSet group, which is always hffix::tag::QuoteSetID.
        // group1_end   will point past-the-end of the QuoteSet group.

        group2_begin = std::find_if(group1_begin, group1_end, hffix::tag_equal(hffix::tag::QuoteEntryID));

        if (group2_begin != group1_end) { // There is at least one QuoteEntry group.

            for (
                hffix::message_reader::const_iterator group2_end = std::find_if(group2_begin + 1, group1_end, hffix::tag_equal(hffix::tag::QuoteEntryID));
                group2_begin != group1_end;
                group2_begin = group2_end, group2_end = std::find_if(group2_begin + 1, group1_end, hffix::tag_equal(hffix::tag::QuoteEntryID))
            ) {
                // This loop body will be entered once for each QuoteEntry Repeating Group.
                //
                // group2_begin will point to the first field of the QuoteEntry group, which is always QuoteEntryID.
                // group2_end   will point past-the-end of the QuoteEntry group.
            }
        }
    }
}
~~~



## Support

Want to talk? Email me at <jamesbrock@gmail.com>.



## Contributing

Pull requests welcome.




## Notes on the Design of FIX Protocol

### The *Logon* - *Resend Request* Race Condition

A FIX session When a FIX client connects to a FIX server, the client doesn't know what sequence number to use for the *Logon* message. 

Either the client can choose to reset both sequence numbers, in which case the client may miss messages, or not, in which case the client is subject to the *Resend Request* race condition.

After *Logon* response, the client may begin sending messages, but the client has to wait some amount of time because the server may send *Resend Request*. If the client sends any message to the server while the server is preparing to send *Resend Request*, then the server's response is not defined by the *FIX* specification, and some servers implementations may seize up in confusion at that point.




## TODO

* `dictionary_init_mdentrytype()` function.

* More support for BlockRepeating NoXXX field types for FIX 5.0 SP2.

* Checks for buffer overflow in hffix::message_writer.

* Lexical cast validation for hffix::message_reader.

### C++14

