#include "AnemoneRuntime.Interop/MemoryBuffer.hxx"

#include <catch_amalgamated.hpp>

TEST_CASE("Memory Buffer")
{
    using namespace Anemone::Interop;

    SECTION("Resize for overwrite")
    {
        memory_buffer<4> buffer{};

        // Zero state.
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        // Resize to 2 bytes.
        buffer.resize_for_override(2);

        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        // Resize beyond static capacity.
        buffer.resize_for_override(8);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer.capacity() == 8);
        REQUIRE(buffer.is_dynamic());

        // Shrink down below static capacity.
        buffer.resize_for_override(2);

        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        // Resize to upper static capacity.
        buffer.resize_for_override(4);

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        // Resize beyond static capacity.
        buffer.resize_for_override(16);
        REQUIRE(buffer.size() == 16);
        REQUIRE(buffer.capacity() == 16);
        REQUIRE(buffer.is_dynamic());
    }

    SECTION("Resize")
    {
        memory_buffer<4> buffer{};

        // Zero state.
        REQUIRE(buffer.size() == 0);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        // Resize to 2 bytes.
        buffer.resize(2);
        buffer[0] = std::byte{0x01};
        buffer[1] = std::byte{0x02};

        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());
        REQUIRE(buffer[0] == std::byte{0x01});
        REQUIRE(buffer[1] == std::byte{0x02});

        // Resize beyond static capacity.
        buffer.resize(8);

        REQUIRE(buffer.size() == 8);
        REQUIRE(buffer.capacity() == 8);
        REQUIRE(buffer.is_dynamic());

        // Initialize rest of the buffer.
        buffer[2] = std::byte{0x03};
        buffer[3] = std::byte{0x04};
        buffer[4] = std::byte{0x05};
        buffer[5] = std::byte{0x06};
        buffer[6] = std::byte{0x07};
        buffer[7] = std::byte{0x08};

        // Check values.
        REQUIRE(buffer[0] == std::byte{0x01});
        REQUIRE(buffer[1] == std::byte{0x02});
        REQUIRE(buffer[2] == std::byte{0x03});
        REQUIRE(buffer[3] == std::byte{0x04});
        REQUIRE(buffer[4] == std::byte{0x05});
        REQUIRE(buffer[5] == std::byte{0x06});
        REQUIRE(buffer[6] == std::byte{0x07});
        REQUIRE(buffer[7] == std::byte{0x08});

        // Resize to 4 bytes.
        buffer.resize(4);

        REQUIRE(buffer.size() == 4);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        REQUIRE(buffer[0] == std::byte{0x01});
        REQUIRE(buffer[1] == std::byte{0x02});
        REQUIRE(buffer[2] == std::byte{0x03});
        REQUIRE(buffer[3] == std::byte{0x04});

        // Resize below static capacity.
        buffer.resize(2);
        REQUIRE(buffer.size() == 2);
        REQUIRE(buffer.capacity() == 4);
        REQUIRE_FALSE(buffer.is_dynamic());

        REQUIRE(buffer[0] == std::byte{0x01});
        REQUIRE(buffer[1] == std::byte{0x02});
    }
}
