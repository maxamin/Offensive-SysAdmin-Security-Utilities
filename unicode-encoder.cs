using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace unicodeencoder
{
    class Program
    {
        static void Main(string[] args)
        {
            // whatever A \u0041
            byte[] data = File.ReadAllBytes(args[0]);


            Console.WriteLine(Hex(data, "\\u00"));
            Console.WriteLine(Hex(data));
            Console.WriteLine(Hex(data, "\\x"));

        }



        static public string Hex(byte[] data, string prefix = "")
        {
            /*
            StringBuilder sb = new StringBuilder();
            foreach(byte b in data)
            {
                sb.Append(prefix + b.ToString("x2"));
            }
            string output = sb.ToString();
            */

            return prefix + String.Join(prefix, data.Select(b => b.ToString("x2")).ToArray());     
        }
    }
}

