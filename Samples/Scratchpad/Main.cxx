#include "AnemoneRuntime/Runtime/EntryPoint.hxx"
#include "AnemoneRuntime/System/Environment.hxx"


#include <string>
#include <string_view>

#include "AnemoneRuntime/Diagnostics/Error.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <expected>

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Interop/Windows/Dwm.hxx"
#include "AnemoneRuntime/Interop/Windows/Registry.hxx"
#endif
#include "AnemoneRuntime/Network/Tcp.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"

#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/System/ProcessorProperties.hxx"

namespace anemone
{
    template <typename T>
    struct bit_pack;

    template <>
    struct bit_pack<uint64_t> final
    {
        constexpr uint64_t operator()(uint32_t v1, uint32_t v0) const noexcept
        {
            return static_cast<uint64_t>(v1) << 32u | static_cast<uint64_t>(v0);
        }

        constexpr uint64_t operator()(uint16_t v3, uint16_t v2, uint16_t v1, uint16_t v0) const noexcept
        {
            return static_cast<uint64_t>(v3) << 48u | static_cast<uint64_t>(v2) << 32u | static_cast<uint64_t>(v1) << 16u | static_cast<uint64_t>(v0);
        }
    };

    template <>
    struct bit_pack<uint32_t> final
    {
        constexpr uint32_t operator()(uint16_t v1, uint16_t v0) const noexcept
        {
            return static_cast<uint32_t>(v1) << 16u | static_cast<uint32_t>(v0);
        }

        constexpr uint32_t operator()(uint8_t v3, uint8_t v2, uint8_t v1, uint8_t v0) const noexcept
        {
            return static_cast<uint32_t>(v3) << 24u | static_cast<uint32_t>(v2) << 16u | static_cast<uint32_t>(v1) << 8u | static_cast<uint32_t>(v0);
        }
    };

    template <>
    struct bit_pack<uint16_t> final
    {
        constexpr uint16_t operator()(uint8_t v1, uint8_t v0) const noexcept
        {
            return static_cast<uint16_t>(v1) << 8u | static_cast<uint16_t>(v0);
        }
    };

    template <typename T>
    inline constexpr auto BitPack = bit_pack<T>{};
}

#include "AnemoneRuntime/Base/Uuid.hxx"
/*
namespace Anemone::inline FileSystemX
{
    struct Base64
    {
        static constexpr char Charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                          "abcdefghijklmnopqrstuvwxyz"
                                          "0123456789+/";


        static std::expected<size_t, Error> GetEncodedSize(size_t size);
        static std::expected<size_t, Error> GetDecodedSize(std::string_view encoded);

        static std::expected<size_t, Error> Encode(std::span<std::byte const> input, std::span<char> output);
        static std::expected<size_t, Error> Decode(std::string_view encoded, std::span<std::byte> data);
    };
}*/

#include "AnemoneApplication/HostApplication.hxx"
#include "AnemoneApplication/HostApplicationEvents.hxx"
#include "AnemoneApplication/HostDialogs.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"

#include "AnemoneRuntime/System/CommandLine.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

class EH final : public Anemone::HostApplicationEvents
{
public:
    EH() = default;
    ~EH() override = default;

