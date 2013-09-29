pecomparator
============

Windows PE executable comparator

Compares two EXE executables (or even DLLs) to check whether they are equal or not regardless the build time. It is very common that you build two EXEs without any source modification and their checksums are different due to the timestamps built in the headers.

This tool is capable of comparing the two binaries and skip the timestamps and checksums present in the executable.

