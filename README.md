High Frequency FIX C++ Library for Financial Information Exchange Protocol 
==========================================================================

# Quick Start

## Development

The library is header-only. To use the `hffix.hpp` library for C++ FIX development, 
```cpp
cp include/hffix.hpp /usr/include/
```
## Documentation

To build the html documentation in the `doc/html` directory,
```
make doc
```

We all hate FIX but we all have to parse it, so let's make it as painless as possible.

Hffix library is not certified by any industry-leading committees of ambitious hustlers. It is not an "engine." It is not an "adaptor." It has no threading, no I/O, no object-oriented inheritance; it is just a non-allocating parser in plain modern generic-iterator-style C++.

# FIX

## Problems with FIX

A FIX session When a FIX client connects to a FIX server, the client doesn't know what sequence number to use for the Logon message. 

Either the client can choose to reset both sequence numbers, in which case the client may miss messages, or not, in which case ???

After Logon response, the client may begin sending messages, but the client has to wait some amount of time because the server may send Request Resend.

MessageLength determination.

Repeating Groups, when do they end? After a field which is not part of the group.


# Test

Sample data from 
http://fixparser.targetcompid.com/
https://www.jse.co.za/content/JSETechnologyDocumentItems/03.%20JSE%20Indices.recv.log.txt
# FAQ

Q: I have a bunch of different threads serializing and sending FIX messages out one socket. When each message is sent it needs a MsgSeqNum, but at serialization time I don't know what the MsgSeqNum will be, I only know that at sending time.
A: You're doing threading wrong. The performance penalty you pay for your threading model is much greater than the performance advantage of this non-allocating parser library, so you may as well use QuickFIX or something. If you insist on this course of action, then you could

hffix::message_writer m;
m.push_back_string(34, "00000000"); // Make a placeholder value over which you can later paste your sequence number.

void thread_safe_send(hffix::message_writer const& w) {
  hffix::message_reader r(w);
  hffix::message_reader::const_iterator i = std::find(r.begin(), r.end(), hffix::tag_predicate(34)); // Find the first MsgSeqNum field.
  if (i != r.end()) {
    std::snprintf(i->begin(), i->size(), "%.8i", next_sequence_number); // Overwrite the "00000000" string with the next_sequence_number.
}
  
# TODO

Make all members public, eliminate all accessor methods.
No /**/-style comments
fixprint --color

Make a find_with_hint function

message_reader::const_iterator i = reader.begin();
if ((i = find_with_hint(reader.begin(), reader.end(), i, tag_predicate(MsgSeqNum))) != reader.end()) {
  int seqnum = i->cast_int();
}
if ((i = find_with_hint(reader.begin(), reader.end(), i, tag_predicate(TargetCompID))) != reader.end()) {
  std::string targetcompid = i->cast_string();
}
