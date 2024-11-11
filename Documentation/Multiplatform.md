# Multiplatform

This engine uses explicit singleton pattern with subclassing.

## Goals

Presented approach allows to have platform-specific implementation parts of common systems.

Usages:
- platform-specific parts implemented separately
    - accessing stack trace
- common functionality with different initialization
    - logger, debugging, task system
- stateful systems

This approach allows to expose public API of system. Platform specific parts are available via specific types / include headers.

## Example

This example shows how to implement such platform specific system.

### Public API

```c++
// FoobarSystem.hxx
class FoobarSystem {
public:
    // Implementation detail:
    // - could be implemented as-is, by using only members of this class
    // - accessing platform-specific things could be done in two different ways:
    //   - access GFoobarSystem directly
    //   - implement as virtual call
    //   - implement in platform specific file
    void SomeApi();

public:
    static void Initialize();
    static void Finalize();
    static FoobarSystem& Get();
};

// FoobarSystem.cxx
#include "FoobarSystem.hxx"

#if WINDOWS
#include "Windows/FoobarSystem.hxx"
#elif LINUX
#include "Linux/FoobarSystem.hxx"
#else
#error Not implemented
#endif

void FoobarSystem::Initialize() {
    GFoobarSystem.Create();
}

void FoobarSystem::Finalize() {
    GFoobarSystem.Destroy();
}

FoobarSystem& FoobarSystem::Get() {
    return GFoobarSystem.Get();
}
```

### Implementation

```c++
// WindowsFoobarSystem.hxx
class WindowsFoobarSystem : public FoobarSystem {
    // platform specific
};

extern UninitializedObject<WindowsFoobarSystem> GFoobarSystem;

// WindowsFoobarSystem.cxx
UninitializedObject<WindowsFoobarSystem> GFoobarSystem;
```
