#include "AnemoneRuntime.Diagnostics/FileTraceListener.hxx"

namespace Anemone
{
    FileTraceListener::FileTraceListener(Reference<FileOutputStream> stream)
        : _stream(std::move(stream))
    {
        AE_ASSERT(this->_stream, "FileOutputStream is null");
    }

    void FileTraceListener::TraceEvent(TraceLevel level, const char* message, size_t size)
    {
        (void)level;
        UniqueLock _{this->_lock};

        (void)this->_stream->Write(std::as_bytes(std::span{message, size}));
        (void)this->_stream->Write(std::as_bytes(std::span{"\n", 1}));
    }

    void FileTraceListener::Flush()
    {
        UniqueLock _{this->_lock};

        (void)this->_stream->Flush();
    }
}
