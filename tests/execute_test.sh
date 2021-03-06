#! /bin/bash

# syntax:
#
#    execute_test.bash [valgrind] test_main main_options [test_side side_count side_options]
#
# - valgrind: tells that valgrind should be run on the tests
# - test_main: the main test application or kernel module
# - main_options: options that need to be sent to test_main.  If testing two processes, main options must end with ::
# - test_side: side test application or kernel module, in case the test is multi-process
# - side_count: number of times test_side should be executed in parallel
# - side_options: similar to main_options, but for the side application
#
# Environment:
#
# - needs_ko: tells that the main urt.ko file should be loaded before executing the test.
# - needs_root: tells that running applications require root access.
# - verbose: tells that the output should not be trimmed to only errors and warnings

# check if doing valgrind check
pre_command=
if [ x"$needs_root" = xy ]; then
	pre_command=sudo
fi

grep_pattern="internal error\\|bad\\|error:\\|wrong\\|fail\\|invalid"
if [ "$1" == valgrind ]; then
	pre_command="$pre_command valgrind"
	grep_pattern="in use at exit:\\|== Invalid\\|== Conditional\\|$grep_pattern"
	shift;
fi

# check if verbose output (for debugging each test manually)
if [ x"$verbose" = xy ]; then
	grep_pattern=""
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
if [ x"$needs_ko" = xy ]; then
	urt_ko="$top_builddir/src/urt.ko"
	if [ ! -f "$urt_ko" ]; then
		echo "Error: URT not built for kernel ('$urt_ko' missing)"
		exit 1
	fi
	if lsmod | grep -q '^urt'; then
		sudo rmmod urt
	fi
	sudo insmod "$urt_ko" || true
fi
echo "Start:         $(date '+%s.%N')"
if $kernel_module; then
	sudo insmod "$test_main" "${main_options[@]}"
else
	($pre_command "$test_main" "${main_options[@]}" 2>&1 | grep --color=never "$grep_pattern"; ret=${PIPESTATUS[0]}) &
fi
echo "Main loaded:   $(date '+%s.%N')"
# if side processes, spawn them too
if $has_side; then
	# give a small delay, so the main process can allocate shared memories and locks, if any
	sleep 1
	for (( i=0; i<$side_count; ++i )); do
		if $kernel_module; then
			sudo insmod "${test_side%.ko}_$i.ko" "${side_options[@]}"
		else
			($pre_command "$test_side" "${side_options[@]}" 2>&1 | grep --color=never "$grep_pattern"; ret2[$i]=${PIPESTATUS[0]}) &
		fi
	done
	echo "Slaves loaded: $(date '+%s.%N')"
fi

if $kernel_module; then
	# can't tell when the test is over.  Most tests don't react on interrupt, so it's ok to
	# go ahead and remove the module (they will wait for test to finish).  For those that do
	# act on interrupt, let's wait a random number of seconds before rmmoding.
	(( r=$RANDOM % 7 + 2 )); sleep $r
else
	wait
fi
echo "Stop:          $(date '+%s.%N')"
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
if [ x"$needs_ko" = xy ]; then
	sudo rmmod urt
fi
exit $ret
