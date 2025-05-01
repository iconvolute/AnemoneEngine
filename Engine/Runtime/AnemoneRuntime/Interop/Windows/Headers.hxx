#ifndef ANEMONE_PLATFORM_WINDOWS_WINDOWSHEADERS_HXX
#define ANEMONE_PLATFORM_WINDOWS_WINDOWSHEADERS_HXX
#pragma once

#include "AnemoneRuntime/Interop/Headers.hxx"

ANEMONE_EXTERNAL_HEADERS_BEGIN

#if !ANEMONE_PLATFORM_WINDOWS
#error "This header should only be included on Windows platform"
#endif

//--------------------------------------------------------------------------------------------------
// Strip system headers from unused symbols.
//

// #define NOCRYPT 1
#define NOATOM 1
// #define NOCLIPBOARD 1
// #define NOCOLOR 1
#define NOCOMM 1
// #define NOCTLMGR 1
#define NODEFERWINDOWPOS 1
#define NODBCS 1
#define NODRAWTEXT 1
// #define NOGDI 1
// #define NOLFILEIO 1
// #define NOLOGERROR 1
#define NOLSTRING 1
#define NOGDIOBJ 1
#define NODRIVERS 1
#define NOMDI 1
#define NOGDICAPMASKS 1
#define NOBITMAP 1
#define NOHELP 1
// #define NOICONS 1
#define NOIMAGE 1
#define NOIME 1
#define NOKANJI 1
#define NOKERNEL 1
// #define NOKEYSTATES 1
#define NOKEYBOARDINFO 1
// #define NOMB 1
#define NOMCX 1
#define NOMEMMGR 1
// #define NOMENUS 1
#define NOMETAFILE 1
#define NOMINMAX 1
// #define NOMSG 1
// #define NONLS 1
#define NOOPENFILE 1
#define NOPROFILER 1
#define NOPROXYSTUB 1
#define NORASTEROPS 1
#define NORPC 1
#define NOSCROLL 1
#define NOSERVICE 1
// define NOSHOWWINDOW 1
#define NOSOUND 1
// #define NOSYSCOMMANDS 1
// #define NOSYSMETRICS 1
#define NOTAPE 1
#define NOTEXTMETRIC 1
// #define NOUSER 1
// #define NOVIRTUALKEYCODES 1
#define NOWH 1
// #define NOWINMESSAGES 1
// #define NOWINOFFSETS 1
// #define NOWINSTYLES 1
#define NOWINRES
#define OEMRESOURCE 1
// #define NOLANGUAGE 1
// #define NORESOURCE 1
// #define NOSCALABLEFONT 1
// #define NOSYSTEMPARAMSINFO 1

#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1

#define ISOLATION_AWARE_ENABLED 1

#ifndef STRICT
#define STRICT 1
#endif

// #if false
// #define _CRTDBG_MAP_ALLOC
// #ifdef _DEBUG
// #define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
// #define new DEBUG_NEW
// #endif
// #endif
//
// #include <crtdbg.h>


//
// Include system configurable headers.
//

#include <SDKDDKVer.h>
#include <intrin.h>


//
// Make sure that build system properly configured headers
//

#if !defined(NTDDI_VERSION)
#error "Unknown DDI version"
#endif

#if !defined(WINVER)
#error "Unknown WINVER version"
#endif

#if !defined(_WIN32_WINNT)
#error "Unknown _WIN32_WINNT version"
#endif

#if defined(_MSC_VER) && defined(NDEBUG)
#define _SECURE_SCL 0
#define _SECURE_SCL_THROWS 0
#endif


//
// Windows headers
//

// won't compile here?
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <winnt.h>
// #include <WinBase.h>
// #include <Windows.h>
// #include <fileapi.h>
#include <afunix.h>
#include <cerrno>
#include <cmath>

#if defined(_MSC_VER) && !defined(__clang__)
#pragma intrinsic(_lrotl, _lrotr, _rotl, _rotr, _strset, abs, fabs, labs, memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen)
#pragma intrinsic(acos, cosh, pow, tanh, asin, fmod, sinh)
#pragma intrinsic(atan, exp, log10, sqrt, atan2, log, sin, tan, cos)
#pragma intrinsic(_rotr8, _rotr16, _rotr, _rotr64)
#pragma intrinsic(_rotl8, _rotl16, _rotl, _rotl64)

#if defined(_M_X64)
#pragma intrinsic(__rdtsc)
#endif
#endif

//--------------------------------------------------------------------------------------------------
// Undefine macros that are defined by Windows headers.
//

