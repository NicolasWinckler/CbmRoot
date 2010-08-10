#!/bin/sh

hnames="lxi009 lxi010 lxi011 lxi012 lxi013 lxi014"

number=0

curdir=`pwd`

for name in $hnames ; do
   echo $name $number
   ssh $name "cd $curdir; ../bin/rocsimul $number -sync 5 >/dev/null 2>&1 &" &
   number=`expr $number + 1`
done