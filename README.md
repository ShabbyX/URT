URT: Unified Interface to Real-Time Operating Systems
=====================================================

This project strives to create a sane interface to common if not all facilities to the various real-time systems
available.  There are multiple benefits to such an interface.

- **Portability**.  This is perhaps the most important reason.  Currently, most of the software written for one
  real-time system is quite hard to be ported to any other.
- **Simplicity**.  This interface will be designed with the goal of providing high level features and therefore
  doesn't involve the user with many of the low level details.
- **Debuggability**.  Besides the real-time systems, this interface will include a normal Linux backend too, allowing
  one to test her software on a non-real-time system.  One benefit of this for example is the ability to use
  `valgrind` and other such software.

State
-----

Currently, the core features of URT have been outlined and implemented under Linux, with no real-time constraints,
as well as under RTAI both in user and kernel spaces. Other operating systems such as RT-Linux, QNX and VxWorks are
also envisioned.

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
