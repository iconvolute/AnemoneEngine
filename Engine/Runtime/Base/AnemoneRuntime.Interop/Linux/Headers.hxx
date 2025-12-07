#ifndef ANEMONE_PLATFORM_POSIX_H
#define ANEMONE_PLATFORM_POSIX_H

#include "AnemoneRuntime.Interop/Headers.hxx"

#if !(ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID)
#error "This header should only be included on Linux platform"
#endif

#include <arpa/inet.h>
#include <endian.h>
#include <cerrno>
#include <execinfo.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <csignal>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <ucontext.h>
#include <unistd.h>
#include <utime.h>
#include <linux/futex.h>
#include <dirent.h>
#include <sys/wait.h>

#endif // ANEMONE_PLATFORM_POSIX_H
