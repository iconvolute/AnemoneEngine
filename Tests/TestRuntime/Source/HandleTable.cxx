#include "AnemoneRuntime.Base/HandleTable.hxx"

#include <catch_amalgamated.hpp>
#include <random>

TEST_CASE("Handle Table")
{
    constexpr size_t loops = 512;
    constexpr size_t allocations = 512;
    using namespace Anemone;

    std::vector<HandleTable::Handle> handles{};
    handles.resize(allocations * 2u);

    std::mt19937_64 random{2137};

    HandleTable table{};

    REQUIRE(table.Count() == 0);
    REQUIRE(table.Capacity() == 0);

    for (size_t i = 0; i < allocations; ++i)
    {
        auto handle = table.Allocate();
        REQUIRE(handle.has_value());
        table.Set(*handle, std::bit_cast<void*>(*handle));
        handles[i] = *handle;
    }

    REQUIRE(table.Capacity() == allocations);

    for (size_t loop = 0; loop < loops; ++loop)
    {
        REQUIRE(table.Count() == allocations);

        for (size_t i = 0; i < allocations; ++i)
        {
            auto handle = table.Allocate();
            REQUIRE(handle.has_value());
            table.Set(*handle, std::bit_cast<void*>(*handle));
            handles[allocations + i] = *handle;
        }

        REQUIRE(table.Count() == allocations * 2);

        std::shuffle(handles.begin(), handles.end(), random);

        for (size_t i = 0; i < allocations; ++i)
        {
            auto handle = handles[allocations + i];
            auto value = table.Get(handle);
            REQUIRE(value.has_value());
            REQUIRE(value.value() == std::bit_cast<void*>(handle));
            REQUIRE(table.Deallocate(handle));
        }

        REQUIRE(table.Count() == allocations);

        REQUIRE(table.Capacity() == allocations * 2);
    }

    for (size_t i = 0; i < allocations; ++i)
    {
        auto handle = handles[i];
        auto value = table.Get(handle);
        REQUIRE(value.has_value());
        REQUIRE(value.value() == std::bit_cast<void*>(handle));
        REQUIRE(table.Deallocate(handle));
    }

    REQUIRE(table.Count() == 0);

    REQUIRE(table.Capacity() == allocations * 2);
}
