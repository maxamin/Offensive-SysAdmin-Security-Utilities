using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace c2
{
    class Program
    {
        static void Main(string[] args)
        {
            Process[] p = Process.GetProcessesByName("notepad");
            if (p.Length > 0)
            {
                // run you code if process is found
            }

            WindowsIdentity identity = WindowsIdentity.GetCurrent();
            if(identity.Name.IndexOf("RINGZER0") != 0)
            {
                // run you code if domain is found in the username
            }
        }
    }
}
