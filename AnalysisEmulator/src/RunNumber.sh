#!/bin/bash
var=($(ls -d $1/run* | grep -o ....$));
echo ${var[0]};
echo ${var[-1]}
printf -v PARALIST "%s/run%s/parameters.dat" $1 ${var[0]}
wc -l < $PARALIST

