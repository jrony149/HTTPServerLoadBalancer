#!/bin/bash

VAR1="mts"
STOP=$(($1-1))

for i in $(seq 0 $STOP);
do
	VAR2="$i"
	docker stop "$VAR1$VAR2"
	docker rm "$VAR1$VAR2"
done
docker network rm serverNet

