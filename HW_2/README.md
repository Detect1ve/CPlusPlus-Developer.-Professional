# 2. IP address filtering
**The goal of the task is to implement an application to process a list of IP addresses.**

The program reads data from standard input. The data is stored line by line. Each line consists of three fields separated by one tab character and ends with an end-of-line character.

**Line format:**
```
text1 \t text2 \t text3 \n
```
Fields text2 and text3 are ignored. Field text1 has the following structure (ip4 address):
```
n1.n2.n3.n4
```
where n1..4 is an integer from 0 to 255.

It is required to load a list of IP addresses into memory and sort them in reverse lexicographic order. An example of lexicographic sorting (by the first number, then by the second, and so on):
```
1.1.1.1
1.2.1.1
1.10.1.1
```
Accordingly, the reverse:
```
1.10.1.1
1.2.1.1
1.1.1.1
```
💡 Please note that sorting is performed in the byte (numeric) representation of the IP address, not in the string representation. Therefore, the address, for example, 1.2.1.1 is less than the address 1.10.1.1 (after all, 2 < 10).

**Next, we output the following to the standard output:**

1. The full list of addresses after sorting. One line - one address.
2. Immediately after that, it is a list of addresses whose first byte is equal to 1. The sorting order does not change. One line - one address. The lists are not separated by anything.
3. The list of addresses continues immediately, with the first byte equal to 46 and the second to 70. The sorting order does not change. One line - one address. The lists are not separated by anything.
4. The list of addresses continues immediately, any byte of which is equal to 46. The sorting order does not change. One line - one address. The lists are not separated by anything.
### *Implementation requirements*
In the source file attached to the task, it is necessary to replace, where possible, the constructions with similar ones from the C++14/C++17 standard. Implement the missing functionality.

It is not necessary to follow the attached template. You can complete the task by formatting the code in any correct way.
### *Double check*
1. lexicographic sorting is understandable for both the string and the container
2. the data structure corresponding to the task is selected
### *Self-control*
A source code layout, as well as a test file with the ip_filter.tsv data, are attached to the lesson materials. You can check yourself as follows (Linux):
```
cat ip_filter.tsv | ip_filter | md5sum
24e7a7b2270daee89c64d3ca5fb3da1a -
```
💡 Attention! Due to the different interpretation of the ‘\n’ symbol on Linux and Windows, the above hash is relevant only for Linux systems. If your working system is Windows, come up with a way to check the implementation, having a reference result only for Linux.

The `ip_filter` package containing the `ip_filter` executable file must be published as a release in the repository.
- **Additional exercise**

  Add unit tests for the implementation using the test framework of your choice (GoogleTest is recommended).
