#pragma once

namespace Anemone::System
{
    struct WindowsSystemStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using SystemStatic = WindowsSystemStatic;
}
