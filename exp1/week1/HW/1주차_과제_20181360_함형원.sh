#!/bin/bash
echo "working directory:"
read dirname

if [ -n "$dirname" ]
then
	cd $dirname
	if [ $? -ne 0 ]
	then
		echo "The directory does not exist or cannot be accessed"
		exit 1
	fi
fi
for dir in *
do
	if [ -f $dir ]
	then
		newname=`echo $dir | tr "[a-z] [A-Z]" "[A-Z] [a-z]"`
		mv $dir $newname
	fi
done
