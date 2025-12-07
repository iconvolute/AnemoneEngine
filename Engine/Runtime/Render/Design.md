# Design

## Global Resources

Renderer allows to declare global resources in source code. Once renderer backend is selected, an implementation specific resources are created so high-level API can access them.

High level API:

```c++
class SomeComputeShaderCS : public GlobalShader
{
    struct Input
    {
        // ...
    };

    struct Output
    {
        // ...
    };
};

AE_DECLARE_GLOBAL_SHADER(name);
AE_IMPLEMENT_GLOBAL_SHADER(name, sourcepath);
```

