#include "AnemoneEntryPoint/EntryPoint.hxx"
#include "AnemoneSystem/Environment.hxx"


#include <string>
#include <string_view>

#include "AnemoneDiagnostics/Error.hxx"
#include "AnemoneBase/UninitializedObject.hxx"

#include <expected>

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Dwm.hxx"
#include "AnemoneInterop/Windows/Registry.hxx"
#endif
#include "AnemoneDiagnostics/ConsoleTraceListener.hxx"

#include "AnemoneNetwork/Tcp.hxx"
#include "AnemoneThreading/CriticalSection.hxx"
#include "AnemoneThreading/Lock.hxx"

#include "AnemoneStorage/FileSystem.hxx"
#include "AnemoneSystem/ProcessorProperties.hxx"

#include "AnemoneBase/Uuid.hxx"
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
#include "AnemoneDiagnostics/Debug.hxx"
#include "AnemoneSystem/Clipboard.hxx"

#include "AnemoneSystem/CommandLine.hxx"
#include "AnemoneDiagnostics/Trace.hxx"

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
        (void)window;
        (void)args;
    }

    void OnMouseButtonUp(Anemone::HostWindow& window, Anemone::MouseButtonEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnMouseButtonClick(Anemone::HostWindow& window, Anemone::MouseButtonEventArgs& args) override
    {
        (void)window;
        (void)args;
    }

    void OnKeyDown(Anemone::HostWindow& window, Anemone::KeyEventArgs& args) override
    {
        (void)window;
        (void)args;
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
        (void)window;
        (void)args;
    }
};
#include "AnemoneSystem/Environment.hxx"

#include "AnemoneBase/FNV.hxx"
#include "AnemoneSystem/Process.hxx"
#include "AnemoneTasks/TaskScheduler.hxx"
#include "AnemoneMath/Detail/SimdFloat.hxx"
#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneDiagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneInterop/Windows/Environment.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#endif

#include "AnemoneMath/PidController.hxx"
#include "AnemoneRandom/Generator.hxx"
#include "AnemoneRandom/Distribution.hxx"


#if __has_include("AnemoneRenderVulkan/VulkanDevice.hxx")
#include "AnemoneRenderVulkan/VulkanDevice.hxx"
#endif

#include "AnemoneDiagnostics/FileTraceListener.hxx"
#include "AnemoneApplication/HostSplashScreen.hxx"

namespace anemone
{
    template <size_t Size, size_t Alignment>
    struct TypeErased
    {
        alignas(Alignment) std::byte buffer[Size];

        template <typename T, typename... Args>
        void Create(Args&&... args)
        {
            new (buffer) T(std::forward<Args>(args)...);
        }

        template <typename T>
        void Destroy()
        {
            std::launder(reinterpret_cast<T*>(buffer))->~T();
        }

        template <typename T>
        void Assign(T const& other)
        {
            (*this->Get<T>()) = other;
        }

        template <typename T>
        void Assign(T&& other)
        {
            (*this->Get<T>()) = std::forward<T&&>(other);
        }

        template <typename T>
        T* Get()
        {
            return std::launder(reinterpret_cast<T*>(buffer));
        }

        template <typename T>
        T const* Get() const
        {
            return std::launder(reinterpret_cast<T const*>(buffer));
        }
    };
}

#include "AnemoneRenderVulkan/Module.hxx"
#include "AnemoneDiagnostics/ConsoleTraceListener.hxx"
#include "AnemoneRender/SwapChain.hxx"

void Scratchpad_ModuleInitialize() { }
void Scratchpad_ModuleFinalize() { }

anemone_noinline int AnemoneMain(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    Anemone::UninitializedObject<Anemone::FileTraceListener> fts{};
//
//    Anemone::HostSplashScreen::Initialize();
//#if ANEMONE_PLATFORM_WINDOWS
//    SleepEx(3'000, FALSE);
//#endif
//
//    constexpr size_t count = 1000;
//    for (size_t i = 0; i < count;++i)
//    {
//        Anemone::HostSplashScreen::Get().SetProgress(static_cast<float>(i) / static_cast<float>(count));
//        Anemone::HostSplashScreen::Get().SetText(fmt::format("{}/{}", i, count));
//        SleepEx(15, FALSE);
//
//    }
//
//    Anemone::HostSplashScreen::Finalize();
//
//
//    Anemone::Parallel::For(10000, 128, [](size_t first, size_t count)
//    {
//        AE_TRACE(Error, "First: {:016x}, Count: {:016x}", first, count);
//    });

    if (auto fh = Anemone::FileSystem::GetPlatformFileSystem().CreateFileWriter("log.txt"))
    {
        fts.Create(Anemone::MakeReference<Anemone::FileOutputStream>(*fh));
        Anemone::Trace::Get().Register(*fts);
    }
    Anemone::ConsoleTraceListener ctl{};
    Anemone::Trace::Get().Register(ctl);

    {
        EH eh{};
        Anemone::HostApplication::Initialize(eh);
        auto window1 = Anemone::HostApplication::Get().MakeWindow(Anemone::WindowType::Game, Anemone::WindowMode::Windowed);

        if (window1)
        {
            window1->SetMinimumSize(Anemone::SizeF{640.0f, 480.0f});
            window1->SetInputEnabled(true);

#if __has_include("AnemoneRenderVulkan/VulkanDevice.hxx")
            auto rd = Anemone::CreateGpuDevice();
            auto sq = rd->CreateViewport(window1);
            auto ct = rd->GetImmediateContext();

#endif

            while (not window1->IsClosed())
            {
                if (window1->IsVisible())
                {
                    ct->BeginFrame();
                    ct->BeginDrawingViewport(*sq);
                    ct->BeginRecording();
                    ct->EndRecording();
                    ct->EndDrawingViewport(*sq);
                    ct->EndFrame();
                    //(void)sq;
                    //sq->Start();
                    //sq->Present();
                }

                Anemone::HostApplication::Get().ProcessMessages();
                Anemone::CurrentThread::Sleep(Anemone::Duration::FromMilliseconds(16));
            }
        }

        Anemone::HostApplication::Finalize();
    }
    if (fts)
    {
        Anemone::Trace::Get().Unregister(*fts);
        fts.Destroy();
    }

    Anemone::Trace::Get().Unregister(ctl);

    return 0;
}

#if false

AE_DECLARE_PROFILE(Main);
AE_DECLARE_PROFILE(Outer);
AE_DECLARE_PROFILE(Inner);

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

#endif


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

    return 0;
}

#endif
