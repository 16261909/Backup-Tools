#!/bin/sh
exe="BackupTools"
des="/home/stevenzhang/Desktop/SE"
deplist=$(ldd $exe|awk '{if (match($3,"/")){printf("%s "),$3}}')
cp $deplist $des
