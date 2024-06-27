#!/bin/bash

SHELLCODE=`for i in $(objdump -d $1 -M intel |grep "^ " |cut -f2); do echo -n '\x'$i; done;echo`
echo Shellcode:
echo $SHELLCODE

LENGTH=$(echo $SHELLCODE | wc -c)
echo Shellcode length is $(($LENGTH / 4)) bytes.
