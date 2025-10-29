#pragma once
#include "AnemoneSystem/ProcessorProperties.hxx"
#include "AnemoneInterop/StringBuffer.hxx"

namespace Anemone
{
    struct WindowsProcessorProperties
    {
        static void Initialize();
        static void Finalize();
        static WindowsProcessorProperties& Get();

        static void ValidateRequirements();

        bool featureSmt = false;

        size_t cacheL1 = 0;
        size_t cacheL2 = 0;
        size_t cacheL3 = 0;
        size_t cacheLineSize = 0;

        size_t logicalCores = 0;
        size_t physicalCores = 0;
        size_t performanceCores = 0;
        size_t efficiencyCores = 0;

        Interop::string_buffer<char, 64> processorName{};

        Interop::string_buffer<char, 64> processorVendor{};
    };
}
