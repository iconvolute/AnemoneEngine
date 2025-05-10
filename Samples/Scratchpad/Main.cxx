#include "AnemoneRuntime/Platform/EntryPoint.hxx"
#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/System/ProcessorProperties.hxx"


#include <string>
#include <string_view>

#include "AnemoneRuntime/Base/ErrorCode.hxx"
#include "AnemoneRuntime/Base/UninitializedObject.hxx"

#include <expected>

#include "AnemoneRuntime/Platform/FileSystem.hxx"
#include "AnemoneRuntime/Threading/CriticalSection.hxx"
#include "AnemoneRuntime/Threading/Lock.hxx"


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

namespace Anemone::inline FileSystemX
{
    struct Base64
    {
        static std::expected<size_t, ErrorCode> GetEncodedSize(size_t size);
        static std::expected<size_t, ErrorCode> GetDecodedSize(std::string_view encoded);

        static std::expected<void, ErrorCode> Encode(std::span<std::byte const> data, std::span<char> encoded);
        static std::expected<void, ErrorCode> Decode(std::string_view encoded, std::span<std::byte> data);
    };
}

#include "AnemoneRuntime/System/Application.hxx"
#include "AnemoneRuntime/Diagnostics/Debugger.hxx"
#include "AnemoneRuntime/Platform/StackTrace.hxx"
#include "AnemoneRuntime/System/Clipboard.hxx"

#include "AnemoneRuntime/System/Dialogs.hxx"

#include "AnemoneRuntime/System/CommandLine.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

class EH final : public Anemone::IApplicationEvents
{
public:
    EH() = default;
    ~EH() override = default;

