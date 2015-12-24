#!/bin/sh
m=$1
rawdat=$2
deleteddat=`echo ${rawdat} | sed -e "s/\.dat/_${m}.dat/"`
outputwrl=`echo ${rawdat} | sed -e "s/\.dat/_${m}.wrl/"`

./delete2 ${rawdat} ${m} > ${deleteddat}
./MyMain ${rawdat} ${deleteddat} > ${outputwrl}
