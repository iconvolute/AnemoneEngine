#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

namespace Anemone::Interop::Windows
{
    template <typename StringBuffer, typename Callback>
    anemone_forceinline HRESULT AdaptStringBuffer(StringBuffer& buffer, Callback&& callback)
    {
        Callback localCallback = std::forward<Callback>(callback);

        size_t requiredSize{};

        if (HRESULT const hr = localCallback(buffer.as_buffer_span(), requiredSize); FAILED(hr))
        {
            buffer.trim(0);
            return hr;
        }

        if (requiredSize <= buffer.capacity())
        {
            assert(requiredSize != 0);
            buffer.trim(requiredSize);
            return S_OK;
        }

        size_t bufferLength{};

        do
        {
            bufferLength = requiredSize;
            buffer.resize_for_override(bufferLength);

            if (HRESULT const hr = localCallback(buffer.as_buffer_span(), requiredSize); FAILED(hr))
            {
                buffer.trim(0);
                return hr;
            }
        } while (requiredSize > bufferLength);

        buffer.trim(requiredSize);
        return S_OK;
    }
}
