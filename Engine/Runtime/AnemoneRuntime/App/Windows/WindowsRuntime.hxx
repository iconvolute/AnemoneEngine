#pragma once
#include "AnemoneRuntime/App/Runtime.hxx"
#include "AnemoneRuntime/UninitializedObject.hxx"
#include "AnemoneRuntime/App/Windows/WindowsApplication.hxx"
#include "AnemoneRuntime/App/Windows/WindowsInput.hxx"

namespace Anemone::App
{
    extern UninitializedObject<WindowsAppState> GWindowsAppState;
    extern UninitializedObject<WindowsInput> GWindowsInput;
}
