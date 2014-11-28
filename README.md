URT: Unified Interface to Real-Time Operating Systems
=====================================================

This project strives to create a sane interface to common if not all facilities to the various real-time systems
available.  There are multiple benefits to such an interface.

- **Portability**.  This is perhaps the most important reason.  Currently, most of the software written for one
  real-time system is quite hard to be ported to any other.
- **Simplicity**.  This interface will be designed with the goal of providing high level features and therefore
  doesn't involve the user with many of the low level details.
- **Debuggability**.  Besides the real-time systems, this interface will include a normal Linux back-end too, allowing
  one to test her software on a non-real-time system.  One benefit of this for example is the ability to use
  `valgrind` and other such software.

The great POSIX interface addresses the portability issue to a great extent, but is far from simple or regular.
It gives many options which are certainly useful, but for the simpler real-time applications may be overwhelming.

As an example, properly requesting shared memory by itself takes tens of lines and you would eventually wrap it in
a function.  What URT does is essentially just that, in this case in `urt_shmem_new` which is the shared counter-part
to `urt_mem_new` (which is just `malloc`) with equal simplicity.  Another example is irregularities between acquiring
shared semaphores vs other locks.

That aside, some real-time systems such as `RTAI` have a wildly different API too.

State
-----

Currently, the core features of URT have been outlined and implemented for POSIX, and tested under Linux (with no
real-time constraints) as well as under RTAI both in user and kernel spaces. Other operating systems such as RT-Linux,
and tests under QNX and VxWorks are also envisioned.

License
-------

Copyright (C) 2013-2014  Shahbaz Youssefi <ShabbyX@gmail.com>

URT is published under the GNU General Public License
version 2.0 or later (GPL v2.0+).

URT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

URT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with URT.  If not, see <http://www.gnu.org/licenses/>.
