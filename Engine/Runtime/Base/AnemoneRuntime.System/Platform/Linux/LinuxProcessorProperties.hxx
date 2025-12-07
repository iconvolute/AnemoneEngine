#pragma once
#include "AnemoneRuntime.System/ProcessorProperties.hxx"
#include "AnemoneRuntime.Interop/StringBuffer.hxx"

namespace Anemone
{
    struct LinuxProcessorProperties
    {
        static void Initialize();
        static void Finalize();
        static LinuxProcessorProperties& Get();

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