// begin: fileapi.h
#undef CreateDirectory
#undef CreateFile
#undef DefineDosDevice
#undef DeleteFile
#undef DeleteVolumeMountPoint
#undef FindFirstChangeNotification
#undef FindFirstFile
#undef FindFirstFileEx
#undef FindFirstVolume
#undef FindNextFile
#undef FindNextVolume
#undef GetDiskFreeSpace
#undef GetDiskFreeSpaceEx
#undef GetDriveType
#undef GetFileAttributes
#undef GetFileAttributesEx
#undef GetFinalPathNameByHandle
#undef GetFullPathName
#undef GetLogicalDriveStrings
#undef GetLongPathName
#undef GetShortPathName
#undef GetTempFileName
#undef GetVolumeInformation
#undef GetVolumePathName
#undef QueryDosDevice
#undef RemoveDirectory
#undef SetFileAttributes
#undef GetTempPath
#undef GetVolumeNameForVolumeMountPoint
#undef GetVolumePathNamesForVolumeName
#undef GetCompressedFileSize
// end: fileapi.h

// begin: winnt.h
// #undef MemoryBarrier -- this one is actually preety neat
#undef YieldProcessor
// end: winnt.h

// begin: WinBase.h
#undef Yield
#undef GetBinaryType
#undef GetLongPathNameTransacted
#undef SetFileShortName
#undef FormatMessage
#undef CreateMailslot
#undef EncryptFile
#undef DecryptFile
#undef FileEncryptionStatus
#undef OpenEncryptedFileRaw
#undef CreateSemaphore
#undef CreateWaitableTimer
#undef CreateFileMapping
#undef CreateFileMappingNuma
#undef OpenFileMapping
#undef LoadLibrary
#undef QueryFullProcessImageName
#undef FatalAppExit
#undef GetStartupInfo
#undef GetFirmwareEnvironmentVariable
#undef GetFirmwareEnvironmentVariableEx
#undef SetFirmwareEnvironmentVariable
#undef SetFirmwareEnvironmentVariableEx
#undef FindResource
#undef FindResourceEx
#undef EnumResourceTypes
#undef EnumResourceNames
#undef EnumResourceLanguages
#undef BeginUpdateResource
#undef UpdateResource
#undef EndUpdateResource
#undef GlobalAddAtom
#undef GlobalAddAtomEx
#undef GlobalFindAtom
#undef GlobalGetAtomName
#undef AddAtom
#undef FindAtom
#undef GetAtomName
#undef GetProfileInt
#undef GetProfileString
#undef WriteProfileString
#undef GetProfileSection
#undef WriteProfileSection
#undef GetPrivateProfileInt
#undef GetPrivateProfileString
#undef WritePrivateProfileString
#undef GetPrivateProfileSection
#undef WritePrivateProfileSection
#undef GetPrivateProfileSectionNames
#undef GetPrivateProfileStruct
#undef WritePrivateProfileStruct
#undef GetSystemWow64Directory
#undef SetDllDirectory
#undef GetDllDirectory
#undef CreateDirectoryEx
#undef CreateDirectoryTransacted
#undef RemoveDirectoryTransacted
#undef GetFullPathNameTransacted
#undef CreateFileTransacted
#undef SetFileAttributesTransacted
#undef GetFileAttributesTransacted
#undef GetCompressedFileSizeTransacted
#undef DeleteFileTransacted
#undef CheckNameLegalDOS8Dot3
#undef FindFirstFileTransacted
#undef CopyFile
#undef CopyFileEx
#undef CopyFileTransacted
#undef MoveFile
#undef MoveFileEx
#undef MoveFileWithProgress
#undef MoveFileTransacted
#undef ReplaceFile
#undef CreateHardLink
#undef CreateHardLinkTransacted
#undef CreateNamedPipe
#undef GetNamedPipeHandleState
#undef CallNamedPipe
#undef WaitNamedPipe
#undef GetNamedPipeClientComputerName
#undef SetVolumeLabel
#undef ClearEventLog
#undef BackupEventLog
#undef OpenEventLog
#undef RegisterEventSource
#undef OpenBackupEventLog
#undef ReadEventLog
#undef ReportEvent
#undef AccessCheckAndAuditAlarm
#undef AccessCheckByTypeAndAuditAlarm
#undef AccessCheckByTypeResultListAndAuditAlarm
#undef AccessCheckByTypeResultListAndAuditAlarmByHandle
#undef ObjectOpenAuditAlarm
#undef ObjectPrivilegeAuditAlarm
#undef ObjectCloseAuditAlarm
#undef ObjectDeleteAuditAlarm
#undef PrivilegedServiceAuditAlarm
#undef GetFileSecurity
#undef IsBadStringPtr
#undef LookupAccountSid
#undef LookupAccountName
#undef LookupAccountNameLocal
#undef LookupAccountSidLocal
#undef LookupPrivilegeValue
#undef LookupPrivilegeName
#undef LookupPrivilegeDisplayName
#undef BuildCommDCB
#undef BuildCommDCBAndTimeouts
#undef CommConfigDialog
#undef GetDefaultCommConfig
#undef SetDefaultCommConfig
#undef GetComputerName
#undef SetComputerName
#undef DnsHostnameToComputerName
#undef GetUserName
#undef LogonUser
#undef LogonUserEx
#undef CreateProcessAsUser
#undef CreatePrivateNamespace
#undef OpenPrivateNamespace
#undef CreateBoundaryDescriptor
#undef GetCurrentHwProfile
//#undef VerifyVersionInfo
#undef CreateJobObject
#undef OpenJobObject
#undef FindFirstVolumeMountPoint
#undef FindNextVolumeMountPoint
#undef SetVolumeMountPoint
#undef CreateActCtx
#undef FindActCtxSectionString
#undef CreateSymbolicLink
#undef CreateSymbolicLinkTransacted
// end: WinBase.h

