# 9. Multithreaded asynchronous command processing

## Condition

Generalize the code from the task about batch command processing, providing two main goals:

1. multithreaded command processing;
1. decomposition of the utility into a library and a client part (data input was controlled by external code).

## 1. Multithreaded command processing

When the program starts, three additional threads should be created in addition to the existing main thread.

Let's give them conventional names:

- **main** - the main thread (in the user application)
- **log** - a thread for outputting data to the console
- **file1** - the first thread for output to a file
- **file2** - the second thread for output to a file

The main processing logic changes in such a way that the command block after its formation should be sent to the console (by the **log** thread) and immediately to the file (by one of the **file1** or **file2** threads).

In this case, sending a block to a file is distributed between file threads. For example, you can directly send even commands through the **file1** thread, and odd commands through **file2**. But the best solution would be to use a single command queue that both file streams will process simultaneously.

It is worth paying attention to the insufficient accuracy of the clock to form a unique name. It is necessary, having saved the timestamp in the name, to add a postfix that will be guaranteed to be different for file streams.

## 2. Managing data input by external code

As part of this part of the work, need to execute the code in the form of a library with an external interface described in the `async.h` file.

The exchange will be initiated by the external code. Instead of the usual entry point to the application `main()`, there will be three external functions `connect()`, `receive()`, and `disconnect()`.

The order of calls is as follows:

- `connect()` is called with the transfer of the size of the command block, and the return value is saved. The value is not interpreted by the calling code in any way and serves only as a context for the `receive()` and `disconnect()` functions.
- `receive()` is called with a pointer to the beginning of the buffer, its size, and the context. The call is repeatable - the calling code can use it to send several commands in a row.
- `disconnect()` is called with the context passed. The call destroys the context completely. From the point of view of the logic of command processing, this call is considered the end of the current block of commands.

It is necessary to implement these functions in such a way as to preserve the previous functionality of the project and add a new feature.

The implementation must allow multiple calls to `connect()`. Calls to `receive()` with different contexts must not interfere with each other.

Depending on the internal implementation, there may be a requirement to have the same block size for all commands. There is no intent to make such a profound change. If the potential for different queue sizes is a major rework, the `connect()` function should return `nullptr`.

Calls can be made from different threads, but calls with the same context are always made from the same thread.

Optionally, implement the ability to call all functions from any thread.

## Implementation Requirements

The result of the work should be a library installed in the standard path. The library must be called [libasync.so](http://libasync.so/) and be in the `async` package.

To test the functionality, implement an executable file that uses the library with a demonstration of all calls. The executable file should also be added to the package.

The result of the work must be published on bintray.

## Verification

The task is considered successful if, after installing the package, linking with the test code (example in `main.cpp`) and running with test data, the output matches the description of [**Task 7**](../HW_7/README.md). The data is supplied in portions in different contexts in large volumes without pauses.

It will be noted that there are no restrictions on calling from different threads.

## Self-control

* file `async.h` should remain unchanged
