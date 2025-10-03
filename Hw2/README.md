Hw2:

This program is designed to print the file tree

To compile:
    In the directory including Hw2 and makefile, run "make".

Usage:
./Hw2 [<starting directory>] [-S] [-s <size(bytes)>] [-f <string option> <depth>]

Options:
-S
    Each entry is printed with detailed stats (size in bytes, permission, last access time/date)

-s <size(bytes)>
    Only prints entries with size less than specified by the <size(bytes)> argument

-f <string options> <depth>
    Only prints entries that contain the substring <string options> and with depth <= <depth>, root directory has depth 0.