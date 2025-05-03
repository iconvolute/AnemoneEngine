# C++20 modules porting guide

## Overview

Key points:

* include header files (`.hxx`) are replaced with module interface files (`.ixx`)
* use module partitions to give name of each inner module interface
* module partitions form a hierarchy of imports, just like includes
* names of the partitions must be unique and should represent the logical location of the feature inside module
* no circular dependencies
* platform-specific source code is selected by build system
* names of the modules and module partitions may be different from the namespaces

## Example

### Project Structure

```
/engine
  /runtime
    /mod.ixx
    /base
      /mod.ixx
      /uuid.ixx
      /uuid.cxx
      /uninitialized.ixx
    /profiling
      /profiler.ixx
      /profiler.cxx
      /backends
        /mod.ixx
        /nvtx
            /mod.ixx
            /mod.cxx
        /intel
            /mod.ixx
            /mod.cxx
    /system
      /mod.ixx
      /environment.ixx
      /windows
        /mod.ixx
        /environment.ixx
        /environment.cxx
      /linux
        /mod.ixx
        /environment.ixx
        /environment.cxx
    /threading
      /mod.ixx
      /windows
        /mod.ixx
        /criticalsection.ixx
        /conditionvariable.ixx
        /thread.ixx
```

### Module Partitions

Partitions are used to split the module sources into multiple files. The `mod.ixx` file is used to group smaller partitions into larger ones.

Name of the partition represents logical location of the feature inside module. Partitions names are not visible to the user, but are crucial when importing parts of the module in other places.

The root `mod.ixx` file reexports all partitions as a single module.

**/engine/runtime/mod.ixx**

```cpp
export module runtime;

export import :base;
export import :profiling;
export import :threading;
```

**/engine/runtime/base/mod.ixx**

Each subdirectory has own `mod.ixx` file, which reexports all partitions in the subdirectory.

```cpp
export module runtime:base;

export import :base.uuid;
export import :base.uninitialized;
```

**/engine/runtime/base/uuid.ixx**

Interface files exporting C++ symbols replace include headers.

Non-exported symbols stay private to the module and are not visible outside.

```cpp
export module runtime:base.uuid;

import std;
import <cstdint>;

namespace runtime
{
    export struct RUNTIME_API uuid final
    {
        std::uint8_t data[16];

        static uuid create_from_string(std::string_view value);
    };
}
```

**/engine/runtime/system/environment.ixx**

Interface files can import other partitions, with required granularity.

Importing partition is similar to including header files, but instead of the filesystem paths we use module names known at compile time.

```cpp
export module runtime:system.environment;

import std;
import :base.uuid;

namespace runtime::environment
{
    export RUNTIME_API std::string_view get_computer_name();
    export RUNTIME_API uuid get_machine_uuid();
}
```

**/engine/runtime/system/windows/environment.ixx**

```cpp
export module runtime:system.environment.internal;

import std;
import :base;

namespace runtime::environment
{
    // Globals. Available to use in the module, not exported to the user.
    struct environment_statics
    {
        std::string _computer_name{};
        uuid _machine_uuid{};

        void initialize();
    };

    uninitialized_object<environment_statics> g_environment_statics{};
}
```

**/engine/runtime/system/windows/environment.cxx**

Implementation files (`.cxx`) are used just as before. They are not exported and are not visible outside the module.

Legacy includes must happen before initial `module;` and module declaration `module name-of-the-module;`.

```cpp
module;

// Place for all include headers
#define NOMINMAX
#include <Windows.h>

module runtime;

import std;
import :system.environment.internal;

namespace runtime::environment
{
    void environment_statics::initialize()
    {
        char name[256]{};
        DWORD size = std::size(name);

        if (GetComputerNameA(name, &size))
        {
            this->_computer_name = name;
        }

        this->_machine_uuid = uuid::create_from_string("12345678-1234-1234-1234-123456789012");
    }

    std::string_view get_computer_name()
    {
        return g_environment_statics->_computer_name;
    }

    uuid get_machine_uuid()
    {
        return g_environment_statics->_machine_uuid;
    }
}
```
