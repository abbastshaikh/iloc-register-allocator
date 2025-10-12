//NAME: Abbas Shaikh
//NETID: ats16

This program provides an implementation of an ILOC register allocator.

To build this program, run `make build`. This will generate an executable, 412alloc, which can be used to run the ILOC register allocator.

There are 3 modes supported:
- `-h`: Prints a help menu.
- `-x <filename>`: Scans and parses the input ILOC block in `<filename>`, then performs renaming on the code in the input block and print the results to the standard output stream.
- `<k> <filename>`: Scans, parses, and allocates the code in the input ILOC block so that it uses only registers r(0) to r(k-1) and print the resulting code to the standard output stream (stdout).