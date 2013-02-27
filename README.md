URT: Unified Interface to Real-Time Operating Systems
=====================================================

This project strives to create a sane interface to common if not all facilities to the various real-time systems
available.  There are multiple benefits to such an interface.

- **Portability**.  This is perhaps the most important reason.  Currently, most of the software written for one
  real-time system is quite hard to be ported to any other.
- **Simplicity**.  This interface will be designed with the goal of providing high level features and therefore
  doesn't involve the user with many of the low level details.
- **Debuggability**.  Besides the real-time systems, this interface will include a normal linux backend too, allowing
  one to test her software on a non-real-time system.  One benefit of this for example is the ability to use
  `valgrind` and other such software.

State
-----

Currently, the idea of this project is being matured.  Different functionalities of different systems would be added
as tests and their semantics, input and output unified with the same functionality for other systems (if any
implemented).  That is to say, the project is still in a very early stage of development.