// begin: WinUser.h
#undef LoadKeyboardLayout
#undef GetKeyboardLayoutName
#undef CreateDesktop
#undef CreateDesktopEx
#undef OpenDesktop
#undef EnumDesktops
#undef CreateWindowStation
#undef OpenWindowStation
#undef EnumWindowStations
#undef GetUserObjectInformation
#undef SetUserObjectInformation
#undef RegisterWindowMessage
#undef GetMessage
#undef DispatchMessage
#undef PeekMessage
#undef SendMessage
#undef SendMessageTimeout
#undef SendNotifyMessage
#undef SendMessageCallback
#undef BroadcastSystemMessageEx
#undef BroadcastSystemMessage
#undef RegisterDeviceNotification
#undef PostMessage
#undef PostThreadMessage
#undef PostAppMessage
#undef DefWindowProc
#undef CallWindowProc
#undef RegisterClass
#undef UnregisterClass
#undef GetClassInfo
#undef RegisterClassEx
#undef GetClassInfoEx
#undef CreateWindowEx
#undef CreateWindow
#undef CreateDialogParam
#undef CreateDialogIndirectParam
#undef CreateDialog
#undef CreateDialogIndirect
#undef DialogBoxParam
#undef DialogBoxIndirectParam
#undef DialogBox
#undef DialogBoxIndirect
#undef SetDlgItemText
#undef GetDlgItemText
#undef SendDlgItemMessage
#undef DefDlgProc
#undef CallMsgFilter
#undef RegisterClipboardFormat
#undef GetClipboardFormatName
#undef CharToOem
#undef OemToChar
#undef CharToOemBuff
#undef OemToCharBuff
#undef CharUpper
#undef CharUpperBuff
#undef CharLower
#undef CharLowerBuff
#undef CharNext
#undef CharPrev
#undef IsCharAlpha
#undef IsCharAlphaNumeric
#undef IsCharUpper
#undef IsCharLower
#undef GetKeyNameText
#undef VkKeyScan
#undef VkKeyScanEx
#undef MapVirtualKey
#undef MapVirtualKeyEx
#undef LoadAccelerators
#undef CreateAcceleratorTable
#undef CopyAcceleratorTable
#undef TranslateAccelerator
#undef LoadMenu
#undef LoadMenuIndirect
#undef ChangeMenu
#undef GetMenuString
#undef InsertMenu
#undef AppendMenu
#undef ModifyMenu
#undef InsertMenuItem
#undef GetMenuItemInfo
#undef SetMenuItemInfo
#undef DrawText
#undef DrawTextEx
#undef GrayString
#undef DrawState
#undef TabbedTextOut
#undef GetTabbedTextExtent
#undef SetProp
#undef GetProp
#undef RemoveProp
#undef EnumPropsEx
#undef EnumProps
#undef SetWindowText
#undef GetWindowText
#undef GetWindowTextLength
#undef MessageBox
#undef MessageBoxEx
#undef MessageBoxIndirect
#undef GetWindowLong
#undef SetWindowLong
#undef GetWindowLongPtr
#undef SetWindowLongPtr
#undef GetClassLong
#undef SetClassLong
#undef GetClassLongPtr
#undef SetClassLongPtr
#undef FindWindow
#undef FindWindowEx
#undef GetClassName
#undef SetWindowsHook
#undef SetWindowsHookEx
#undef LoadBitmap
#undef LoadCursor
#undef LoadCursorFromFile
#undef LoadIcon
#undef PrivateExtractIcons
#undef LoadImage
#undef GetIconInfoEx
#undef LoadString
#undef IsDialogMessage
#undef DlgDirList
#undef DlgDirSelectEx
#undef DlgDirListComboBox
#undef DlgDirSelectComboBoxEx
#undef DefFrameProc
#undef DefMDIChildProc
#undef CreateMDIWindow
#undef WinHelp
#undef ChangeDisplaySettings
#undef ChangeDisplaySettingsEx
#undef EnumDisplaySettings
#undef EnumDisplaySettingsEx
#undef EnumDisplayDevices
#undef SystemParametersInfo
#undef GetMonitorInfo
#undef GetWindowModuleFileName
#undef GetAltTabInfo
#undef GetRawInputDeviceInfo
// end: WinUser.h

