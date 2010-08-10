#!/bin/sh

hnames="lxi009 lxi010 lxi011 lxi012 lxi013 lxi014"

for name in $hnames ; do
   echo $name
   ssh $name killall rocsimul
done