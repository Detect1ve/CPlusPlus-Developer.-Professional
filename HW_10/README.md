# 10. Asynchronous network command processing
The goal is to rework Task 9 so that data input is performed over the network.

It is necessary to develop an asynchronous server that works with a protocol similar to the console. The server accepts commands line by line.

Further processing remains unchanged - output to the console and files, static blocks, and dynamically sized blocks.

The Boost.ASIO library should be used as a network interaction library (we use asynchronous capabilities).

Since it is not supposed to send data from the server to the client in response to commands, the main interaction will be around the **async_read** and **async_accept** methods.

Start order:
```cpp
# bulk_server <port> <bulk_size>
```
where
- **port** - is the tcp port number for incoming connections. Connections should be serviced from arbitrary interfaces
- **bulk_size** – is the already familiar command block size.

There should be no artificial limits on the number of simultaneously connected clients. Commands from static blocks received from different connections should be mixed. However, commands from dynamic blocks (delimited by the { and } symbols) from different connections should not be mixed.

Example of running commands:
```cpp
bulk_server 9000 3
```
and
```cpp
seq 0 9 | nc localhost 9000
```
(which means generating 10 lines with numbers from 0 to 9 on each line and sending them over the network to local port 9000).

Running the commands should result in the console displaying:
```cpp
bulk: 0, 1, 2
bulk: 3, 4, 5
bulk: 6, 7, 8
bulk: 9
```
and generating four files with content similar to each line.

Running two commands simultaneously (for example, from two different consoles) with a previously running server with bulk_size == 3:
```cpp
seq 0 9 | nc localhost 9000
```
and
```cpp
seq 10 19 | nc localhost 9000
```
(which means generating 20 lines with numbers from 0 to 19 on each line and sending them over the network to local port 9000 from different connections) should result in the console:
```cpp
bulk: 0, 1, 10
bulk: 2, 3, 11
bulk: 4, 5, 12
bulk: 6, 7, 13
bulk: 8, 9, 14
bulk: 15, 16, 17
bulk: 18, 19
```
and generating seven files with content similar to each line.

The exact way the commands from different sources are mixed depends on the runtime conditions and may vary from run to run.
### Implementation requirements
The result of the work should be the executable file **bulk_server**, which is located in the **bulk_server** package. There is no need to make special efforts to daemonize the server - immediately after launch, the server may not return control until forced termination.

**The result of the work should be published on bintray.**
### Verification
The task is considered successful if, after installing the package and running with test data, the output matches the description [**Task 9**](../HW_9/README.md). The ability to not lose commands received immediately before the client closes the connection will be noted.
