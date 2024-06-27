using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace class_remote_session_enum
{
    class Program
    {
        [DllImport("netapi32.dll")]
        private static extern int NetSessionEnum([In, MarshalAs(UnmanagedType.LPWStr)] string ServerName, [In, MarshalAs(UnmanagedType.LPWStr)] string UncClientName, [In, MarshalAs(UnmanagedType.LPWStr)] string UserName, Int32 Level, out IntPtr bufptr, int prefmaxlen, ref Int32 entriesread, ref Int32 totalentries,  ref Int32 resume_handle);

        [DllImport("Netapi32.dll")]
        static extern int NetApiBufferFree(IntPtr Buffer);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        struct SESSION_INFO_10
        {
            public string sesi10_cname;
            public string sesi10_username;
            public int sesi10_time;
            public int sesi10_idle_time;
        }

        static void DumpRemoteSessions(string hostname)
        {
            IntPtr bufPtr = IntPtr.Zero;
            int MAX_PREFERRED_LENGTH = -1;
            Int32 entriesread = 0;
            Int32 totalentries = 0;
            Int32 resume_handle = 0;

            NetSessionEnum(hostname, null, null, 10, out bufPtr, MAX_PREFERRED_LENGTH, ref entriesread, ref totalentries, ref resume_handle);
            long offset = bufPtr.ToInt64();

            int SESSION_INFO_10_size = Marshal.SizeOf(typeof(SESSION_INFO_10));

            for(int i = 0; i < entriesread; i++)
            {
                IntPtr nextPtr = new IntPtr(offset);
                SESSION_INFO_10 data = (SESSION_INFO_10)Marshal.PtrToStructure(nextPtr, typeof(SESSION_INFO_10));
                Console.WriteLine("sesi10_cname: {0}", data.sesi10_cname);
                Console.WriteLine("sesi10_username: {0}", data.sesi10_username);

                offset = nextPtr.ToInt64() + SESSION_INFO_10_size;
            }
            NetApiBufferFree(bufPtr);
        }

        static void Main(string[] args)
        {
            string hostname = args[0];
            Console.WriteLine("Dump sessions on {0}", hostname);
            DumpRemoteSessions(hostname);
        }
    }
}
