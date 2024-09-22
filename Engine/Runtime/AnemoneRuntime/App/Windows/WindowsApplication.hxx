#pragma once
#include "AnemoneRuntime/App/Runtime.hxx"
#include "AnemoneRuntime/App/Application.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <array>

namespace Anemone::App
{
    struct WindowsAppState final
    {
        HINSTANCE Instance{};
        HICON Icon{};
        ATOM WindowClass{};

        std::array<HCURSOR, 10> Cursor{};

        void InitializeCursor(CursorType type, HCURSOR resource)
        {
            this->Cursor[static_cast<size_t>(type)] = resource;
        }

        HCURSOR GetCursor(CursorType type) const
        {
            return this->Cursor[static_cast<size_t>(type)];
        }
    };
}
