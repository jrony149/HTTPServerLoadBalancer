#!/bin/bash

docker build -t mts .
docker network create --subnet=10.10.0.0/16 serverNet

VAR=0
VAR2="mts"
VAR3="10.10.0."
VAR4=3

for i; do
        	
	sudo docker run -d -p $i:8085 --net=serverNet --ip="$VAR3$VAR4" --name="$VAR2$VAR" mts ./httpserver 8085 -l logFile
	echo "$i"
	echo "$VAR2$VAR"
	echo "$VAR3$VAR4"
	VAR=$((VAR+1))
	VAR4=$((VAR4+1))
done
