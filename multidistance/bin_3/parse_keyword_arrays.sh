#!/bin/bash

if [ -z $CONFIG ]
then
	source config.sh
fi

tn=".feed.entry[].$PROBSENTRY.\"\$t\""

jq --arg tn "$tn" "$tn" "$JSONKEYWORDARRAYS" > $JSONPROBS

sed 's/"//g;' $JSONPROBS > $PROBSFILE

