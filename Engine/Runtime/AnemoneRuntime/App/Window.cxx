#include "AnemoneRuntime/App/Window.hxx"

namespace Anemone::App
{
    Window::Window() = default;

    Window::~Window() = default;

    void Window::AcquireReference()
    {
        ++this->m_ReferenceCount;
    }

    void Window::ReleaseReference()
    {
        if (--this->m_ReferenceCount == 0)
        {
            delete this;
        }
    }
};
