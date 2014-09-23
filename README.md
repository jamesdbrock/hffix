High Frequency FIX C++ Library for Financial Information Exchange Protocol 
==========================================================================

We all hate FIX but we all have to parse it, so let's make it as painless as possible.

Hffix library is not certified by any industry-leading committees. It is not an "engine." It is not an "adaptor." It has no threading, no I/O, no object-oriented inheritance. It does **no memory allocation** on the free store (the "heap"). It is just a superfast parser and serializer in plain modern generic-iterator-style C++03.

## Hello, FIX! Quick Start

### fixprint
To see an example of the library in action, enter these four commands at your shell prompt. This example uses the `fixprint` utility which comes with the hffix library. The result will be a colorized and pretty-printed FIX 5.0 test data set.

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    make fixprint
    util/bin/fixprint --color < test/fix.5.0.set.2 | less -R


### Development

The library is header-only. To use the `hffix.hpp` library for C++ FIX development: 

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    cp include/*.hpp /usr/include/

### Documentation

To build the Doxygen html documentation in the `doc/html` directory and view it:

    git clone https://github.com/jamesdbrock/hffix.git
    cd hffix
    make doc
    xdg-open doc/html/index.html

## FIX

### Problems with FIX

A FIX session When a FIX client connects to a FIX server, the client doesn't know what sequence number to use for the *Logon* message. 

Either the client can choose to reset both sequence numbers, in which case the client may miss messages, or not, in which case the client is subject to the *Resend Request* race condition.

After *Logon* response, the client may begin sending messages, but the client has to wait some amount of time because the server may send *Request Resend*.

*MessageLength* determination.

Repeating Groups, when do they end? After a field which is not part of the group. By convention, repeating groups are at the end of the message. Find each group by the "NoXXX" field.


## Test

Sample data sources, discovered by Googling. If you own one of these data sets and you don't want me to use it, please send me an email. 

* `test/fix.4.1.set.1`    http://fixparser.targetcompid.com/
* `test/fix.5.0.set.1`    https://www.jse.co.za/content/JSETechnologyDocumentItems/03.%20JSE%20Indices.recv.log.txt
* `test/fix.5.0.set.2`    http://blablastreet.com/workshop/TestSocketServer/TestSocketServer/bin/Debug/store/FIXT.1.1-ATP1CMEMY-OMSCMEMY.body


## FAQ

Q: I have a bunch of different threads serializing and sending FIX messages out one socket. When each message is sent it needs a *MsgSeqNum*, but at serialization time I don't know what the *MsgSeqNum* will be, I only know that at sending time.

A: You're doing threading wrong. The performance penalty you pay for your threading model is much greater than the performance advantage of this non-allocating parser library, so you may as well use QuickFIX or something. If you insist on multi-threading, then you could do something like this code example. This `thread_safe_send()` function will correctly sequence FIX messages if two threads are racing to call `thread_safe_send()`.

```cpp
hffix::message_writer m;
m.push_back_string(hffix::tag::MsgSeqNum, "00000000"); // Make a placeholder value over which you can later paste your sequence number.

void thread_safe_send(hffix::message_writer const& w) {
  lock l(send_mutex_); // Serialize access to this function.
  hffix::message_reader r(w); // Construct a reader from the writer.
  hffix::message_reader::const_iterator i = std::find(r.begin(), r.end(), hffix::tag_predicate(hffix::tag::MsgSeqNum)); // Find the MsgSeqNum field.
  if (i != r.end()) {
    std::snprintf(const_cast<char*>(i->begin()), i->size(), "%.8i", next_sequence_number); // Overwrite the "00000000" string with the next_sequence_number.
  write(fd, w.message_begin(), w.message_size()); // Send the message to the socket.
}
```
  
## Contributing

Patches welcome.

### TODO

`dictionary_init_mdentrytype()` function.
