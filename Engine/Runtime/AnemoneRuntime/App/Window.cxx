#include "AnemoneRuntime/App/Window.hxx"

#include "Application.hxx"

namespace Anemone::App
{
    Window::Window(Application* application)
        : m_application{application}
    {
        this->m_application->AddWindow(*this);
    }

    Window::~Window()
    {
        this->m_application->RemoveWindow(*this);
    }

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
