
namespace Anemone::Internal
{
    extern void InitializeWindowsInput();
    extern void FinalizeWindowsInput();
    extern void InitializeGameInput();
    extern void FinalizeGameInput();
    extern void InitializeXInput();
    extern void FinalizeXInput();

    extern void InitializeInput()
    {
        InitializeWindowsInput();
        InitializeGameInput();
        InitializeXInput();
    }

    extern void FinalizeInput()
    {
        FinalizeXInput();
        FinalizeGameInput();
        FinalizeWindowsInput();
    }
}
