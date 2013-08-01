#! /bin/bash

# spawn main process
(./$1 2>&1 | grep --color=never 'internal error'; ret=${PIPESTATUS[0]}) &
# if children processes, spawn them too
if [ $# -gt 2 ]; then
	for (( i=0; i<$3; ++i )); do
		(./$2 2>&1 | grep --color=never 'internal error'; ret2[$i]=${PIPESTATUS[0]}) &
	done
fi

wait
# check for output of children processes.  If they were bad, the whole return value becomes bad
if [ $# -gt 2 ]; then
	for (( i=0; i<$3; ++i )); do
		if ! ${ret2[$i]}; then
			$ret=1
		fi
	done
fi
exit $ret
