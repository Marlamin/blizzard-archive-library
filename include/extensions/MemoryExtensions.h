#pragma once
#include <string>
#include <sstream>

namespace BlizzardDatabaseLib {
    namespace Extension {

        class Memory
        {
        public:
            bool static IsEmpty(char* data, size_t length)
            {
                return memcmp(data, data + 1, length - 1) == 0;
            }

            static void Dump(const char* mem, size_t length) {
                char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
                std::stringstream ss;
                for (int i = 0; i < length; ++i)
                {
                    char const byte = mem[i];

                    ss << hex_chars[(byte & 0xF0) >> 4];
                    ss << hex_chars[(byte & 0x0F) >> 0];

                    std::cout << ss.str();
                }

                std::cout << std::endl;
            }
        };
    }
}