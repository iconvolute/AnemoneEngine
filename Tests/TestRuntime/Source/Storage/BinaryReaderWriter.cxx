#include "AnemoneStorage/BinaryReader.hxx"
#include "AnemoneStorage/BinaryWriter.hxx"
#include "AnemoneBase/MemoryBuffer.hxx"
#include "AnemoneStorage/MemoryInputStream.hxx"
#include "AnemoneStorage/MemoryOutputStream.hxx"

#include <catch_amalgamated.hpp>


TEST_CASE("Storage Binary Reader Writer")
{
    using namespace Anemone;

    if (auto buffer = MemoryBuffer::Create(0))
    {
        {
            BinaryWriter writer{MakeReference<MemoryOutputStream>(*buffer)};

            writer << float{2.1f};
            writer << double{3.7};
            writer << int8_t{1};
            writer << int16_t{2};
            writer << int32_t{3};
            writer << int64_t{4};
            writer << uint8_t{5};
            writer << uint16_t{6};
            writer << uint32_t{7};
            writer << uint64_t{8};
        }
        float floatValue{};
        double doubleValue{};
        int8_t int8Value{};
        int16_t int16Value{};
        int32_t int32Value{};
        int64_t int64Value{};
        uint8_t uint8Value{};
        uint16_t uint16Value{};
        uint32_t uint32Value{};
        uint64_t uint64Value{};

        {
            BinaryReader reader{MakeReference<MemoryInputStream>(*buffer)};

            reader >> floatValue;
            reader >> doubleValue;
            reader >> int8Value;
            reader >> int16Value;
            reader >> int32Value;
            reader >> int64Value;
            reader >> uint8Value;
            reader >> uint16Value;
            reader >> uint32Value;
            reader >> uint64Value;
        }

        REQUIRE(floatValue == Catch::Approx{2.1f});
        REQUIRE(doubleValue == Catch::Approx{3.7f});
        REQUIRE(int8Value == 1);
        REQUIRE(int16Value == 2);
        REQUIRE(int32Value == 3);
        REQUIRE(int64Value == 4);
        REQUIRE(uint8Value == 5);
        REQUIRE(uint16Value == 6);
        REQUIRE(uint32Value == 7);
        REQUIRE(uint64Value == 8);
    }
}
