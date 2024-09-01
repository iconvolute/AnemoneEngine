#pragma once

namespace Anemone::System
{
    struct PosixSystemStatic
    {
        static void Initialize();
        static void Finalize();
    };

    using SystemStatic = PosixSystemStatic;
}
