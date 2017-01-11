#!/bin/sh
fifoin="/tmp/eingang"
fifoout="/tmp/ausgang"
tAMB="/tmp/tAMB"
tOBJ="/tmp/tOBJ"
btadr="B0:B4:48:BE:2B:02"

if ! [ -p "$fifoin" ]
then
	mkfifo "$fifoin"
fi

if ! [ -p "$fifoout" ]
then
	mkfifo "$fifoout"
fi

if ! [ -p "$tAMB" ]
then
	mkfifo "$tAMB"
fi

if ! [ -p "$tOBJ" ]
then
	mkfifo "$tOBJ"
fi


gatttool -b "$btadr" -I <> "$fifoin" > "$fifoout" &
./readtag

rm "$fifoin"
rm "$fifoout"
rm "$tAMB"
rm "$tOBJ"
