# 11. Outer join
### Condition
Develop an asynchronous server that performs intersection and symmetric difference operations on sets.

The internal implementation must provide the ability to work with two tables of identical and fixed structure. One table is called A, the other is B, and has the following common structure:
```cpp
{
int id;
std::string name;
}
id is a primary key and cannot be duplicated in one table.
```
For the contents of table A:
```cpp
id | name
---+----------
0 | lean
1 | sweater
2 | frank
3 | violation
4 | quality
5 | precision
```
and the contents of table B:
```cpp
id | name
---+----------
3 | proposal
4 | example
5 | lake
6 | flour
7 | wonder
8 | selection
```
You must be able to form the intersection of data from two tables as a collection:
```cpp
id | A | B
---+-----------+---------
3 | violation | proposal
4 | quality | example
5 | precision | lake
```
You must be able to form the symmetric difference of data from two tables as a collection:
```cpp
id | A | B
---+-----------+---------
0 | lean |
1 | sweater |
2 | frank |
6 | | flour
7 | | wonder
8 | | selection
```
The rows must follow in ascending order of the id field. To replenish the collection and perform operations on sets, the following protocol must be implemented:
```cpp
INSERT table id name
TRUNCATE table
INTERSECTION
SYMMETRIC_DIFFERENCE
```
### Protocol
Commands are sent over the network to the server's listening port. The \n symbol indicates that a command has been completed. In response to any command, the server sends the OK\n sequence in case of success and the ERR message\n in case of an error with decoding.
To form tables from the condition, the following commands will be executed sequentially:
```cpp
INSERT A 0 lean
< OK
INSERT A 0 understand
< ERR duplicate 0
INSERT A 1 sweater
< OK
INSERT A 2 frank
< OK
...
INSERT B 6 flour
< OK
INSERT B 7 wonder
< OK
INSERT B 8 selection
< OK
```
Exactly one space character is used as a separator. The TRUNCATE A and TRUNCATE B commands clear the corresponding tables.
```cpp
TRUNCATE A
< OK
INTERSECTION and SYMMETRIC_DIFFERENCE have identical output format
INTERSECTION
< 3,violation,proposal
< 4,quality,example
< 5,precision,lake
< OK
and
SYMMETRIC_DIFFERENCE
< 0,lean,
< 1,sweater,
< 2,frank,
< 6,,flour
< 7,,wonder
< 8,,selection
< OK
```
### Startup order:
```cpp
join_server <port>
```
where • **port** is the tcp port number for incoming connections. Connections must be serviced from arbitrary interfaces.
### Implementation requirements
The result of the work must be the executable file join_server and must be located in the join_server package. No special efforts should be made to daemonize the server, this means that immediately after starting, the server does not return control until forced termination. The result of the work should be published on bintray.
### Verification
The task is considered successful if, after installing the package and running with test data, the output matches the expectation.
