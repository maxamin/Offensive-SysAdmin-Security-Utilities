using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.DirectoryServices;
using System.DirectoryServices.ActiveDirectory;

namespace season3_1
{
    class Program
    {
        static string FormatData(ResultPropertyValueCollection r) {
            StringBuilder sb = new StringBuilder();
            Int32 size = r.Count;
            for(Int32 i = 0; i < size; i++)
            {
                sb.Append(FormatTime(r[i]) + ",");
            }

            return sb.ToString().TrimEnd(',');
        }

        static string FormatTime(object o)
        {
            if(o.GetType().ToString() == "System.Int64")
            {
                return DateTime.FromFileTime((long)o).ToString();
            }
            return o.ToString();
        }
        static void LdapQuery(string domain, string query, string properties) {
            try
            {
                DirectoryEntry de = new DirectoryEntry("LDAP://" + domain);
                DirectorySearcher ds = new DirectorySearcher(de);

                ds.Filter = query;
                ds.PageSize = Int32.MaxValue;

                foreach (SearchResult sr in ds.FindAll())
                {
                    foreach (string p in properties.Split(','))
                    {

                        // current item properties[name] how many names

                        Int32 item = sr.Properties[p].Count;
                        if (item > 0)
                        {
                            Console.WriteLine(String.Format("{0}: {1}", p, FormatData(sr.Properties[p])));
                        }
                        else
                        {
                            Console.WriteLine(String.Format("{0}: empty", p));
                        }
                    }
                    Console.WriteLine("------------------");
                }
            } catch(Exception e)
            {
                Console.WriteLine(String.Format("Error: {0}", e.Message));
            }
        }

        static void Main(string[] args)
        {
            string domain = "";
            string option = "";
            string argument = "";
            try
            {
                // string domain = System.Net.NetworkInformation.IPGlobalProperties.GetIPGlobalProperties().DomainName;
                Domain d = Domain.GetCurrentDomain();
                domain = d.Name;
                Console.WriteLine(String.Format("Querying {0}", domain));
            } catch(Exception e)
            {
                Console.WriteLine(e.Message);
                Console.WriteLine("Domain cannot be reached");
                return;
            }

            try
            {
                option = args[0].ToLower();
                if(args.Length >= 2)
                {
                    argument = args[1];
                }
                
            } catch
            {
                Console.WriteLine("Missing arguments");
                return;
            }

            if(option == "listusers")
            {
                if(argument != "")
                {
                    argument = "(name=*" + argument + "*)";
                }
                string query = "(&(objectClass=user)" + argument + ")";
                Console.WriteLine(String.Format("Using query {0}", query));
                string properties = "name,samaccountname,memberof";
                LdapQuery(domain, query, properties);
            } else if (option == "listcomputers")
            {
                if (argument != "")
                {
                    argument = "(name=*" + argument + "*)";
                }
                string query = "(&(objectClass=computer)" + argument + ")";
                Console.WriteLine(String.Format("Using query {0}", query));
                string properties = "name,displayname,operatingsystem,description,adspath,objectcategory,serviceprincipalname,distinguishedname,cn,lastlogon,managedby,managedobjects";
                LdapQuery(domain, query, properties);
            } else
            {
                Console.WriteLine("Invalid options");
            }
        }
    }
}
