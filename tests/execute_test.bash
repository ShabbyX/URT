#! /bin/bash

# check if doing valgrind check
pre_command=
grep_pattern="internal error\\|bad\\|error\\|wrong\\|fail"
if [ "$1" == valgrind ]; then
	pre_command=valgrind
	grep_pattern="in use at exit:\\|== Invalid\\|""$grep_pattern"
	shift;
fi

# check if executing user application or kernel module
kernel_module=false
if [[ "$1" == *.ko ]]; then
	kernel_module=true;
fi

# spawn main process
if $kernel_module; then
	if lsmod | grep -q urt; then
		sudo rmmod urt
	fi
	sudo insmod "$(dirname "$0")"/../build/kernel/urt.ko || exit 1
	sudo insmod "$1"
else
	($pre_command ./"$1" 2>&1 | grep --color=never "$grep_pattern"; ret=${PIPESTATUS[0]}) &
fi
# if children processes, spawn them too
if [ $# -gt 2 ]; then
	for (( i=0; i<$3; ++i )); do
		if $kernel_module; then
			sudo insmod "$2_$i"
		else
			($pre_command ./"$2" 2>&1 | grep --color=never "$grep_pattern"; ret2[$i]=${PIPESTATUS[0]}) &
		fi
	done
fi

if $kernel_module; then
	# can't tell when the test is over.  Most tests don't react on interrupt, so it's ok to
	# go ahead and remove the module (they will wait for test to finish).  For those that do
	# act on interrupt, let's wait a random number of seconds before rmmoding.
	(( r=$RANDOM % 7 + 2 )); sleep $r
else
	wait
fi
# check for output of children processes.  If they were bad, the whole return value becomes bad
# in kernel tests, unload the modules and ask the user to check dmesg
if [ $# -gt 2 ]; then
	for (( i=0; i<$3; ++i )); do
		if ! ${ret2[$i]}; then
			if $kernel_module; then
				sudo rmmod "$2_$i"
			else
				$ret=1
			fi
		fi
	done
fi
if $kernel_module; then
	sudo rmmod "$1"
	sudo rmmod urt
	printf -- " *** Please check dmesg for results\n"
fi
exit $ret
