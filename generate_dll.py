import sys

header = ""
definition = ""

for line in open(sys.argv[1]).readlines():
        header = header + "void DLL_EXPORT %s();\n" % line.strip()
        definition = definition + "void DLL_EXPORT %s() { ExecShellcode(); }\n" % line.strip()

print header
print definition
