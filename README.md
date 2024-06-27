
# Project Repository

Welcome to our multi-project repository. This repository includes the following projects:
- VPN Installer: A Windows Forms application for managing VPN configurations.
- Ansible Demo: A set of Ansible playbooks and roles for automating various tasks.
- Transformer: A .NET application for data transformation.

## Table of Contents

- [VPN Installer](#vpn-installer)
  - [Features](#features-1)
  - [Prerequisites](#prerequisites-1)
  - [Installation](#installation-1)
  - [Usage](#usage-1)
- [Ansible Demo](#ansible-demo)
  - [Features](#features-2)
  - [Prerequisites](#prerequisites-2)
  - [Setup](#setup-2)
  - [Usage](#usage-2)
- [Transformer](#transformer)
  - [Features](#features-3)
  - [Prerequisites](#prerequisites-3)
  - [Installation](#installation-3)
  - [Usage](#usage-3)
- [C Files](#c-files)
- [C# Files](#csharp-files)
- [ASM Files](#asm-files)
- [VB Files](#vb-files)
- [Contributing](#contributing)


## VPN Installer

A Windows Forms application designed for easy VPN configuration management.

### Features

- User-friendly graphical interface.
- Supports various VPN protocols (OpenVPN, PPTP, L2TP).
- Logging and error handling for better troubleshooting.
- Simple configuration setup and management.

### Prerequisites

- Windows operating system
- Visual Studio 2019 or later
- .NET Framework 4.7.2 or later

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/maxamin/repository.git
   cd repository/VPNInstaller
   ```
2. Open `VPNInstaller.sln` in Visual Studio.
3. Restore the NuGet packages.
4. Build the solution.

### Usage

1. Run the application from Visual Studio or execute the compiled `.exe` file.
2. Follow the on-screen instructions to configure your VPN settings.
3. Connect and disconnect as needed using the provided interface.

## Ansible Demo

A comprehensive set of Ansible playbooks and roles for automating common tasks and exploitation setups.

### Features

- Common role for system setup, package installation, and PowerShell management.
- Exploitation role with tasks for deploying tools such as Bloodhound, Cobalt Strike, Metasploit, and more.

### Prerequisites

- Ansible installed on your control node
- SSH access to the target nodes

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/maxamin/repository.git
   cd repository/ansible-demo
   ```
2. Ensure your Ansible inventory is correctly configured to target the appropriate nodes.

### Usage

1. Run the playbook:
   ```bash
   ansible-playbook playbook.yml
   ```
2. Customize roles and tasks as needed for your specific environment.

## Transformer

A .NET application for performing various data transformation operations.

### Features

- Supports a wide range of data transformation tasks.
- Extensible architecture for adding custom transformation rules.
- High performance and scalability.

### Prerequisites

- Windows operating system
- Visual Studio 2019 or later
- .NET 6.0 SDK

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/maxamin/repository.git
   cd repository/Transformer
   ```
2. Open `Transformer.sln` in Visual Studio.
3. Restore the NuGet packages.
4. Build the solution.

### Usage

1. Run the application from Visual Studio or execute the compiled `.exe` file.
2. Load your data files into the application.
3. Select the desired transformation rules and execute the transformation.
4. Export the transformed data as needed.

## C Files

### Overview

The C files in this repository cover a range of functionalities from system-level programming to utility scripts. Below is a brief description of each file:

- **adduser.c**: Script to add a user to the system.
- **adjuststack.asm**: Assembly code for adjusting stack pointers.
- **ApiResolve.c**: Implementation for API resolution.
- **bof.c**: Buffer overflow example.
- **cavehunter.c**: Example of a code cave hunter.
- **clientpipe.c**: Client-side implementation for named pipes communication.
- **CreateFileSyscall.c**: Example of creating a file using system calls.
- **createremotethread.c**: Example of creating a remote thread.
- **custom_loader.c**: Custom loader for dynamic code execution.
- **dll_hash_calculator.c**: Calculates the hash of a DLL.
- **DllMain.c**: Entry point for a DLL.
- **dump_export.c**: Exports dump data.
- **dynamic_syscall_resolve.c**: Dynamically resolves syscalls.
- **elevate.c**: Elevates process privileges.
- **elevate_bof.c**: Buffer overflow for privilege escalation.
- **enum_window.c**: Enumerates windows on the system.
- **etwbypass.c**: ETW bypass technique.
- **etwkiller.c**: ETW killer script.
- **evasion_obfuscation.c**: Techniques for evasion and obfuscation.
- **firewall_com.c**: Interacts with the Windows Firewall using COM.
- **foreground-window-check.c**: Checks the foreground window.
- **get_namedpipe_ownership.c**: Gets the ownership of a named pipe.
- **get_sedebugprivilege.c**: Obtains SeDebugPrivilege.
- **hooking_dll.c**: Example of a DLL for hooking.
- **http_beacon.c**: Implementation of an HTTP beacon.
- **injector.c**: Code injection example.
- **keylogger-getasynckeystate.c**: Keylogger using GetAsyncKeyState.
- **keylogger-setwindowshookex.c**: Keylogger using SetWindowsHookEx.
- **list-export.c**: Lists exported functions from a DLL.
- **mailslot.c**: Example of a mailslot communication.
- **obfuscated-c-code.c**: Example of obfuscated C code.
- **peb_injector.c**: PEB-based code injection.
- **queueuserapc.c**: Uses QueueUserAPC for code execution.
- **resolve.c**: Resolves function addresses.
- **sandbox_check.c**: Checks if the code is running in a sandbox.
- **serverpipe.c**: Server-side implementation for named pipes communication.
- **SetThreadContextInject.c**: Injects code using SetThreadContext.
- **shellcode_loader_from_file.c**: Loads shellcode from a file.
- **shellcode_runnner.c**: Runs shellcode.
- **string_generator.c**: Generates obfuscated strings.
- **suspend.c**: Suspends and resumes processes.
- **vba_encoder.c**: Encodes payloads for VBA macros.

## C# Files

### Overview

The C# files include various utilities and examples for different purposes:

- **amsibypass.cs**: Bypasses AMSI (Antimalware Scan Interface).
- **azuread_phishing.ps1**: PowerShell script for Azure AD phishing.
- **createremotethread.cs**: Creates a remote thread in a target process.
- **csharp_evasion.cs**: Examples of evasion techniques in C#.
- **execute_dotnet.cs**: Executes .NET code in memory.
- **htmlentities_encoder.cs**: Encodes HTML entities.
- **http_c2.cs**: HTTP command and control implementation.
- **invisible_shellcode.cs**: Executes invisible shellcode.
- **ldap_csharp.cs**: Interacts with LDAP in C#.
- **msbuild_csharp_example.cs**: Example of using MSBuild with C#.
- **net-session-enum.cs**: Enumerates network sessions.
- **obfuscator.py**: Python script for obfuscating C# code.
- **unicode-encoder.cs**: Encodes strings to Unicode.
- **unmanaged_powershell.cs**: Executes PowerShell from unmanaged code.
- **ThunderShell_module_client.cs**: Client module for ThunderShell.

## ASM Files

### Overview

The ASM files include various assembly language scripts:

- **adjuststack.asm**: Assembly code for adjusting stack pointers.
- **dynamically-resolving-shellcode.asm**: Shellcode with dynamic resolving.
- **simple_sysexecve_linux_shellcode.asm**: Simple execve shellcode for Linux.
- **simple-encoder.asm**: Simple encoder in assembly.
- **socket_reuse_sysexecve_linux_shellcode.asm**: Socket reuse execve shellcode for Linux.
- **sysexecve_linux_shellcode.asm**: Execve shellcode for Linux.
- **sysread_write_linux_shellcode.asm**: Read/write shellcode for Linux.

## VB Files

### Overview

The VB files include various VBA scripts for different purposes:

- **com-in-vba.vba**: VBA script for COM interactions.
- **vba_macro_wmi.vba**: VBA macro for WMI interactions.