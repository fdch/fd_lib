#!/bin/bash

source config.sh

source get_keyword_arrays.sh

source parse_keyword_arrays.sh

source convert_to_pd_arrays.sh

echo $COUNT > array_count.txt

source sort_em_all.sh #1 to make it reverse sort

source abby_quote_order.sh
# rm $JSONKEYWORDARRAYS
# rm $JSONPROBS

exit 0
