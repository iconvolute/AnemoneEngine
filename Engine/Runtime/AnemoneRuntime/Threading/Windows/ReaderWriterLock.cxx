#include "AnemoneRuntime/Threading/ReaderWriterLock.hxx"
#include "AnemoneRuntime/Diagnostic/Assert.hxx"
#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

namespace Anemone::Threading
{
    ReaderWriterLock::ReaderWriterLock()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Create(this->_native);

        InitializeSRWLock(&nativeThis.Inner);
    }

    ReaderWriterLock::~ReaderWriterLock()
    {
        Platform::Destroy(this->_native);
    }

    void ReaderWriterLock::EnterShared()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        AcquireSRWLockShared(&nativeThis.Inner);
    }

    bool ReaderWriterLock::TryEnterShared()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        return TryAcquireSRWLockShared(&nativeThis.Inner) != FALSE;
    }

    void ReaderWriterLock::LeaveShared()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        ReleaseSRWLockShared(&nativeThis.Inner);
    }

    void ReaderWriterLock::Enter()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        AcquireSRWLockExclusive(&nativeThis.Inner);
    }

    bool ReaderWriterLock::TryEnter()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        return TryAcquireSRWLockExclusive(&nativeThis.Inner) != FALSE;
    }

    void ReaderWriterLock::Leave()
    {
        Platform::NativeReaderWriterLock& nativeThis = Platform::Get(this->_native);

        ReleaseSRWLockExclusive(&nativeThis.Inner);
    }
}
