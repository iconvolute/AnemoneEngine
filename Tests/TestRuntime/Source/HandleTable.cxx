#include "AnemoneRuntime/HandleTable.hxx"

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

    for (size_t i = 0; i < allocations; ++i)
    {
        auto handle = table.Allocate();
        REQUIRE(handle.has_value());
        table.Set(*handle, std::bit_cast<void*>(*handle));
        handles[i] = *handle;
    }

    for (size_t loop = 0; loop < loops; ++loop)
    {
        for (size_t i = 0; i < allocations; ++i)
        {
            auto handle = table.Allocate();
            REQUIRE(handle.has_value());
            table.Set(*handle, std::bit_cast<void*>(*handle));
            handles[allocations + i] = *handle;
        }

        std::shuffle(handles.begin(), handles.end(), random);

        for (size_t i = 0; i < allocations; ++i)
        {
            auto handle = handles[allocations + i];
            auto value = table.Get(handle);
            REQUIRE(value.has_value());
            REQUIRE(value.value() == std::bit_cast<void*>(handle));
            REQUIRE(table.Deallocate(handle));
        }
    }

    for (size_t i = 0; i < allocations; ++i)
    {
        auto handle = handles[i];
        auto value = table.Get(handle);
        REQUIRE(value.has_value());
        REQUIRE(value.value() == std::bit_cast<void*>(handle));
        REQUIRE(table.Deallocate(handle));
    }
}
