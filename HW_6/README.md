# 6. Matrix

Implement an infinite sparse matrix using the Proxy pattern.

Design a 2-dimensional sparse infinite matrix filled with default values. The matrix should store only occupied elements - those whose values ​​have been assigned at least once. Assigning a default value to a cell frees the cell.

💡 It is necessary to be able to answer the question - how many cells are occupied?

💡 It is necessary to be able to iterate over all occupied cells. The order does not matter. The position of the cell and its value are returned.

💡 When reading an element from a free cell, return the default value.

**Example:**
```cpp
// infinite matrix int filled with values ​​-1
Matrix<int, -1> matrix;
assert(matrix.size() == 0); // all cells are free
auto a = matrix[0][0];
assert(a == -1);
assert(matrix.size() == 0);
matrix[100][100] = 314;
assert(matrix[100][100] == 314);
assert(matrix.size() == 1);
// one row will be printed
// 100100314
for(auto c: matrix)
{
	int x;
	int y;
	int v;
	std::tie(x, y, v) = c;
	std::cout << x << y << v << std::endl;
}
```
1. When starting the program, it is necessary to create a matrix with an empty value 0, fill the main diagonal of the matrix (from \[0,0\] to \[9,9\]) with values ​​from 0 to 9.
2. The secondary diagonal (from \[0,9\] to \[9,0\]) with values ​​from 9 to 0.
3. It is necessary to output a fragment of the matrix from \[1,1\] to \[8,8\]. Space between columns. Each row of the matrix is on a new line of the console.
4. Output the number of occupied cells.
5. Output all occupied cells along with their positions.
6. Optionally implement an N-dimensional matrix.
7. Optionally implement the canonical form of the `=` operator, allowing expressions `((matrix[100][100] = 314) = 0) = 217`

### Self-control
- indexing with the `[]` operator
- the number of occupied cells should be 18 after running the example above

### Verification
The task is considered successful if, after analyzing the code, installing the package, and running the application, a fragment of the matrix, the number of cells, and a list of all values with their corresponding positions appear.
