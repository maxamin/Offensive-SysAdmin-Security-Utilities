import sys

data = open(sys.argv[1]).read()

for i in range(0, 256):
        if data.find(chr(i)) == -1:
                print "%s %s was not found" % (str(i), hex(i))
