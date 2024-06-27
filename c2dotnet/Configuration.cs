using System.Text.Json;

namespace c2dotnet
{
    public class C2Configuration
    {
        public bool Debug { get; set; }
        public string[] AgentListeners { get; set; }
        public string ServerResponseServer { get; set; }

        public string ServerApiPath { get; set; }
        public static C2Configuration ParseConfig(string path)
        {
            using(StreamReader sr = new StreamReader(path))
            {
                try
                {
                    C2Configuration c2configuration = JsonSerializer.Deserialize<C2Configuration>(sr.ReadToEnd());
                    sr.Close();
                    return c2configuration;
                } catch(Exception e)
                {
                    Console.WriteLine($"Could not load {path}. Error: {e.Message}");
                    Environment.Exit(0);
                }
            }
            return new C2Configuration();
        }
    }


}
