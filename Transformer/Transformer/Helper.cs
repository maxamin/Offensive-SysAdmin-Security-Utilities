using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Transformer
{
    internal class Helper
    {
        private static Random random = new Random((int)DateTimeOffset.Now.ToUnixTimeMilliseconds());
        public static EncoderFile ReadFile(string path)
        {
            StreamReader sr = new StreamReader(path);
            return JsonSerializer.Deserialize<EncoderFile>(sr.ReadToEnd());
        }

        public static byte[] CallEncoder(string encoderPath, byte[] data, TextBox log)
        {
            foreach(string file in Directory.GetFiles("encoders/"))
            {
                if(file == $"encoders/{encoderPath}.dll")
                {
                    string fullpath = $"{Directory.GetCurrentDirectory()}\\{file}";
                    Assembly assembly = Assembly.LoadFile(fullpath);
                    Type type = assembly.GetExportedTypes()[0];
                    dynamic instance = Activator.CreateInstance(type);
                    return instance.Run(data);
                }
            }

            log.AppendText($"{encoderPath} was not found.\r\n");
            return data;
        }

        public static string GenerateRandomString(int size)
        {
            string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
            char[] outputChars = new char[size];
            for(int i = 0; i < outputChars.Length; i++)
            {
                if(i == 0)
                {
                    outputChars[i] = charset[random.Next(charset.Length - 10)];
                } else
                {
                    outputChars[i] = charset[random.Next(charset.Length)];
                }
            }
            return new string(outputChars);
        }

        public static string TransformVariable(string delimeter, string data)
        {
            Regex regex = new Regex($"{delimeter}(.*?){delimeter}");
            foreach(Match match in regex.Matches(data))
            {
                data = data.Replace(match.Value, GenerateRandomString(random.Next(6, 16)));
            }
            return data;

        }

    }
}
