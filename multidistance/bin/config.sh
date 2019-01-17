#!/bin/bash

ABBYQUOTEORDER=~/Documents/litrev/js/abby_quote_order.js

gs=https://spreadsheets.google.com/feeds/list
id=1tMkdssQlN_wbGS1SjfORS7AOBspKvvun7_AvzxctMrE
js=public/values?alt=json
PROBSENTRY="\"gsx\$probs\""


JSONPROBS=probs.json
JSONKEYWORDARRAYS=keyword_arrays.json
PROBSFILE=probs

ARRPREFIX=arr
ARRDIR=arrs
COUNT=0

DATE=`date`

SORTEDPROBS=sorted_probs
SORTEDID=sorted_id

CONFIG=1