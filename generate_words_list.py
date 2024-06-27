import sys
import random

items = []
words = []

for item in open(sys.argv[1]).readlines():
        items.append(item.strip())

for i in range(0, int(sys.argv[2])):
        current = items[random.randrange(0, len(items))]
        exists = True
        while exists:
                if current in words:
                         current = items[random.randrange(0, len(items))]
                else:
                        exists = False
                        words.append(current)

print words
