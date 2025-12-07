#include "AnemoneRuntime.Interop/RingBuffer.hxx"

#include <catch_amalgamated.hpp>

TEST_CASE("Ring Buffer")
{
    using namespace Anemone::Interop;

    std::array<std::byte, 32> buffer{};

    ring_buffer_view view{buffer};

    REQUIRE(view.available_space() == 31);
    REQUIRE(view.available_data() == 0);

    // Write uin64_t values
    uint64_t write1{0x1234567890abcdef};
    REQUIRE(view.write(std::as_bytes(std::span{&write1, 1})));
    REQUIRE(view.available_space() == 23);
    REQUIRE(view.available_data() == 8);

    uint64_t write2{0xabcdef1234567890};
    REQUIRE(view.write(std::as_bytes(std::span{&write2, 1})));
    REQUIRE(view.available_space() == 15);
    REQUIRE(view.available_data() == 16);

    uint64_t write3{0xdeadbeefdeadbeef};
    REQUIRE(view.write(std::as_bytes(std::span{&write3, 1})));
    REQUIRE(view.available_space() == 7);
    REQUIRE(view.available_data() == 24);

    // Can't write more than available space
    uint64_t write4{0xcafebabefeedcafe};
    REQUIRE_FALSE(view.write(std::as_bytes(std::span{&write4, 1})));
    REQUIRE(view.available_space() == 7);
    REQUIRE(view.available_data() == 24);

    // Read some data from the front.
    uint64_t read12[2]{};
    REQUIRE(view.read(std::as_writable_bytes(std::span{read12, 2})));
    REQUIRE(view.available_space() == 23);
    REQUIRE(view.available_data() == 8);
    REQUIRE(read12[0] == write1);
    REQUIRE(read12[1] == write2);

    // Try to write value4 once again
    REQUIRE(view.write(std::as_bytes(std::span{&write4, 1})));
    REQUIRE(view.available_space() == 15);
    REQUIRE(view.available_data() == 16);

    uint64_t read3{};
    REQUIRE(view.read(std::as_writable_bytes(std::span{&read3, 1})));
    REQUIRE(view.available_space() == 23);
    REQUIRE(view.available_data() == 8);
    REQUIRE(read3 == write3);

    uint64_t write5{0x7839b0c0d0f0a0c0};
    REQUIRE(view.write(std::as_bytes(std::span{&write5, 1})));
    REQUIRE(view.available_space() == 15);
    REQUIRE(view.available_data() == 16);

    uint64_t read4{};
    REQUIRE(view.read(std::as_writable_bytes(std::span{&read4, 1})));
    REQUIRE(view.available_space() == 23);
    REQUIRE(view.available_data() == 8);
    REQUIRE(read4 == write4);

    uint64_t read5{};
    REQUIRE(view.read(std::as_writable_bytes(std::span{&read5, 1})));
    REQUIRE(view.available_space() == 31);
    REQUIRE(view.available_data() == 0);
    REQUIRE(read5 == write5);
}
