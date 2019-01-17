#!/bin/bash

if [ -z $CONFIG ]
then
	source config.sh
fi

curl $gs/$id/5/$js -o $JSONKEYWORDARRAYS
