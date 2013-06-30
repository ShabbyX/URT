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

Currently, the core features of URT have been outlined and implemented under Linux, with no real-time constraints.
The development under RTAI is in progress. Other operating systems such as QNX and VxWorks are also envisioned.
