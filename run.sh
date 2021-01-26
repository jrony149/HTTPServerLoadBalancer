#!/bin/bash

VAR=$#
handler1() { kill $!; }
handler2() { make clean; }
handler3() { cd MTS; }
handler4() { ./stop_remove.sh $VAR; }
handler5() { handler1; handler2; handler3; handler4; }
trap handler5 SIGINT

cd MTS
./run_server_instances.sh "$@"
cd ..
make
./loadbalancer2 -N 4 -R 4 8080 "$@" &
wait
 