    void OnMouseEnter(Anemone::HostWindow& window, Anemone::MouseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseLeave(Anemone::HostWindow& window, Anemone::MouseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }
    void OnMouseMove(Anemone::HostWindow& window, Anemone::MouseMoveEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseWheel(Anemone::HostWindow& window, Anemone::MouseWheelEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseButtonDown(Anemone::HostWindow& window, Anemone::MouseButtonEventArgs& args) override
    {
        AE_TRACE(Error, "mouse button down: {}", std::to_underlying(args.Key));
        (void)window;
        (void)args;

        std::string ret;
        if (Anemone::HostDialogs::OpenFile(&window, ret, "Select file",
                std::array{
                    Anemone::FileDialogFilter{"Any", "*.*"},
                }) == Anemone::DialogResult::Ok)
        {
            AE_TRACE(Error, "Selected file: '{}'", ret);

            Anemone::CriticalSection cs{};

            {
                Anemone::UniqueLock scope{cs};

                if (auto f = Anemone::FileSystem::GetPlatformFileSystem().GetPathInfo(ret))
                {
                    AE_TRACE(Error, "---------------");
                    AE_TRACE(Error, "File-Size:     '{}'", f->Size);
                    AE_TRACE(Error, "File-Created:  '{}'", f->Created);
                    AE_TRACE(Error, "File-Modified: '{}'", f->Modified);
                    AE_TRACE(Error, "File-ReadOnly: '{}'", f->ReadOnly);
                    AE_TRACE(Error, "File-Type:     '{}'", std::to_underlying(f->Type));
                    AE_TRACE(Error, "---------------");
                }

                if (auto r = Anemone::Clipboard::SetText(ret))
                {
                    AE_TRACE(Error, "Set clipboard text");
                }
                else
                {
                    AE_TRACE(Error, "Failed to set clipboard text: {}", std::to_underlying(r.error()));
                }
            }
        }
    }

    void OnMouseButtonUp(Anemone::HostWindow& window, Anemone::MouseButtonEventArgs& args) override
    {
        AE_TRACE(Error, "mouse button up: {}", std::to_underlying(args.Key));
        (void)window;
        (void)args;
    }

    void OnMouseButtonClick(Anemone::HostWindow& window, Anemone::MouseButtonEventArgs& args) override
    {
        (void)window;
        (void)args;

        *(volatile void**)0x42069 = nullptr;
    }

    void OnKeyDown(Anemone::HostWindow& window, Anemone::KeyEventArgs& args) override
    {
        AE_TRACE(Error, "Key down: {}", std::to_underlying(args.Key));
        (void)window;
        (void)args;

        if (args.Key == Anemone::VirtualKey::F1)
        {
            window.SetMode((window.GetMode() == Anemone::WindowMode::Windowed)
                    ? Anemone::WindowMode::Borderless
                    : Anemone::WindowMode::Windowed);
        }
        else if (args.Key == Anemone::VirtualKey::F2)
        {
            window.SetBounds(Anemone::Math::RectF{0, 0, 1000, 1000});
        }
        else if (args.Key == Anemone::VirtualKey::F3)
        {
            window.SetBounds(Anemone::Math::RectF{1000, 1000, 1000, 1000});
        }
        else
        {
            AE_ASSERT(false);
        }
    }

    void OnKeyUp(Anemone::HostWindow& window, Anemone::KeyEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnCharacterReceived(Anemone::HostWindow& window, Anemone::CharacterReceivedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnGamepadAnalog(Anemone::InputDeviceId device, Anemone::GamepadAnalogEventArgs& args) override
    {
        (void)device;
        (void)args;
    }

    void OnGamepadButtonDown(Anemone::InputDeviceId device, Anemone::GamepadButtonEventArgs& args) override
    {
        (void)device;
        (void)args;
    }

    void OnGamepadButtonUp(Anemone::InputDeviceId device, Anemone::GamepadButtonEventArgs& args) override
    {
        (void)device;
        (void)args;
    }

    void OnWindowClose(Anemone::HostWindow& window, Anemone::WindowCloseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowActivated(Anemone::HostWindow& window, Anemone::WindowActivatedEventArgs& args) override
    {
        window.SetTitle(fmt::format("Window: {}, Active: {}", fmt::ptr(&window), window.IsActive()));
        (void)window;
        (void)args;
    }

    void OnWindowSizeChanged(Anemone::HostWindow& window, Anemone::WindowSizeChangedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowLocationChanged(Anemone::HostWindow& window, Anemone::WindowLocationChangedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowResizeStarted(Anemone::HostWindow& window, Anemone::WindowEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowResizeCompleted(Anemone::HostWindow& window, Anemone::WindowEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowDpiChanged(Anemone::HostWindow& window, Anemone::WindowDpiChangedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnEndSession(Anemone::EndSessionEventArgs& args) override
    {
        (void)args;
    }

    void OnSystemSuspending() override
    {
    }

    void OnSystemResuming() override
    {
    }

    void OnDisplayChange() override
    {
    }

    void OnWindowVisibilityChanged(Anemone::HostWindow& window, Anemone::WindowVisibilityChangedEventArgs& args) override
    {
        AE_TRACE(Error, "Window: {} is visible: {}", fmt::ptr(&window), args.Visible);
    }
};

#if ANEMONE_PLATFORM_WINDOWS
#pragma comment(lib, "Dwmapi.lib")
#endif

#include "AnemoneRuntime/System/Environment.hxx"

anemone_noinline void test()
{
    AE_TRACE(Error, "cpu-Name:             '{}'", Anemone::ProcessorProperties::GetProcessorName());
    AE_TRACE(Error, "cpu-Vendor:           '{}'", Anemone::ProcessorProperties::GetProcessorVendor());
}

#include "AnemoneRuntime/Hash/FNV.hxx"
#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneTasks/TaskScheduler.hxx"
#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneRuntime/Interop/Windows/Environment.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#endif

struct V2 : Anemone::FileSystemVisitor
{
    void Visit(std::string_view path, std::string_view name, Anemone::FileInfo const& info) override
    {
        AE_TRACE(Error, "Path: '{}'", path);
        AE_TRACE(Error, "Name: '{}'", name);
        AE_TRACE(Error, "Size: '{}'", info.Size);
        AE_TRACE(Error, "Type: '{}'", std::to_underlying(info.Type));
        AE_TRACE(Error, "Created: '{}'", info.Created);
        AE_TRACE(Error, "Modified: '{}'", info.Modified);
        AE_TRACE(Error, "ReadOnly: '{}'", info.ReadOnly);
    }
};

#include "AnemoneTasks/Parallel.hxx"

//
// Engine : IApplicationEvents
// GameEngine : Engine
// EditorEngine : Engine
//

#include "AnemoneRuntime/Math/PidController.hxx"
#include "AnemoneRuntime/Math/Random.hxx"

namespace Anemone::Builtins
{
    constexpr uint32_t UInt32_MaskTrue = 0xFFFFFFFFu;
    constexpr uint32_t UInt32_MaskFalse = 0x00000000u;

    constexpr uint32_t UInt32_ToBitwiseMask(bool value)
    {
        return static_cast<uint32_t>(-static_cast<int32_t>(value));
    }

    constexpr uint32_t UInt32_CompareEquals(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a == b);
    }

    constexpr uint32_t UInt32_CompareNotEquals(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a != b);
    }

    constexpr uint32_t UInt32_CompareLess(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a < b);
    }

    constexpr uint32_t UInt32_CompareLessEqual(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a <= b);
    }

    constexpr uint32_t UInt32_CompareGreater(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a > b);
    }

    constexpr uint32_t UInt32_CompareGreaterEqual(uint32_t a, uint32_t b)
    {
        return UInt32_ToBitwiseMask(a >= b);
    }

    constexpr uint32_t UInt32_ExtractSignMask(uint32_t value)
    {
        return static_cast<uint32_t>(static_cast<int32_t>(value) >> 31);
    }

    constexpr uint32_t UInt32_BitwiseAnd(uint32_t a, uint32_t b)
    {
        return a & b;
    }

    constexpr uint32_t UInt32_BitwiseAndNot(uint32_t a, uint32_t b)
    {
        return a & ~b;
    }

    constexpr uint32_t UInt32_BitwiseOr(uint32_t a, uint32_t b)
    {
        return a | b;
    }

    constexpr uint32_t UInt32_BitwiseXor(uint32_t a, uint32_t b)
    {
        return a ^ b;
    }

    constexpr uint32_t UInt32_BitwiseNot(uint32_t a)
    {
        return ~a;
    }

    constexpr uint32_t UInt32_BitwiseSelect(uint32_t mask, uint32_t whenFalse, uint32_t whenTrue)
    {
        return (mask & whenTrue) | (~mask & whenFalse);
    }

    constexpr uint32_t UInt32_Min(uint32_t a, uint32_t b)
    {
        return (a < b) ? a : b;
    }

    constexpr uint32_t UInt32_Max(uint32_t a, uint32_t b)
    {
        return (a > b) ? a : b;
    }

    constexpr uint32_t UInt32_ShiftLeft(uint32_t value, int shift)
    {
        return value << shift;
    }

    constexpr uint32_t UInt32_ShiftRight(uint32_t value, int shift)
    {
        return value >> shift;
    }

    constexpr uint32_t UInt32_RotateLeft(uint32_t value, int shift)
    {
        return (value << shift) | (value >> (32 - shift));
    }

    constexpr uint32_t UInt32_RotateRight(uint32_t value, int shift)
    {
        return (value >> shift) | (value << (32 - shift));
    }

    constexpr uint32_t UInt32_Add(uint32_t a, uint32_t b)
    {
        return a + b;
    }

    constexpr uint32_t UInt32_Subtract(uint32_t a, uint32_t b)
    {
        return a - b;
    }

    constexpr uint32_t UInt32_Multiply(uint32_t a, uint32_t b)
    {
        return a * b;
    }

    constexpr uint32_t UInt32_Divide(uint32_t a, uint32_t b)
    {
        return a / b;
    }

    constexpr uint32_t UInt32_Modulus(uint32_t a, uint32_t b)
    {
        return a % b;
    }

    constexpr uint32_t UInt32_Increment(uint32_t x)
    {
        return x + 1u;
    }

    constexpr uint32_t UInt32_Decrement(uint32_t x)
    {
        return x - 1u;
    }

    constexpr uint32_t UInt32_Negate(uint32_t x)
    {
        return ~x + 1u; // Two's complement negation
    }

    // constexpr bool UInt32_HasSingleBit(uint32_t x)
    //{
    //     // A number has a single bit set if it is not zero and its bitwise AND with its negation is zero.
    //     return (x != 0) && ((x & -x) == x);
    // }

    constexpr uint32_t UInt32_CountBits(uint32_t x)
    {
        x = (x & uint32_t{0x55555555u}) + ((x & uint32_t{0xAAAAAAAAu}) >> 1);
        x = (x & uint32_t{0x33333333u}) + ((x & uint32_t{0xCCCCCCCCu}) >> 2);
        x = (x & uint32_t{0x0F0F0F0Fu}) + ((x & uint32_t{0xF0F0F0F0u}) >> 4);
        x = (x & uint32_t{0x00FF00FFu}) + ((x & uint32_t{0xFF00FF00u}) >> 8);
        x = (x & uint32_t{0x0000FFFFu}) + ((x & uint32_t{0xFFFF0000u}) >> 16);
        return x;
    }

    constexpr uint32_t UInt32_ReverseBits(uint32_t x)
    {
        x = ((x >> 1) & uint32_t{0x55555555}) | ((x << 1) & uint32_t{0xAAAAAAAA});
        x = ((x >> 2) & uint32_t{0x33333333}) | ((x << 2) & uint32_t{0xCCCCCCCC});
        x = ((x >> 4) & uint32_t{0x0F0F0F0F}) | ((x << 4) & uint32_t{0xF0F0F0F0});
        x = ((x >> 8) & uint32_t{0x00FF00FF}) | ((x << 8) & uint32_t{0xFF00FF00});
        x = ((x >> 16) & uint32_t{0x0000FFFF}) | ((x << 16) & uint32_t{0xFFFF0000});
        return x;
    }

    constexpr uint32_t UInt32_NextPowerOf2(uint32_t value)
    {
        value |= (value >> 1);
        value |= (value >> 2);
        value |= (value >> 4);
        value |= (value >> 8);
        value |= (value >> 16);
        return value + 1;
    }

    constexpr uint64_t UInt64_NextPowerOf2(uint64_t value)
    {
        value |= (value >> 1);
        value |= (value >> 2);
        value |= (value >> 4);
        value |= (value >> 8);
        value |= (value >> 16);
        value |= (value >> 32);
        return value + 1;
    }
}

anemone_noinline double gen(Anemone::Math::Xorshiro256ss& generator)
{
    return Anemone::Math::UniformDistribution<double>{}(generator);
}

anemone_noinline void test2()
{
    if (auto r = Anemone::Clipboard::SetText("testing2123"))
    {
        AE_TRACE(Error, "Set clipboard text");
    }
    else
    {
        AE_TRACE(Error, "Failed to set clipboard text: {}", std::to_underlying(r.error()));
    }

    std::string result{};
    if (auto r = Anemone::Clipboard::GetText(result))
    {
        AE_TRACE(Error, "Set clipboard text");
    }
    else
    {
        AE_TRACE(Error, "Failed to set clipboard text: {}", std::to_underlying(r.error()));
    }
}

anemone_noinline Anemone::Math::Detail::SimdMask4F foo(Anemone::Math::Detail::SimdVector4F v)
{
    return Anemone::Math::Detail::Vector4F_CompareNaN(v);
}

#include "AnemoneRuntime/Base/AdaptiveTimer.hxx"

anemone_noinline void test3()
{
    Anemone::AdaptiveTimer timer{};

    for (size_t i = 0; i < 1000; ++i)
    {
        timer.Tick([&](Anemone::Duration const& duration)
        {
            Anemone::CurrentThread::YieldAnyThreadOnSameProcessor();
            (void)duration;
            // Anemone::CurrentThread::Sleep(50);
            // AE_TRACE(Error, "Tick: {}, DT: {}, FC: {}, FPS: {}", i, duration.ToMicroseconds(), timer.GetFramesCount(), timer.GetFramesPerSecond());
        });
    }
    AE_TRACE(Error, "Frames:            '{}'", timer.GetFramesCount());
    AE_TRACE(Error, "Frames-Per-Second: '{}'", timer.GetFramesPerSecond());
}

#include "AnemoneRuntime/System/SharedLibrary.hxx"

#include "AnemoneApplication/Module.hxx"
#include "AnemoneMemory/Module.hxx"


#if __has_include("AnemoneRenderVulkan/VulkanDevice.hxx")
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#endif

#include "AnemoneRuntime/Diagnostics/Internal/FileTraceListener.hxx"
#include "AnemoneTasks/Module.hxx"

anemone_noinline int AnemoneMain(int argc, char** argv)
{
    Anemone::ModuleInitializer<Anemone::Module_Application> moduleApplication{};
    Anemone::ModuleInitializer<Anemone::Module_Memory> moduleMemory{};
    Anemone::ModuleInitializer<Anemone::Module_Tasks> moduleTasks{};
    Anemone::UninitializedObject<Anemone::FileTraceListener> fts{};

    Anemone::Parallel::For(10000, 128, [](size_t first, size_t count)
    {
        AE_TRACE(Error, "First: {:016x}, Count: {:016x}", first, count);
    });

    if (auto fh = Anemone::FileSystem::GetPlatformFileSystem().CreateFileWriter("log.txt"))
    {
        fts.Create(Anemone::MakeReference<Anemone::FileOutputStream>(*fh));
        Anemone::Trace::Get().Register(*fts);
    }

    {
        EH eh{};
        Anemone::HostApplication::Initialize(eh);
#if __has_include("AnemoneRenderVulkan/VulkanDevice.hxx")
        auto rd = Anemone::CreateRenderDevice();
        (void)rd;
#endif
        Anemone::HostApplication::Finalize();
        
    }
    if (fts)
    {
        Anemone::Trace::Get().Unregister(*fts);
        fts.Destroy();
    }

    if (auto sh = Anemone::SharedLibrary::Load("TestLibrary.dll"))
    {
        AE_TRACE(Error, "Works");
    }
    else
    {
        AE_TRACE(Error, "Failed with {}", std::to_underlying(sh.error()));
    }
    auto& fs = Anemone::FileSystem::GetPlatformFileSystem();

    auto path = "temp/a/b/c/d/e/f";
    if (fs.DirectoryCreateRecursive(path))
    {
        if (fs.DirectoryDeleteRecursive(path))
        {
            AE_TRACE(Error, "++");
        }
        else
        {
            AE_TRACE(Error, "+-");
        }

        path = "temp/a/b";

        if (fs.DirectoryDeleteRecursive(path))
        {
            AE_TRACE(Error, "++");
        }
        else
        {
            AE_TRACE(Error, "+-");
        }
    }
    else
    {
        AE_TRACE(Error, "--");
    }

    if (auto r = fs.FileCopy("d:/test.txt", "d:/test2.txt", Anemone::NameCollisionResolve::Fail))
    {
        AE_TRACE(Error, "1: Copy file succeeded");
    }
    else
    {
        AE_TRACE(Error, "1: Copy file failed: {}", r.error());
    }

    if (auto r = fs.FileCopy("d:/test.txt", "d:/test2.txt", Anemone::NameCollisionResolve::Fail))
    {
        AE_TRACE(Error, "1: Copy file succeeded");
    }
    else
    {
        AE_TRACE(Error, "1: Copy file failed: {}", r.error());
    }

    if (auto r = fs.FileCopy("d:/test.txt", "d:/test2.txt", Anemone::NameCollisionResolve::Overwrite))
    {
        AE_TRACE(Error, "1: Copy file succeeded");
    }
    else
    {
        AE_TRACE(Error, "1: Copy file failed: {}", r.error());
    }

    // test3();
    // if (argc)
    //{
    //     return -1;
    // }
    // AE_TRACE(Error, "{}", Anemone::Math::Detail::Vector4F_Extract<0>(foo(*reinterpret_cast<Anemone::Math::Detail::SimdVector4F*>(argv))));
    test2();

#if false
    (void)Anemone::Process::Start("DevDebugger.exe", fmt::format("--pid {}", GetCurrentProcessId()), {});
#endif
    {
        AE_TRACE(Error, "{}", Anemone::Environment::GetDeviceUniqueId());
        AE_TRACE(Error, "Default: '{}'", Anemone::Environment::GetSystemId());
        AE_TRACE(Error, "Braces:  '{:b}'", Anemone::Environment::GetSystemId());
        AE_TRACE(Error, "Dashes:  '{:d}'", Anemone::Environment::GetSystemId());
        AE_TRACE(Error, "Raw:     '{:r}'", Anemone::Environment::GetSystemId());
        AE_TRACE(Error, "Full:    '{:f}'", Anemone::Environment::GetSystemId());
    }
    {
        Anemone::Parallel::For(1024 + 2, 6, [](size_t index, size_t count)
        {
            AE_TRACE(Error, "work: tid: {:x}, index: {}, count: {}", Anemone::CurrentThread::Id(), index, count);
        }, [](size_t count)
        {
            AE_TRACE(Error, "finalize: tid: {:x}, count: {}", Anemone::CurrentThread::Id(), count);
        });
    }
    {
        struct Vi : Anemone::FileSystemVisitor
        {
            void Visit(std::string_view path, std::string_view name, Anemone::FileInfo const& info) override
            {
                AE_TRACE(Error, "Path: '{}'", path);
                AE_TRACE(Error, "Name: '{}'", name);
                AE_TRACE(Error, "Size: '{}'", info.Size);
                AE_TRACE(Error, "Type: '{}'", std::to_underlying(info.Type));
                AE_TRACE(Error, "Created: '{}'", info.Created);
                AE_TRACE(Error, "Modified: '{}'", info.Modified);
                AE_TRACE(Error, "ReadOnly: '{}'", info.ReadOnly);
            }
        };

        Vi vi{};
        (void)Anemone::FileSystem::GetPlatformFileSystem().DirectoryEnumerate(Anemone::Environment::GetStartupPath(), vi);
    }

    Anemone::CurrentThread::YieldAnyThreadOnAnyProcessor();

    test();
    (void)argc;
    (void)argv;

    Anemone::FNV1A128 original{};

    if (auto h = Anemone::FileSystem::GetPlatformFileSystem().CreateFileWriter("test.txt"))
    {
        std::string_view s = "Hello World!\n";

        auto sb = std::as_bytes(std::span{s.data(), s.size()});

        for (size_t i = 0; i < 63; ++i)
        {
            original.Update(sb);
            (void)(*h)->Write(sb);
        }

        (void)(*h)->Flush();
    }

    Anemone::FNV1A128 current{};

    if (auto h = Anemone::FileSystem::GetPlatformFileSystem().CreateFileReader("test.txt"))
    {
        std::vector<std::byte> buffer(32);
        auto sb = std::as_writable_bytes(std::span{buffer});

        while (auto r = (*h)->Read(sb))
        {
            if (*r == 0)
            {
                break;
            }
            current.Update(sb.subspan(0, *r));
        }
    }

    auto hashOriginal = original.Finalize();
    auto hashCurrent = current.Finalize();

    AE_TRACE(Error, "hashes: [0]: {:016x} == {:016x} : {}", hashOriginal[0], hashCurrent[0], static_cast<int64_t>(hashOriginal[0]) - static_cast<int64_t>(hashCurrent[0]));
    AE_TRACE(Error, "hashes: [1]: {:016x} == {:016x} : {}", hashOriginal[1], hashCurrent[1], static_cast<int64_t>(hashOriginal[1]) - static_cast<int64_t>(hashCurrent[1]));

    if (auto content = Anemone::FileSystem::GetPlatformFileSystem().ReadTextFile("test.txt"))
    {
        Anemone::FNV1A128 hash{};

        hash.Update(*content);
        auto r = hash.Finalize();

        AE_TRACE(Error, "hashes: [0]: {:016x} == {:016x} : {}", hashOriginal[0], r[0], static_cast<int64_t>(hashOriginal[0]) - static_cast<int64_t>(r[0]));
    }
    else
    {
    }

    if (std::vector<std::string_view> args; Anemone::CommandLine::GetPositional(args), true)
    {
        AE_TRACE(Error, "Positional arguments:");

        for (auto const& arg : args)
        {
            AE_TRACE(Error, "  '{}'", arg);
        }

        if (auto const value = Anemone::CommandLine::GetOption("a"); value.has_value())
        {
            AE_TRACE(Error, "Named argument 'a': '{}'", *value);
        }
        else
        {
            AE_TRACE(Error, "Named argument 'a' not found");
        }

        std::vector<std::string_view> values{};
        Anemone::CommandLine::GetOption("b", values);

        for (auto value : values)
        {
            AE_TRACE(Error, "Named argument 'b-list': '{}'", value);
        }

        if (auto const value = Anemone::CommandLine::GetOption("b"); value.has_value())
        {
            AE_TRACE(Error, "Named argument 'b': '{}'", *value);
        }
        else
        {
            AE_TRACE(Error, "Named argument 'b' not found");
        }

        if (auto const value = Anemone::CommandLine::GetOption("c"); value.has_value())
        {
            AE_TRACE(Error, "Named argument 'c': '{}'", *value);
        }
        else
        {
            AE_TRACE(Error, "Named argument 'c' not found");
        }
    }
    test();
    AE_TRACE(Error, "Hello, World!");
    AE_TRACE(Error, "cpu-Name:             '{}'", Anemone::ProcessorProperties::GetProcessorName());
    AE_TRACE(Error, "cpu-Vendor:           '{}'", Anemone::ProcessorProperties::GetProcessorVendor());
    AE_TRACE(Error, "cpu-EfficiencyCores:  '{}'", Anemone::ProcessorProperties::GetEfficiencyCoresCount());
    AE_TRACE(Error, "cpu-PerformanceCores: '{}'", Anemone::ProcessorProperties::GetPerformanceCoresCount());
    AE_TRACE(Error, "cpu-PhysicalCores:    '{}'", Anemone::ProcessorProperties::GetPhysicalCoresCount());
    AE_TRACE(Error, "cpu-LogicalCores:     '{}'", Anemone::ProcessorProperties::GetLogicalCoresCount());

    fmt::println("cpu-name:    '{}'", Anemone::ProcessorProperties::GetProcessorName());
    fmt::println("cpu-vendor:  '{}'", Anemone::ProcessorProperties::GetProcessorVendor());
    /*fmt::println("device-name: '{}'", Anemone::Environment::GetDeviceName());
    fmt::println("device-id:   '{}'", Anemone::Environment::GetDeviceUniqueId());*/

    EH eh{};

#if ANEMONE_PLATFORM_WINDOWS
    AE_TRACE(Error, "Error: {}", Anemone::WindowsDebug::TranslateErrorCodeHRESULT(E_OUTOFMEMORY));
#endif

    Anemone::HostApplication::Initialize(eh);
    auto window1 = Anemone::HostApplication::Get().MakeWindow(Anemone::WindowType::Game, Anemone::WindowMode::Windowed);
    auto window2 = Anemone::HostApplication::Get().MakeWindow(Anemone::WindowType::Game, Anemone::WindowMode::Windowed);

    if (window1 and window2)
    {
        window1->SetMinimumSize(Anemone::Math::SizeF{640.0f, 480.0f});
        window1->SetInputEnabled(true);

        window2->SetMinimumSize(Anemone::Math::SizeF{640.0f, 480.0f});
        window2->SetInputEnabled(true);

        while (not window1->IsClosed() or not window2->IsClosed())
        {
            Anemone::HostApplication::Get().ProcessMessages();
            Anemone::CurrentThread::Sleep(Anemone::Duration::FromMilliseconds(16));
        }
    }

#if false
    Execute([&]
    {
        AE_PANIC("Hello {}", 42);
    });

    (void)argc;
    (void)argv;
    AE_PANIC("Hello {}", 42);
    Anemone::Platform::Internal::Initialize(argc, argv);
    Anemone::GTrace.Create();
    Anemone::Platform::Internal::SetupTraceListeners();
    Anemone::GTrace->AddListener(sl);
    Anemone::GTrace->AddListener(dl);

    AE_TRACE(Verbose, "Verbose");
    AE_TRACE(Debug, "Debug");
    AE_TRACE(Information, "Information");
    AE_TRACE(Warning, "Warning");
    AE_TRACE(Error, "Error");
    AE_TRACE(Critical, "Critical");

    AE_ASSERT(false);

    _mm_pause();
    Anemone::GTrace->WriteLine(Anemone::TraceLevel::Information, "'Hello {} {}'", 42, "World");
    _mm_pause();

    _mm_pause();


    fmt::println("os1: {}", Anemone::Platform::GetSystemId());
    fmt::println("os2: {}", Anemone::Platform::GetSystemVersion());
    Anemone::Platform::WalkStackTrace([](void* ptr, std::string_view name)
    {
        fmt::println(" at {} {}", ptr, name);
    });

    std::array<std::byte, 32> random{};
    Anemone::Platform::GetRandom(random);
    for (auto b : random)
    {
        fmt::println("Random byte: {}", static_cast<uint8_t>(b));
    }

    EH eh{};
    Anemone::Platform::SetPlatformEvents(&eh);
#if ANEMONE_PLATFORM_WINDOWS
    if (Anemone::Interop::win32_registry_key key{
            HKEY_LOCAL_MACHINE,
            LR"(SOFTWARE\Microsoft\Windows NT\CurrentVersion)",
            KEY_READ,
        })
    {
        Anemone::Interop::win32_string_buffer<wchar_t, 512> buffer{};
        if (key.read_string(L"ProductName", buffer))
        {
            std::string value{};
            Anemone::Interop::win32_NarrowString(value, buffer.as_view());
            fmt::println("ProductName: {}", value);
        }
    }
#endif

    Anemone::SplashScreen::Show();

#if ANEMONE_PLATFORM_WINDOWS
    SleepEx(3'000, FALSE);
#endif

    Anemone::SplashScreen::Hide();

    auto window = Anemone::Platform::Window::Create(Anemone::Platform::WindowType::Game);
    if (window)
    {
        window->SetMinimumSize(Anemone::Math::SizeF{640.0f, 480.0f});
        window->SetInputEnabled(true);

        while (window->IsVisible())
        {
            Anemone::Platform::ProcessMessages();
        }
    }

#if ANEMONE_PLATFORM_WINDOWS
    DWORD qq{};
    Anemone::Interop::win32_QueryRegistry(
        qq,
        HKEY_LOCAL_MACHINE,
        LR"(SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion)",
        L"CurrentMajorVersionNumber");
    Anemone::Interop::win32_QueryRegistry(
        qq,
        HKEY_LOCAL_MACHINE,
        LR"(SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion)",
        L"CurrentMinorVersionNumber");
    Anemone::Interop::win32_string_buffer<wchar_t, 128> winver{};
    Anemone::Interop::win32_QueryRegistry(
        winver,
        HKEY_LOCAL_MACHINE,
        LR"(SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion)",
        L"CurrentBuildNumber");
#endif

    Anemone::GTrace->RemoveListener(dl);
    Anemone::GTrace->RemoveListener(sl);
    Anemone::GTrace.Destroy();
    Anemone::Platform::Internal::Finalize();
#endif
    return 0;
}

#if false
#include "AnemoneRuntime/Instant.hxx"
#include "AnemoneRuntime/Profiler/Profiler.hxx"
#include "AnemoneRuntime/Threading/Yielding.hxx"
#include "AnemoneRuntime/Runtime.hxx"
#include "AnemoneRuntime/Diagnostic/StackTrace.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

#include <fmt/format.h>
#include <atomic>

#include "AnemoneRuntime/Bitwise.hxx"
#include "AnemoneRuntime/Error.hxx"
#include "AnemoneRuntime/Unicode.hxx"
#include "AnemoneRuntime/Hash/FNV.hxx"

AE_DECLARE_PROFILE(Main);
AE_DECLARE_PROFILE(Outer);
AE_DECLARE_PROFILE(Inner);

#include "AnemoneRuntime/Math/Detail/SimdFloat.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include "AnemoneRuntime/Math/Types.hxx"
#include "AnemoneRuntime/Math/Transform2.hxx"
#include "AnemoneRuntime/Math/Matrix3x2.hxx"

#include <WinSock2.h>

struct VertexPacked
{
    Anemone::Math::Packed::Vector3F Position;
    Anemone::Math::Packed::Vector3F Normal;
    Anemone::Math::Packed::Vector2F TexCoord;
};
static_assert(sizeof(VertexPacked) == (sizeof(float) * 8));

struct VertexAligned
{
    Anemone::Math::Aligned::Vector3F Position;
    Anemone::Math::Aligned::Vector3F Normal;
    Anemone::Math::Aligned::Vector2F TexCoord;
};
static_assert(sizeof(VertexAligned) == (sizeof(float) * 12));

struct VertexNatural
{
    Anemone::Math::Vector3F Position;
    Anemone::Math::Vector3F Normal;
    Anemone::Math::Vector2F TexCoord;
};

static_assert(sizeof(VertexNatural) == (sizeof(float) * 12));

anemone_noinline Anemone::Math::Matrix3x2F Too(Anemone::Math::Transform2F const& xform)
{
    return ToMatrix3x2(xform);
}

#include "AnemoneRuntime/App/Application.hxx"

void TestTasking();

namespace Anemone::System // platform specific primitives
{
    class File;
    class Directory;
    class NetworkSocket;
    class NetworkAddress; // ip address; stores IPv4 as IPv6 mapped
}

namespace Anemone::Storage //
{

}

namespace Anemone::Threading
{
    // auto & reset event - possible implementations
    // - CreateEvent (WinAPI)
    // - SRWLOCK + CONDITION_VARIABLE (WinAPI)
    // - WaitOnAddress (WinAPI)
    // - futex (Linux)
    // - pthread_cond_t + pthread_mutex_t (Linux)
    // - eventfd (Linux)

#if ANEMONE_PLATFORM_LINUX
    class LinuxManualResetEvent
    {
        int32_t m_descriptor = -1;

    public:
        LinuxManualResetEvent(bool signaled)
        {
            this->m_descriptor = eventfd(signaled ? 1 : 0, EFD_CLOEXEC | EFD_NONBLOCK);
            AE_ASSERT(this->m_descriptor != -1, "Failed to create eventfd");
        }

        void Set()
        {
            uint64_t const increment = 1;

            if (write(this->m_descriptor, &increment, sizeof(increment)) < 0)
            {
                AE_ASSERT(errno == EAGAIN, "Failed to set eventfd");
            }
        }

        void Reset()
        {
            uint64_t previous = 0;

            if (read(this->m_descriptor, &previous, sizeof(previous)) < 0)
            {
                AE_ASSERT(errno == EAGAIN, "Failed to reset eventfd");
            }
        }

        void Wait()
        {
            pollfd pfd{
                .fd = this->m_descriptor,
                .events = POLLIN,
                .revents = 0};

            if (poll(&pfd, 1, -1) < 0)
            {
                AE_ASSERT(false, "Failed to wait on eventfd");
            }
        }
    };
#endif
}

namespace Anemone::Network
{
    class LocalSocket
    {
    public:
        virtual ~LocalSocket() = default;
    };
}

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#include <afunix.h>

using NativeUnixSocket = SOCKET;

class UnixSocket final
{
    NativeUnixSocket m_handle;

public:
    UnixSocket()
        : m_handle{}
    {
    }

    UnixSocket(NativeUnixSocket handle)
        : m_handle{handle}
    {
    }

    UnixSocket(UnixSocket const&) = delete;
    UnixSocket(UnixSocket&& other) noexcept
        : m_handle{std::exchange(other.m_handle, {})}
    {
    }

    UnixSocket& operator=(UnixSocket const&) = delete;
    UnixSocket& operator=(UnixSocket&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->Close();
            this->m_handle = std::exchange(other.m_handle, {});
        }

        return *this;
    }

    ~UnixSocket()
    {
        this->Close();
    }

    static std::expected<UnixSocket, Anemone::Error> Create()
    {
        UnixSocket h{socket(AF_UNIX, SOCK_STREAM, 0)};
        return h;
    }

    void Close()
    {
        if (this->m_handle != INVALID_SOCKET)
        {
            closesocket(this->m_handle);
            this->m_handle = INVALID_SOCKET;
        }
    }

    NativeUnixSocket GetHandle() const
    {
        return this->m_handle;
    }

    bool IsValid() const
    {
        return this->m_handle != INVALID_SOCKET;
    }

    static sockaddr_un CreateSocketAddress(std::string_view name)
    {
        AE_TRACE(Info, "Creating socket address for '{}'", name);
        sockaddr_un result{};
        result.sun_family = AF_UNIX;

        auto const id = Anemone::FNV1A128::FromString(name);

        fmt::format_to(result.sun_path, "anemone_{:016x}{:016x}.sock", id[1], id[0]);

        return result;
    }

    std::expected<void, Anemone::Error> Bind(std::string_view path)
    {
        AE_TRACE(Info, "Binding socket to '{}'", path);
        sockaddr_un addr = CreateSocketAddress(path);

        ::DeleteFileA(addr.sun_path);

        if (bind(this->m_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        return {};
    }

    std::expected<void, Anemone::Error> Connect(std::string_view path)
    {
        AE_TRACE(Info, "Connecting socket to '{}'", path);
        sockaddr_un addr = CreateSocketAddress(path);

        if (connect(this->m_handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        AE_TRACE(Info, "Connected socket to '{}'", path);
        return {};
    }

    std::expected<void, Anemone::Error> Listen(int backlog)
    {
        AE_TRACE(Info, "Listening on socket with backlog '{}'", backlog);
        if (listen(this->m_handle, backlog) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        return {};
    }

    std::expected<UnixSocket, Anemone::Error> Accept()
    {
        AE_TRACE(Info, "Accepting connection on socket");
        sockaddr_un addr{};
        int len = sizeof(addr);
        NativeUnixSocket const client = accept(this->m_handle, reinterpret_cast<sockaddr*>(&addr), &len);

        if (client == INVALID_SOCKET)
        {
            AE_TRACE(Error, "Failed to accept socket: {}", WSAGetLastError());
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        AE_TRACE(Info, "Accepted connection on socket");
        return UnixSocket{client};
    }

    std::expected<size_t, Anemone::Error> Send(std::span<std::byte const> data)
    {
        AE_TRACE(Info, "Sending data of size '{}'", data.size());
        int const result = send(this->m_handle, reinterpret_cast<char const*>(data.data()), static_cast<int>(data.size()), 0);

        if (result < 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        AE_TRACE(Info, "Sent data of size '{}'", result);
        return static_cast<size_t>(result);
    }

    std::expected<size_t, Anemone::Error> Receive(std::span<std::byte> data)
    {
        AE_TRACE(Info, "Receiving data of size '{}'", data.size());
        int const result = recv(this->m_handle, reinterpret_cast<char*>(data.data()), static_cast<int>(data.size()), 0);

        if (result < 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        AE_TRACE(Info, "Received data of size '{}'", result);
        return static_cast<size_t>(result);
    }

    // gets local address
    std::expected<sockaddr_storage, Anemone::Error> GetLocalAddress()
    {
        sockaddr_storage addr{};
        int len = sizeof(addr);

        if (getsockname(this->m_handle, reinterpret_cast<sockaddr*>(&addr), &len) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        return addr;
    }

    // gets remote address
    std::expected<sockaddr_storage, Anemone::Error> GetRemoteAddress()
    {
        sockaddr_storage addr{};
        int len = sizeof(addr);

        if (getpeername(this->m_handle, reinterpret_cast<sockaddr*>(&addr), &len) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        return addr;
    }
};

#pragma comment(lib, "Ws2_32.lib")
#endif


#include "AnemoneRuntime/Network/IpEndPoint.hxx"
#include "AnemoneRuntime/Network/Socket.hxx"


// note: networking done via Client/Server abstract classes
// - LocalClient, LocalServer
// - TcpClient, TcpServer
// - UdpClient, UdpServer

// plain networking supported via Socket and SocketEndPoint generic types

// SocketEndPoint (EndPoint) serves as a type erased end point object.

namespace Anemone::Network2
{
    struct EndPoint final
    {
    };

    struct IpV4EndPoint final
    {
        // Network::IpAddress
    };
}

AE_DECLARE_PROFILE(ClientWorker);
AE_DECLARE_PROFILE(ServerWorker);

#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Math/Random.hxx"


#include <random>

int main(int argc, char* argv[])
{
    Anemone::RuntimeInitializer runtime{argc, argv};
    {
    }
    GXtrace.Create();
    {
        Anemone::Math::Random rng{static_cast<uint64_t>(argc)};
        Anemone::Math::UniformDistribution dist{0.0, 1.0};
        double const d = dist(rng);
        GXtrace->Log(Anemone::TraceLevel::Critical,"Hello {}", "World");
        GXtrace->Log(Anemone::TraceLevel::Critical, "This is a test: {}", d);
    }
    GXtrace.Destroy();
    {
        Anemone::CriticalSection cs{};
        Anemone::UniqueLock _{cs};

        if (argc > 0)
        {
            AE_PANIC("Hell of panic from {}", argv[0]);
        }
    }
    {
        Anemone::GTrace->WriteLine(Anemone::TraceLevel::Critical, "Hello {}", "World");
        Anemone::GTrace->WriteLine(Anemone::TraceLevel::Critical, "This is a test: {}", 42);
        AE_ASSERT(false);
    }
    {
#if true
        Anemone::Network::IpAddress i1 = Anemone::Network::IpAddressV6::Localhost();
        Anemone::Network::IpAddress i2 = Anemone::Network::IpAddressV4::Localhost();
        i2 = i1;

        Anemone::Network::IpEndPoint e0{};
        Anemone::Network::IpEndPoint e1{i2, 2137};
        {
            if (auto client = Anemone::Network::Socket::Create(
                    Anemone::Network::AddressFamily::InterNetwork,
                    Anemone::Network::SocketType::Stream,
                    Anemone::Network::ProtocolType::Tcp))
            {
                Anemone::Network::SocketEndPoint ep{};

                if (auto err = client->Connect(ep))
                {
                }
                else
                {
                    AE_TRACE(Error, "Failed to connect: {}", err.error());
                }
            }
        }

#endif

        std::string ss;
        // if (e1.ToString(ss))
        {
            AE_TRACE(Info, "IpEndPoint: {}: {}", ss, fmt::ptr(&e1));
        }
        // else
        {
            AE_TRACE(Error, "IpEndPoint: failed");
        }
    }
#if ANEMONE_PLATFORM_WINDOWS
    {
        UnixSocket server = UnixSocket::Create().value();

        [[maybe_unused]] auto bb = server.Bind("test.sock");
        [[maybe_unused]] auto bc = server.GetLocalAddress();


        class T : public Anemone::Tasks::Task
        {
        protected:
            void OnExecute() override
            {
                AE_PROFILE_SCOPE(ClientWorker);
                AE_TRACE(Info, "Client task started");
                UnixSocket client = UnixSocket::Create().value();
                [[maybe_unused]] auto cc = client.Connect("test.sock");
                Anemone::Tasks::GTaskScheduler->Delay(Anemone::Duration::FromMilliseconds(1000));
                [[maybe_unused]] auto dd = client.Send(std::as_bytes(std::span{"hello world"}));
                [[maybe_unused]] auto la = client.GetLocalAddress();
                [[maybe_unused]] auto ra = client.GetRemoteAddress();
                client.Close();
                Anemone::Tasks::GTaskScheduler->Delay(Anemone::Duration::FromMilliseconds(1000));
                AE_TRACE(Info, "Client task finished");
            }
        };

        Anemone::Tasks::AwaiterHandle awaiter{new Anemone::Tasks::Awaiter()};
        Anemone::Tasks::AwaiterHandle dependency{new Anemone::Tasks::Awaiter()};
        Anemone::Tasks::TaskHandle t = new T();
        Anemone::Tasks::GTaskScheduler->Dispatch(*t, awaiter, dependency);


        std::array<std::byte, 64> buffer{};
        [[maybe_unused]] auto ee = server.Listen(5);

        while (auto other = server.Accept())
        {
            AE_PROFILE_SCOPE(ServerWorker);
            other->Receive(buffer);
            [[maybe_unused]] auto la = other->GetLocalAddress();
            [[maybe_unused]] auto ra = other->GetRemoteAddress();
            AE_TRACE(Info, "Received: {}", std::string_view{reinterpret_cast<char*>(buffer.data()), buffer.size()});
            break;
        }

        Anemone::Tasks::GTaskScheduler->Wait(awaiter);
    }
#endif

#if true
    {
        class App : public Anemone::App::Application
        {
        public:
            bool Closing = false;

        public:
            void OnWindowClose(Anemone::App::Window& window, Anemone::App::WindowCloseEventArgs& args) override
            {
                (void)window;
                (void)args;
                this->Closing = true;
            }

            void OnGamepadButtonDown(Anemone::App::InputDeviceId device, Anemone::App::GamepadButtonEventArgs& args) override
            {
                AE_TRACE(Info, "GamepadButtonDown: device={}, button={}", device.Inner, std::to_underlying(args.Button));

                if (args.Button == Anemone::App::GamepadButton::Menu)
                {
                    this->Closing = true;
                }
            }

            void OnGamepadButtonUp(Anemone::App::InputDeviceId& device, Anemone::App::GamepadButtonEventArgs& args) override
            {
                AE_TRACE(Info, "GamepadButtonUp: device={}, button={}", device.Inner, std::to_underlying(args.Button));
            }

            void OnGamepadAnalog(Anemone::App::InputDeviceId device, Anemone::App::GamepadAnalogEventArgs& args) override
            {
                AE_TRACE(Info, "GamepadAnalog: device={}, axis={}, value={}", device.Inner, std::to_underlying(args.Axis), args.Value);
            }

            void OnCharacterReceived(Anemone::App::Window& window, Anemone::App::CharacterReceivedEventArgs& args) override
            {
                (void)window;
                std::u32string_view sv{&args.Character, 1};
                std::string u{};
                Anemone::Unicode::Convert(u, sv, Anemone::Unicode::ConversionType::None);
                AE_TRACE(Info, "CharacterReceived: codepoint={} ({}), repat={}", static_cast<uint32_t>(args.Character), u, args.Repeat);

                if (args.Character == 'q')
                {
                    this->Closing = true;
                    AE_PANIC("This is not fine");
                }
            }
        };

        App app{};

        auto win = app.MakeWindow(Anemone::App::WindowType::Form);
        win->SetInputEnabled(true);

        while (not app.Closing)
        {
            app.ProcessMessages();
        }
    }
#endif

    auto const& processor = Anemone::System::GetProcessorProperties();

    AE_TRACE(Info, "PhysicalCores:       '{}'", processor.PhysicalCores);
    AE_TRACE(Info, "LogicalCores:        '{}'", processor.LogicalCores);
    AE_TRACE(Info, "PerformanceCores:    '{}'", processor.PerformanceCores);
    AE_TRACE(Info, "EfficiencyCores:     '{}'", processor.EfficiencyCores);
    AE_TRACE(Info, "CacheSizeLevel1:     '{}'", processor.CacheSizeLevel1);
    AE_TRACE(Info, "CacheSizeLevel2:     '{}'", processor.CacheSizeLevel2);
    AE_TRACE(Info, "CacheSizeLevel3:     '{}'", processor.CacheSizeLevel3);
    AE_TRACE(Info, "CacheSizeLevel4:     '{}'", processor.CacheSizeLevel4);
    AE_TRACE(Info, "CacheLineSize:       '{}'", processor.CacheLineSize);
    AE_TRACE(Info, "Name:                '{}'", processor.Name);
    AE_TRACE(Info, "Vendor:              '{}'", processor.Vendor);

    auto const& environment = Anemone::System::GetEnvironment();
    AE_TRACE(Info, "SystemVersionString: '{}'", environment.SystemVersion);
    AE_TRACE(Info, "SystemId:            '{}'", environment.SystemId);
    AE_TRACE(Info, "ExecutablePath:      '{}'", environment.ExecutablePath);
    AE_TRACE(Info, "StartupPath:         '{}'", environment.StartupPath);
    AE_TRACE(Info, "ComputerName:        '{}'", environment.ComputerName);
    AE_TRACE(Info, "UserName:            '{}'", environment.UserName);
    AE_TRACE(Info, "DesktopPath:         '{}'", environment.DesktopPath);
    AE_TRACE(Info, "DocumentsPath:       '{}'", environment.DocumentsPath);
    AE_TRACE(Info, "ProfilePath:         '{}'", environment.ProfilePath);
    AE_TRACE(Info, "TempPath:            '{}'", environment.TempPath);
    AE_TRACE(Info, "StartupTime:         '{}'", environment.StartupTime);

    {
        AE_PROFILE_SCOPE(Main);
        {
            AE_PROFILE_SCOPE(Inner);
            AE_TRACE(Warning, "Hello {}", Anemone::System::GetEnvironment().StartupTime);
            fmt::println("Hello at {}", Anemone::Instant::Now().SinceEpoch());
            Anemone::Diagnostic::GetCurrentStackTrace([&](auto const& entry)
            {
                AE_TRACE(Warning, "{}", entry);
                return true;
            });
        }

        Anemone::SleepThread(Anemone::Duration::FromMilliseconds(500));
        {
            AE_PROFILE_SCOPE(Outer);
            Anemone::SleepThread(Anemone::Duration::FromMilliseconds(500));
            {
                AE_PROFILE_SCOPE(Inner);
                Anemone::SleepThread(Anemone::Duration::FromMilliseconds(500));

                AE_ASSERT(false, "This is a test of the emergency broadcast system: {}", 42);

                TestTasking();

                AE_TRACE(Critical, "Crashed!?");
                // std::atomic<int>* p = nullptr;
                // p->store(42);
            }
            Anemone::SleepThread(Anemone::Duration::FromMilliseconds(500));
        }
        Anemone::SleepThread(Anemone::Duration::FromMilliseconds(500));
    }

    return 0;
}


AE_DECLARE_PROFILE(ReportTimeUsage);

template <typename T>
void ReportTimeUsage(T t)
{
    AE_PROFILE_SCOPE(ReportTimeUsage);

    auto const started = Anemone::Instant::Now();
    t();
    auto const elapsed = started.QueryElapsed();
    fmt::println("Time elapsed: {}", elapsed);
}

AE_DECLARE_PROFILE(ParallelFor4);

#if true
#define TRACEPOINT(format, ...) AE_TRACE(Verbose, "({}): {} at {:x}: " format, Anemone::DateTime::Now(), __func__, Anemone::GetThisThreadId().Value __VA_OPT__(, ) __VA_ARGS__);

#else
#define TRACEPOINT(...)
#endif


#include "AnemoneRuntime/Crypto/Sha512.hxx"

class MyFancyTaskWithAlignment : public Anemone::Tasks::Task
{
public:
    alignas(64) std::array<uint8_t, 64> m_Data{};
    Anemone::Crypto::Sha512Hash m_Output{};

public:
    MyFancyTaskWithAlignment() = default;

    void OnExecute() override
    {
        AE_PROFILE_SCOPE(ParallelFor4);
        TRACEPOINT("started: ptr={}, id={}", fmt::ptr(this), this->GetId());
#if 0
        using namespace Anemone::Crypto;

        auto hash = sha512_from_buffer(std::as_bytes(std::span{this->m_Data}));

        size_t limit = 1'000'00;
        if (this->GetId() % 2 == 0)
        {
            limit *= 13;
            limit /= 10;
        }
        else
        {
            limit *= 8;
            limit /= 10;
        }

        for (size_t i = 0; i < limit; ++i)
        {
            hash = sha512_from_buffer(std::as_bytes(std::span{hash}));
        }

        this->m_Output = hash;
#else
        Anemone::SleepThread(Anemone::Duration::FromMilliseconds(100));
#endif

        TRACEPOINT("finished: ptr={}, id={}", fmt::ptr(this), this->GetId());
    }
};

AE_DECLARE_PROFILE(EnumerateConsoleVariables);
AE_DECLARE_PROFILE(ParallelFor1);
AE_DECLARE_PROFILE(ParallelFor2);
AE_DECLARE_PROFILE(ParallelFor3);

#include "AnemoneRuntime/Threading/UserCriticalSection.hxx"
#include "AnemoneRuntime/Threading/ConcurrentAccess.hxx"
#include "AnemoneRuntime/Tasks/ParallelFor.hxx"

void TestTasking()
{
    {
        using namespace Anemone::Tasks;
        using namespace Anemone;

        {
#ifdef UNSYNCED1
            CriticalSectionSlim scs{};
            ConcurrentAccess ca{};
            size_t processed = 0;
#endif
            constexpr size_t total = 128 * 1024 + 63;
            ReportTimeUsage([&]
            {
                ParallelFor(
                    total,
                    16,
                    [&](size_t first, size_t last)
                {
                    (void)first;
                    (void)last;
                    AE_PROFILE_SCOPE(ParallelFor1);
#ifdef UNSYNCED1
                    {
                        UniqueLock lock{scs};
                        UniqueLock lock2{ca};
                        processed += (last - first);
                    }
#endif
                    TRACEPOINT("ParallelFor: [{}:{}): {}", first, last, last - first);
                    Anemone::SleepThread(Anemone::Duration::FromMilliseconds(50));
                },
                    [&](size_t count)
                {
                    AE_PROFILE_SCOPE(ParallelFor2);
                    (void)count;
#ifdef UNSYNCED1
                    AE_ASSERT(processed == count);
#endif
                    TRACEPOINT("ParallelFor: done [0:{})", count);
                    Anemone::SleepThread(Anemone::Duration::FromMilliseconds(10));
                });
            });
#ifdef UNSYNCED1
            AE_FATAL(processed == total);
#endif
        }
        {
            UserCriticalSection scs{};
            size_t processed = 0;
            constexpr size_t total = 128 * 1024 + 63;

            ReportTimeUsage([&]
            {
                ParallelFor(
                    total,
                    1,
                    [&](size_t first, size_t last)
                {
                    AE_PROFILE_SCOPE(ParallelFor3);
                    {
                        UniqueLock _{scs};
                        processed += (last - first);
                    }
                    Anemone::SleepThread(Anemone::Duration::FromMilliseconds(5));
                },
                    [&](size_t count)
                {
                    AE_PROFILE_SCOPE(ParallelFor3);
                    (void)count;
                    AE_ASSERT(processed == count);
                    TRACEPOINT("ParallelFor: done [0:{})", count);
                });
            });
            AE_ASSERT(processed == total);
        }
        {
            AwaiterHandle barrier{new Awaiter{}};
            AwaiterHandle dummy{new Awaiter{}};

            TaskHandle h1{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*h1, barrier, dummy);
            TaskHandle h2{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*h2, barrier, dummy);
            dummy = {};
            h1 = {};
            h2 = {};

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(barrier);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::SleepThread(Anemone::Duration::FromSeconds(10));
        TRACEPOINT("<< waiting for workers to finish mid test");

        {
            AwaiterHandle barrier{new Awaiter{}};
            AwaiterHandle dummy{new Awaiter{}};

            for (size_t i = 0; i < 18; ++i)
            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                GTaskScheduler->Dispatch(*h1, barrier, dummy);
            }

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(barrier);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::SleepThread(Anemone::Duration::FromSeconds(10));
        TRACEPOINT("<< waiting for workers to finish mid test");

        {
            AwaiterHandle leaf{new Awaiter{}};
            AwaiterHandle trunk{new Awaiter{}};
            AwaiterHandle root{new Awaiter{}};

            TaskHandle h1{new MyFancyTaskWithAlignment()};
            TaskHandle h2{new MyFancyTaskWithAlignment()};

            GTaskScheduler->Dispatch(*h1, trunk, leaf);

            GTaskScheduler->Dispatch(*h2, root, trunk);

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });
        }
        TRACEPOINT(">> waiting for workers to finish mid test");
        AE_TRACE(Verbose, "------------------------------------------");
        // Anemone::SleepThread(Anemone::Duration::FromSeconds(2));
        TRACEPOINT("<< waiting for workers to finish mid test");
        {
            AwaiterHandle root{new Awaiter{}};
            AwaiterHandle trunk1{new Awaiter{}};
            AwaiterHandle trunk2{new Awaiter{}};
            AwaiterHandle leaf{new Awaiter{}};

            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                TaskHandle h2{new MyFancyTaskWithAlignment()};
                TaskHandle h3{new MyFancyTaskWithAlignment()};
                GTaskScheduler->Dispatch(*h1, trunk1, leaf);
                GTaskScheduler->Dispatch(*h2, trunk2, trunk1);
                GTaskScheduler->Dispatch(*h3, root, trunk2);
            }

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });

            AE_TRACE(Verbose, "------------------------------------------");
        }

        AE_TRACE(Verbose, "-- should wait here ----------------------------------------");
        {
            AwaiterHandle awaiters[65]{};

            for (AwaiterHandle& awaiter : awaiters)
            {
                awaiter = new Awaiter{};
            }

            TaskHandle tasks[64]{};

            for (TaskHandle& task : tasks)
            {
                task = new MyFancyTaskWithAlignment();
            }

            for (size_t i = 0; i < 64; ++i)
            {
                GTaskScheduler->Dispatch(*tasks[i], awaiters[i + 1], awaiters[i]);
            }

            // AwaiterHandle accumulate{new Awaiter{}};
            // AwaiterHandle wait{new Awaiter{}};
            AwaiterHandle total = awaiters[64];

            //{
            //    for (size_t i = 0; i < 64; ++i)
            //    {
            //        TaskHandle h1{new MyFancyTaskWithAlignment()};
            //        DispatchTask(*h1, wait, accumulate);
            //        total = accumulate;
            //        // accumulate = std::exchange(wait, AwaiterHandle{new Awaiter{}});
            //        wait = std::exchange(accumulate, AwaiterHandle{new Awaiter{}});
            //    }
            //}

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(total);
            });
        }

        AE_TRACE(Verbose, "------------------------------------------");
        {
            AwaiterHandle ca1{new Awaiter()};
            AwaiterHandle ca2{new Awaiter()};
            AwaiterHandle ca3{new Awaiter()};

            AwaiterHandle cw1{new Awaiter()};
            AwaiterHandle cw2{new Awaiter()};
            AwaiterHandle cw3{new Awaiter()};

            AwaiterHandle root1{};
            AwaiterHandle root2{};
            AwaiterHandle root3{};

            for (size_t i = 0; i < 64; ++i)
            {
                TaskHandle h1{new MyFancyTaskWithAlignment()};
                TaskHandle h2{new MyFancyTaskWithAlignment()};
                TaskHandle h3{new MyFancyTaskWithAlignment()};

                GTaskScheduler->Dispatch(*h1, ca1, cw1);
                GTaskScheduler->Dispatch(*h2, ca2, cw2);
                GTaskScheduler->Dispatch(*h3, ca3, cw3);

                root1 = ca1;
                root2 = ca2;
                root3 = ca3;

#if 0 // parallel
                            ca1 = std::exchange(cw1, AwaiterHandle{new Awaiter()});
                            ca2 = std::exchange(cw2, AwaiterHandle{new Awaiter()});
                            ca3 = std::exchange(cw3, AwaiterHandle{new Awaiter()});
#else // serial
                cw1 = std::exchange(ca1, AwaiterHandle{new Awaiter()});
                cw2 = std::exchange(ca2, AwaiterHandle{new Awaiter()});
                cw3 = std::exchange(ca3, AwaiterHandle{new Awaiter()});
#endif
            }

            AwaiterHandle root{new Awaiter{}};
            TaskHandle r1{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r1, root, root1);
            TaskHandle r2{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r2, root, root2);
            TaskHandle r3{new MyFancyTaskWithAlignment()};
            GTaskScheduler->Dispatch(*r3, root, root3);

            ReportTimeUsage([&]
            {
                GTaskScheduler->Wait(root);
            });
        }
        AE_TRACE(Verbose, ">> finish ------------------------------------------");
    }
}
#endif
