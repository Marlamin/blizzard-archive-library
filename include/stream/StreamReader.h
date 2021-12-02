#pragma once
#include <fstream>
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <structures/Types.h>

namespace BlizzardDatabaseLib {
    namespace Stream {

        class StreamReader
        {
        private:
            std::shared_ptr<std::ifstream> _underlyingStream;
        public:
            StreamReader(std::shared_ptr<std::ifstream> stream);
            ~StreamReader();
            template <typename T> T Read()
            {
                auto type = T();
                auto size = sizeof(T);
                auto pointer = reinterpret_cast<char*>(&type);
                _underlyingStream->read(pointer, size);
                return type;
            }

            template <typename T> std::vector<T> ReadArray(std::size_t length)
            {
                auto vector = std::vector<T>(length);
                auto size = sizeof(T);
                auto pointer = reinterpret_cast<char*>(vector.data());
                _underlyingStream->read(pointer, size * length);
                return vector;
            }

            std::unique_ptr<char[]> ReadBlock(std::size_t length);
            std::string ReadString();
            std::string ReadString(std::size_t length);
            void Jump(std::streampos position);
            void JumpEnd();
            void JumpStart();
            std::size_t Length();
            std::streampos Position();
            bool Good();
        };
    }
}