// begin: processenv.h
#undef GetEnvironmentStrings
#undef SetEnvironmentStrings
#undef FreeEnvironmentStrings
#undef GetCommandLine
#undef GetEnvironmentVariable
#undef SetEnvironmentVariable
#undef ExpandEnvironmentStrings
#undef SetCurrentDirectory
#undef GetCurrentDirectory
#undef SearchPath
#undef NeedCurrentDirectoryForExePath
// end: processenv.h

// begin: winreg.h
#undef RegConnectRegistry
#undef RegConnectRegistryEx
#undef RegCreateKey
#undef RegCreateKeyEx
#undef RegCreateKeyTransacted
#undef RegDeleteKey
#undef RegDeleteKeyEx
#undef RegDeleteKeyTransacted
#undef RegDeleteValue
#undef RegEnumKey
#undef RegEnumKeyEx
#undef RegEnumValue
#undef RegLoadKey
#undef RegOpenKey
#undef RegOpenKeyEx
#undef RegOpenKeyTransacted
#undef RegQueryInfoKey
#undef RegQueryValue
#undef RegQueryMultipleValues
#undef RegQueryValueEx
#undef RegReplaceKey
#undef RegRestoreKey
#undef RegSaveKey
#undef RegSetValue
#undef RegSetValueEx
#undef RegUnLoadKey
#undef RegDeleteKeyValue
#undef RegSetKeyValue
#undef RegDeleteTree
#undef RegCopyTree
#undef RegGetValue
#undef RegCopyTree
#undef RegLoadMUIString
#undef RegLoadAppKey
#undef InitiateSystemShutdown
#undef AbortSystemShutdown
#undef InitiateSystemShutdownEx
#undef InitiateShutdown
#undef RegSaveKeyEx
// end: winreg.h

// begin: shellapi.h
#undef DragQueryFile
#undef ShellExecute
#undef FindExecutable
#undef ShellAbout
#undef ExtractIconEx
#undef SHFileOperation
#undef ShellExecuteEx
#undef SHQueryRecycleBin
#undef SHEmptyRecycleBin
#undef Shell_NotifyIcon
#undef SHGetFileInfo
#undef SHGetDiskFreeSpaceEx
#undef SHInvokePrinterCommand
#undef ShellMessageBox
#undef IsLFNDrive
#undef SHEnumerateUnreadMailAccounts
#undef SHGetUnreadMailCount
#undef SHSetUnreadMailCount
// end: shellapi.h

// begin: sysinfoapi.h
#undef GetSystemDirectory
#undef GetWindowsDirectory
#undef GetSystemWindowsDirectory
#undef GetComputerNameEx
#undef SetComputerNameEx
#undef GetVersionEx
// end: sysinfoapi.h

// begin: libloaderapi.h
#undef GetModuleFileName
#undef GetModuleHandle
#undef GetModuleHandleEx
#undef LoadLibraryEx
#undef LoadString
#undef EnumResourceLanguagesEx
#undef EnumResourceNamesEx
#undef EnumResourceTypesEx
// end: libloaderapi.h

// begin: synchapi.h
#undef CreateMutex
#undef OpenMutex
#undef CreateEvent
#undef OpenEvent
#undef OpenSemaphore
#undef OpenWaitableTimer
#undef CreateMutexEx
#undef CreateEventEx
#undef CreateSemaphoreEx
#undef CreateWaitableTimerEx
// end: synchapi.h

// begin: winspool.h
#undef SetPort
// end: winspool.h

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

ANEMONE_EXTERNAL_HEADERS_END

#endif // ANEMONE_PLATFORM_WINDOWS_WINDOWSHEADERS_HXX
