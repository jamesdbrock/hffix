#include <iostream>
#include <cstdio>
#include <algorithm>
#include <hffix.hpp>

enum { blocksize = 4096 }; // Choose a preferred I/O block size.

char buffer[blocksize * 4];

int main(int argc, char** arcv)
{
    std::map<int, std::string> dictionary;
    hffix::field_dictionary_init(dictionary);

    size_t buffer_length(0);

    for (
        size_t fred = std::fread(buffer + buffer_length, 1, std::min(sizeof(buffer) - buffer_length, size_t(blocksize)), stdin);
        fred > 0;
        fred = std::fread(buffer + buffer_length, 1, std::min(sizeof(buffer) - buffer_length, size_t(blocksize)), stdin)
    ) {
        buffer_length += fred;
        hffix::message_reader reader(buffer, buffer + buffer_length);

        // Try to read as many complete messages as there are in the buffer.
        for (; reader.is_complete(); reader = reader.next_message_reader()) {
            if (reader.is_valid()) {

                // Here is a complete message. Read fields out of the reader.
                for(hffix::message_reader::const_iterator i = reader.begin(); i != reader.end(); ++i) {
                    std::map<int, std::string>::iterator fname = dictionary.find(i->tag());
                    if (fname != dictionary.end()) 
                        std::cout << fname->second << "_";
                    std::cout << i->tag();
                    std::cout << "=";
                    std::cout.write(i->value().begin(), i->value().size());
                    std::cout << " ";
                }
                std::cout << "\n";
            } else {
                // An invalid, corrupted FIX message. Do not try to read fields out of this reader.
                // The beginning of the invalid message is at location reader.message_begin() in the buffer,
                // but the end of the invalid message is unknown (because it's invalid).
                // Stay in this for loop, because the messager_reader::operator++()
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
