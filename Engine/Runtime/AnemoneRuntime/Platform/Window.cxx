#include "AnemoneRuntime/Platform/Window.hxx"
#include "AnemoneRuntime/Platform/Platform.hxx"

namespace Anemone::Platform
{
    Window::Window()
    {
        Internal::GPlatformStatics->WindowsCollection.PushBack(this);
    }

    Window::~Window()
    {
        Internal::GPlatformStatics->WindowsCollection.Remove(this);
    }
}