    void OnMouseEnter(Anemone::Window& window, Anemone::MouseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseLeave(Anemone::Window& window, Anemone::MouseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }
    void OnMouseMove(Anemone::Window& window, Anemone::MouseMoveEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseWheel(Anemone::Window& window, Anemone::MouseWheelEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseButtonDown(Anemone::Window& window, Anemone::MouseButtonEventArgs& args) override
    {
        AE_TRACE(Error, "mouse button down: {}", std::to_underlying(args.Key));
        (void)window;
        (void)args;

        std::string ret;
        if (Anemone::Dialogs::OpenFile(&window, ret, "Select file",
                std::array{
                    Anemone::FileDialogFilter{"Any", "*.*"},
                }) == Anemone::DialogResult::Ok)
        {
            AE_TRACE(Error, "Selected file: '{}'", ret);

            Anemone::CriticalSection cs{};

            {
                Anemone::UniqueLock scope{cs};

                if (auto f = Anemone::FileSystem::GetFileInfo(ret))
                {
                    AE_TRACE(Error, "---------------");
                    AE_TRACE(Error, "File-Size:     '{}'", f->Size);
                    AE_TRACE(Error, "File-Created:  '{}'", f->Created);
                    AE_TRACE(Error, "File-Accessed: '{}'", f->Accessed);
                    AE_TRACE(Error, "File-Modified: '{}'", f->Modified);
                    AE_TRACE(Error, "File-ReadOnly: '{}'", f->ReadOnly);
                    AE_TRACE(Error, "File-Type:     '{}'", std::to_underlying(f->Type));
                    AE_TRACE(Error, "---------------");
                }

                Anemone::Clipboard::SetText(ret);
            }
        }
    }

    void OnMouseButtonUp(Anemone::Window& window, Anemone::MouseButtonEventArgs& args) override
    {
        AE_TRACE(Error, "mouse button up: {}", std::to_underlying(args.Key));
        (void)window;
        (void)args;
    }

    void OnMouseButtonClick(Anemone::Window& window, Anemone::MouseButtonEventArgs& args) override
    {
        (void)window;
        (void)args;

        *(volatile void**)0x42069 = nullptr;
    }

    void OnKeyDown(Anemone::Window& window, Anemone::KeyEventArgs& args) override
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

    void OnKeyUp(Anemone::Window& window, Anemone::KeyEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnCharacterReceived(Anemone::Window& window, Anemone::CharacterReceivedEventArgs& args) override
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

    void OnWindowClose(Anemone::Window& window, Anemone::WindowCloseEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowActivated(Anemone::Window& window, Anemone::WindowActivatedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowSizeChanged(Anemone::Window& window, Anemone::WindowSizeChangedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowLocationChanged(Anemone::Window& window, Anemone::WindowLocationChangedEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowResizeStarted(Anemone::Window& window, Anemone::WindowEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowResizeCompleted(Anemone::Window& window, Anemone::WindowEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnWindowDpiChanged(Anemone::Window& window, Anemone::WindowDpiChangedEventArgs& args) override
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
};

#include "AnemoneRuntime/System/Environment.hxx"
#include "AnemoneRuntime/System/ProcessorProperties.hxx"

anemone_noinline void test()
{
    AE_TRACE(Error, "cpu-Name:             '{}'", Anemone::ProcessorProperties::GetName());
    AE_TRACE(Error, "cpu-Vendor:           '{}'", Anemone::ProcessorProperties::GetVendor());
}

#include "AnemoneRuntime/System/FileHandle.hxx"
#include "AnemoneRuntime/Hash/FNV.hxx"
#include "AnemoneRuntime/System/Process.hxx"
#include "AnemoneRuntime/Tasks/TaskScheduler.hxx"

#include "AnemoneRuntime/Diagnostics/Platform/Error.hxx"

#if ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#endif

struct V2 : Anemone::DirectoryVisitor
{
    void Visit(std::string_view path, Anemone::FileInfo const& info) override
    {
        AE_TRACE(Error, "Path: '{}'", path);
        AE_TRACE(Error, "Size: '{}'", info.Size);
        AE_TRACE(Error, "Type: '{}'", std::to_underlying(info.Type));
        AE_TRACE(Error, "Created: '{}'", info.Created);
        AE_TRACE(Error, "Accessed: '{}'", info.Accessed);
        AE_TRACE(Error, "Modified: '{}'", info.Modified);
        AE_TRACE(Error, "ReadOnly: '{}'", info.ReadOnly);
    }
};

#include "AnemoneRuntime/Tasks/Parallel.hxx"

//
// Engine : IApplicationEvents
// GameEngine : Engine
// EditorEngine : Engine
//

int AnemoneMain(int argc, char** argv)
{
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
        struct Vi : Anemone::DirectoryVisitor
        {
            void Visit(std::string_view path, Anemone::FileInfo const& info) override
            {
                AE_TRACE(Error, "Path: '{}'", path);
                AE_TRACE(Error, "Size: '{}'", info.Size);
                AE_TRACE(Error, "Type: '{}'", std::to_underlying(info.Type));
                AE_TRACE(Error, "Created: '{}'", info.Created);
                AE_TRACE(Error, "Accessed: '{}'", info.Accessed);
                AE_TRACE(Error, "Modified: '{}'", info.Modified);
                AE_TRACE(Error, "ReadOnly: '{}'", info.ReadOnly);
            }
        };

        Vi vi{};
        (void)Anemone::FileSystem::DirectoryEnumerate(".", vi);
    }

    if (auto fh = Anemone::FileHandle::Create("non-existing", Anemone::FileMode::OpenExisting, Anemone::FileAccess::Read))
    {
        AE_TRACE(Error, "Should fail");
    }
    else
    {
        AE_TRACE(Error, "Non-existing: {}", fh.error());
    }

    Anemone::CurrentThread::YieldAnyThreadOnAnyProcessor();

    test();
    (void)argc;
    (void)argv;

    Anemone::FNV1A128 original{};

    if (auto h = Anemone::FileHandle::Create("test.txt", Anemone::FileMode::CreateAlways, Anemone::FileAccess::Write))
    {
        std::string_view s = "Hello World!\n";

        auto sb = std::as_bytes(std::span{s.data(), s.size()});

        for (size_t i = 0; i < 64; ++i)
        {
            original.Update(sb);
            (void)h->Write(sb);
        }

        (void)h->Flush();
    }

    Anemone::FNV1A128 current{};

    if (auto h = Anemone::FileHandle::Create("test.txt", Anemone::FileMode::OpenExisting, Anemone::FileAccess::Read))
    {
        std::vector<std::byte> buffer(32);
        auto sb = std::as_writable_bytes(std::span{buffer});

        while (true)
        {
            if (auto r = h->Read(sb))
            {
                if (*r == 0)
                {
                    break;
                }

                current.Update(sb.subspan(0, *r));
            }
            else
            {
                break;
            }
        }
    }

    auto hashOriginal = original.Finalize();
    auto hashCurrent = current.Finalize();

    AE_TRACE(Error, "hashes: [0]: {:016x} == {:016x} : {}", hashOriginal[0], hashCurrent[0], static_cast<int64_t>(hashOriginal[0]) - static_cast<int64_t>(hashCurrent[0]));
    AE_TRACE(Error, "hashes: [1]: {:016x} == {:016x} : {}", hashOriginal[1], hashCurrent[1], static_cast<int64_t>(hashOriginal[1]) - static_cast<int64_t>(hashCurrent[1]));

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
    AE_TRACE(Error, "cpu-Name:             '{}'", Anemone::ProcessorProperties::GetName());
    AE_TRACE(Error, "cpu-Vendor:           '{}'", Anemone::ProcessorProperties::GetVendor());
    AE_TRACE(Error, "cpu-EfficiencyCores:  '{}'", Anemone::ProcessorProperties::GetEfficiencyCoresCount());
    AE_TRACE(Error, "cpu-PerformanceCores: '{}'", Anemone::ProcessorProperties::GetPerformanceCoresCount());
    AE_TRACE(Error, "cpu-PhysicalCores:    '{}'", Anemone::ProcessorProperties::GetPhysicalCoresCount());
    AE_TRACE(Error, "cpu-LogicalCores:     '{}'", Anemone::ProcessorProperties::GetLogicalCoresCount());

    fmt::println("cpu-name:    '{}'", Anemone::ProcessorProperties::GetName());
    fmt::println("cpu-vendor:  '{}'", Anemone::ProcessorProperties::GetVendor());
    /*fmt::println("device-name: '{}'", Anemone::Environment::GetDeviceName());
    fmt::println("device-id:   '{}'", Anemone::Environment::GetDeviceUniqueId());*/

    EH eh{};

    auto window1 = Anemone::Application::MakeWindow(Anemone::WindowType::Game);
    auto window2 = Anemone::Application::MakeWindow(Anemone::WindowType::Game);

    if (window1 and window2)
    {
        window1->SetMinimumSize(Anemone::Math::SizeF{640.0f, 480.0f});
        window1->SetInputEnabled(true);

        window2->SetMinimumSize(Anemone::Math::SizeF{640.0f, 480.0f});
        window2->SetInputEnabled(true);

        while (not window1->IsClosed() or not window2->IsClosed())
        {
            auto const client = window1->GetClientBounds();
            auto const window = window1->GetBounds();
            AE_TRACE(Error, "Client: {}, {}, {}, {}, Window: {}, {}, {}, {}",
                client.X, client.Y, client.Width, client.Height,
                window.X, window.Y, window.Width, window.Height);

            Anemone::Application::ProcessMessages();
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
    Anemone::Diagnostics::GTrace.Create();
    Anemone::Platform::Internal::SetupTraceListeners();
    Anemone::Diagnostics::GTrace->AddListener(sl);
    Anemone::Diagnostics::GTrace->AddListener(dl);

    AE_TRACE(Verbose, "Verbose");
    AE_TRACE(Debug, "Debug");
    AE_TRACE(Information, "Information");
    AE_TRACE(Warning, "Warning");
    AE_TRACE(Error, "Error");
    AE_TRACE(Critical, "Critical");

    AE_ASSERT(false);

    _mm_pause();
    Anemone::Diagnostics::GTrace->WriteLine(Anemone::Diagnostics::TraceLevel::Information, "'Hello {} {}'", 42, "World");
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

    Anemone::Diagnostics::GTrace->RemoveListener(dl);
    Anemone::Diagnostics::GTrace->RemoveListener(sl);
    Anemone::Diagnostics::GTrace.Destroy();
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
#include "AnemoneRuntime/ErrorCode.hxx"
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

    static std::expected<UnixSocket, Anemone::ErrorCode> Create()
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

    std::expected<void, Anemone::ErrorCode> Bind(std::string_view path)
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

    std::expected<void, Anemone::ErrorCode> Connect(std::string_view path)
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

    std::expected<void, Anemone::ErrorCode> Listen(int backlog)
    {
        AE_TRACE(Info, "Listening on socket with backlog '{}'", backlog);
        if (listen(this->m_handle, backlog) != 0)
        {
            return std::unexpected(Anemone::System::Private::ErrorCodeFromWin32Error(WSAGetLastError()));
        }

        return {};
    }

    std::expected<UnixSocket, Anemone::ErrorCode> Accept()
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

    std::expected<size_t, Anemone::ErrorCode> Send(std::span<std::byte const> data)
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

    std::expected<size_t, Anemone::ErrorCode> Receive(std::span<std::byte> data)
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
    std::expected<sockaddr_storage, Anemone::ErrorCode> GetLocalAddress()
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
    std::expected<sockaddr_storage, Anemone::ErrorCode> GetRemoteAddress()
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
        GXtrace->Log(Anemone::Diagnostics::TraceLevel::Critical,"Hello {}", "World");
        GXtrace->Log(Anemone::Diagnostics::TraceLevel::Critical, "This is a test: {}", d);
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
        Anemone::Diagnostics::GTrace->WriteLine(Anemone::Diagnostics::TraceLevel::Critical, "Hello {}", "World");
        Anemone::Diagnostics::GTrace->WriteLine(Anemone::Diagnostics::TraceLevel::Critical, "This is a test: {}", 42);
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
