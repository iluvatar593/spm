#!/bin/bash
cd ../src/
icc -mmic -std=c++11 -I../../mc-fastflow-code/ Farm2.cpp -O3 -restrict -vec-report -lpthread -o Farm.out 2>&1
scp ./Farm.out mic0:
#echo -n "starting computation"
#ssh mic0 "./Farm.out $1 $2 $3"
