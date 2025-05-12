# 8. Boost
## bayan
Create a useful utility for quickly searching for duplicate files by content, practicing using the Boost library.

### Condition

Using the structures and algorithms available in the Boost library, develop a utility for detecting duplicate files.

The utility should be able to specify via command line parameters

- directories to scan (there may be several)
- directories to exclude from scanning (there may be several)
- scanning level (one for all directories, 0 - only the specified directory without nested ones)
- minimum file size, by default, all files larger than 1 byte are checked.
- masks of file names allowed for comparison (case-insensitive)
- the block size used to read files, in the task, this size is referred to as S
- one of the existing hashing algorithms (crc32, md5 - determine specific options yourself), in the task, this function is referred to as H

The result of the utility should be a list of full paths of files with identical contents, output to standard output. One file per line. Identical files must be placed in a row, in one group. An empty line separates different groups.

A mandatory feature of the utility is the careful handling of disk input/output. Each file can be represented as a list of blocks of size S. If the file size is not a multiple of S, it is padded with binary zeros.

One line `world.txt` file
```
Hello, World\n
```
With a block size of 5 bytes, it will be represented as
```
Hello
, Wor
ld\n\0\0
```
The chosen hashing function must calculate each block. Possible collisions are ignored. Assuming that
```
H("Hello") == A
H(", Wor") == B
H("ld\n\0\0") == C
```
`world.txt` file can be represented as a sequence of ABC

Let's look at the second file, `cpp.txt`
```
Hello, C++\n
```
Which, after hashing the blocks
```
H("Hello") == A
H(", C++") == D
H(\n\0\0\0\0") == E
```
can be represented as a sequence of ADE

The order of comparing these files should be as careful as possible. That is, processing the first file `world.txt` does not result in reading from disk at all, because we have nothing to compare with yet. As soon as we get to the file `cpp.txt`, only at this moment does the first reading of the first block of both files occur. In this case, the blocks are identical, and it is necessary to read the second block, which is already different. The files are different, and the remaining data is not read. Files are considered identical if their sequence of block hashes matches exactly.

### Self-control

- a file block is read from disk no more than once
- the file block is read only when necessary
- don't forget that there can be more than two duplicates
- package `bayan` containing executable `bayan` is published on `bintray`
- description of parameters in the file [README.md](http://readme.md/) of the repository root
- link to repository page sent for verification

### Verification

The task is considered completed successfully if, after viewing the code, connecting the repository, installing the package, and running the binary file with the command (parameters from the description):
```
$ bayan \[...\]
```
Duplicate files will be detected without false positives and missing existing duplicates. The amount of data read from the disk is minimal.
