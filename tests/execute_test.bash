#! /bin/bash

# syntax:
#
#    execute_test.bash [needs_ko] [valgrind] [verbose] test_main main_options [test_side side_count side_options]
#
# - needs_ko: tells that the main urt.ko file should be loaded before executing the test.
# - valgrind: tells that valgrind should be run on the tests
# - verbose: tells that the output should not be trimmed to only errors and warnings
# - test_main: the main test application or kernel module
# - main_options: options that need to be sent to test_main.  They should all be given as one parameter to this script
# - test_side: side test application or kernel module, in case the test is multi-process
# - side_count: number of times test_side should be executed in parallel
# - side_options: similar to main_options, but for the side application

# check if needs main kernel module
needs_ko=false
if [ "$1" == needs_ko ]; then
	needs_ko=true
	shift
fi

# check if doing valgrind check
pre_command=
grep_pattern="internal error\\|bad\\|error:\\|wrong\\|fail\\|invalid"
if [ "$1" == valgrind ]; then
	pre_command=valgrind
	grep_pattern="in use at exit:\\|== Invalid\\|== Conditional\\|""$grep_pattern"
	shift;
fi

# check if verbose output (for debugging each test manually)
if [ "$1" == verbose ]; then
	grep_pattern=""
	shift
fi

# check if executing user application or kernel module
kernel_module=false
if [[ "$1" == *.ko ]]; then
	kernel_module=true;
fi

test_main="$1"
test_side=
side_count=

shift
i=0
while [ $# -gt 0 ] && [ "$1" != "::" ]; do
	main_options[i]="$1"
	((i=i+1))
	shift
done

has_side=false
if [ "$1" == "::" ]; then
	shift
	has_side=true
fi

if $has_side; then
	test_side="$1"
	shift
	side_count="$1"
	shift
	i=0
	while [ $# -gt 0 ]; do
		side_options[i]="$1"
		((i=i+1))
		shift
	done
fi

# spawn main process
urt_ko=
urt=
if $needs_ko; then
	urt_ko="$(find "$(dirname "$0")"/../build/kernel -name "urt*.ko" | head -1)"
	if [ -z "$urt_ko" ]; then
		echo "Error: URT not built for kernel"
		exit 1
	fi
	urt="$(basename "${urt_ko%.ko}")"
	if lsmod | grep -q '^urt'; then
		sudo rmmod "$urt"
	fi
	sudo insmod "$urt_ko" || exit 1
fi
if $kernel_module; then
	sudo insmod "$test_main" "${main_options[@]}"
else
	($pre_command ./"$test_main" "${main_options[@]}" 2>&1 | grep --color=never "$grep_pattern"; ret=${PIPESTATUS[0]}) &
fi
# if side processes, spawn them too
if $has_side; then
	for (( i=0; i<$side_count; ++i )); do
		if $kernel_module; then
			sudo insmod "${test_side%.ko}_$i.ko" "${side_options[@]}"
		else
			($pre_command ./"$test_side" "${side_options[@]}" 2>&1 | grep --color=never "$grep_pattern"; ret2[$i]=${PIPESTATUS[0]}) &
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
# check for output of side processes.  If they were bad, the whole return value becomes bad.
# In kernel tests, unload the modules and ask the user to check dmesg
if $has_side; then
	for (( i=0; i<$side_count; ++i )); do
		if $kernel_module; then
			sudo rmmod "${test_side%.ko}_$i"
		else
			if ! ${ret2[$i]}; then
				$ret=1
			fi
		fi
	done
fi
if $kernel_module; then
	sudo rmmod "${test_main%.ko}"
	echo " *** Please check dmesg for results"
fi
if $needs_ko; then
	sudo rmmod "$urt"
fi
exit $ret
