// gcc .\createlocaluser.c -o create.exe -lnetapi32
#include <windows.h>
#include <lm.h>
#include <stdio.h>

// create a local admin
VOID GenString(WCHAR **output, DWORD dwSize, DWORD dwKeySpace) {
    CHAR charset[] = "abcdefghijlkmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@$?&*()";
    CHAR *current = (CHAR*)GlobalAlloc(GPTR, dwSize + 1);
    DWORD i = 0;
    for(i; i < dwSize; i++) {
        current[i] = charset[rand() % dwKeySpace];
    }

    WCHAR *wCurrent = (WCHAR*)GlobalAlloc(GPTR, (dwSize * 2) + 2);
    swprintf(wCurrent, dwSize, L"%hs", current);
    *output = wCurrent;

    GlobalFree(current);
}

int main() {
    srand(GetCurrentProcessId());
    WCHAR *username = NULL;
    WCHAR *password = NULL;
    USER_INFO_1 ui;
    DWORD dwError = 0;
    GenString(&username, 12, 26);
    GenString(&password, 12, 71);
    printf("Username is: %ls\n", username);
    printf("Password is: %ls\n", password);
 
    ui.usri1_name = username;
    ui.usri1_password = password;
    ui.usri1_priv = USER_PRIV_USER;
    ui.usri1_flags = UF_DONT_EXPIRE_PASSWD;
    ui.usri1_home_dir = NULL;
    ui.usri1_comment = NULL;
    ui.usri1_script_path = NULL;

    NET_API_STATUS status;
    status = NetUserAdd(NULL, 1, (BYTE*)&ui, &dwError);
    if(status != NERR_Success) {
        printf("NetUserAdd failed. Error: %d\n", status);
    }

    LOCALGROUP_MEMBERS_INFO_3 lmi;
    lmi.lgrmi3_domainandname = username;
    status = NetLocalGroupAddMembers(NULL, L"Administrators", 3, (BYTE*)&lmi, 1);
    if(status != NERR_Success) {
        printf("NetLocalGroupAddMembers failed. Error: %d\n", status);
    }
    
    return 0;
}
