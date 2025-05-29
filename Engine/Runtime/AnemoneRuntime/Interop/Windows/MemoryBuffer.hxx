#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/MemoryBuffer.hxx"

namespace Anemone::Interop::Windows
{
    template <size_t StaticCapacity, typename Callback>
    anemone_forceinline HRESULT AdaptMemoryBuffer(memory_buffer<StaticCapacity>& buffer, Callback&& callback)
    {
        Callback localCallback = std::forward<Callback>(callback);

        size_t requiredSize{};

        if (HRESULT const hr = localCallback(buffer.as_span(), requiredSize); FAILED(hr))
        {
            // Failed to adapt the buffer. Callback reported failure.
            buffer.clear();
            return hr;
        }

        if (requiredSize <= buffer.size())
        {
            // Callback reported success and the buffer is large enough to hold the required size.
            buffer.resize(requiredSize);
            return S_OK;
        }

        size_t bufferLength{};

        do
        {
            bufferLength = requiredSize;
            buffer.resize_for_override(bufferLength);

            if (HRESULT const hr = localCallback(buffer.as_span(), requiredSize); FAILED(hr))
            {
                // Failed to adapt the buffer. Callback reported failure.
                buffer.clear();
                return hr;
            }
        } while (bufferLength != requiredSize);

        buffer.resize(bufferLength);
        return S_OK;
    }
}
