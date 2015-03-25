#! /bin/bash

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

echo "Start: $(date '+%s.%N')"
if [ x"$needs_root" = xy ]; then
  sudo LD_LIBRARY_PATH=$top_builddir/src/.libs PYTHONPATH=$top_builddir/urt-py "$python" test.py
  ret=$?
else
  LD_LIBRARY_PATH=$top_builddir/src/.libs PYTHONPATH=$top_builddir/urt-py "$python" test.py
  ret=$?
fi

echo "Stop:  $(date '+%s.%N')"
if [ x"$needs_ko" = xy ]; then
  sudo rmmod urt
fi
exit $ret
