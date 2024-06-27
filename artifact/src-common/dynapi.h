//unsigned char sKernel32dll[] = "kernel32.dll";
unsigned char sKernel32dll[] = {'k','e','r','n','e','l','3','2','.','d','l','l',0x0};
unsigned char sVirtualProtect[] = {'V','i','r','t','u','a','l','P','r','o','t','e','c','t',0x0};

typedef BOOL (WINAPI *VirtualProtect_t)(LPVOID, SIZE_T, DWORD, PDWORD);


