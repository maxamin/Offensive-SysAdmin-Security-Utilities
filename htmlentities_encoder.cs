using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace msbuilddata
{
    class Program
    {
        static void Main(string[] args)
        {
            byte[] data = File.ReadAllBytes(args[0]);
            string encoded = Encoder(data);
            Console.WriteLine(Encoder(Encoding.ASCII.GetBytes(encoded)));
        }


        static string Encoder(byte[] data)
        {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in data)
            {
                sb.Append(String.Format("&#x{0};", b.ToString("x2")));
            }
            return sb.ToString();
        }
    }
}
