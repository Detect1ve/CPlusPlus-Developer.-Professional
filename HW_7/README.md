# 7. Batch Command Parser

Creating a utility for parsing and complex logging of batch commands. We strive to reduce dependencies between modules.

### Develop a program for batch processing of commands.

Commands are read line by line from standard input and processed in blocks of N commands. One command is one line, the specific value does not matter. If the data ends, the block is terminated forcibly. The N parameter is passed as the only command-line parameter as an integer.

**Operation logic for static blocks (in the example N == 3):**

./bulk 3
| Input | Output | Comment |
| ----- | ------ | ------- |
| cmd1 | | |
| cmd2 | | |
| cmd3 | | |
| | bulk: cmd1, cmd2, cmd3 | The block is completed - output the block.
| cmd4 | | |
| cmd5 | | |
| EOF | | |
| | bulk: cmd4, cmd5 | End of input – forcefully terminates the block.

The block size can be changed dynamically if the `{` and `}` commands are given before and immediately after the block. The previous packet is then forcibly terminated. Such blocks can be included in each other, and the nested `{` and `}` commands are ignored (but not the blocks themselves). The entire dynamic block is ignored if the data ends inside a dynamic block.

**The logic of operation for dynamic block size (in the example N == 3):**

./bulk 3
| Input | Output | Comment |
| ----- | ------ | ------- |
| cmd1 | | |
| cmd2 | | |
| { | | |
| | bulk: cmd1, cmd2 | Start of dynamic block – output the previous static block ahead of schedule.
| cmd3 | | |
| cmd4 | | |
| } | | |
| | bulk: cmd3, cmd4 | End of dynamic block – output.
| { | | |
| cmd5 | | |
| cmd6 | | |
| { | | Ignore nested commands. |
| cmd7 | | |
| cmd8 | | |
| } | | Ignore nested commands. |
| cmd9 | | |
| } | | |
| | bulk: cmd5, cmd6, cmd7, cmd8, cmd9 | End of dynamic block – output. |
| { | | |
| cmd10 | | |
| cmd11 | | |
| EOF | | |
| | | End of input - dynamic block is ignored, not output. |

Along with the console output, the blocks should be saved in separate files named bulk1517223860.log , where 1517223860 is the time the first command from the block was received. One file per block.

- **Requirements for implementation**

  The binary file must be named bulk.
- **Verification**

  The task is considered completed successfully if, after installing the package and running with test data, the output matches the description. Data is fed to the standard input line by line with a 1-second pause for visual control. Low coupling of data processing, accumulation of batches of commands, output to the console, and file saving will be noted.
