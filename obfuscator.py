import sys
import random
import re

data = open(sys.argv[1]).read()

final = ""

for c in data:
        final = final + c + (chr(int(sys.argv[2])) * random.randrange(10, 50))

print "\\x" + "\\x".join(re.findall("..", final.encode("hex")))
