# 3. Allocator
**Implement STL-compatible allocator, use it in standard containers. Write a container that supports parameterization by the allocator similar to STL.**

Implement a memory allocator that will allow you to perform the memory reservation operation. Then use this allocator with the std::map container.

The allocator must be parameterized by the number of elements allocated at a time. Freeing a specific element is not assumed - the allocator must free all memory itself.

The allocator works with a fixed number of elements. An attempt to allocate more elements is considered an error.

Optionally implement allocator extensibility. When attempting to allocate a number of elements that exceeds the current reserved number, the allocator expands the reserved memory.

Optionally implement element-wise deallocation.

**The purpose of such an allocator is to reduce the number of memory allocation operations.**

Implement your container, which is parameterized by an allocator, by analogy with stl containers. The container should have two options: add a new element and go around the container in one direction.

Optionally implement compatibility with STL containers, including iterators, helper methods such as size and empty, etc.

**The goal of implementing your container is to try to use std::allocator as well as your allocator.**

**The application code must contain the following calls:**

- create an instance of `std::map<int, int>`
- fill with 10 elements, where the key is a number from 0 to 9, and the value is the factorial of the key
- create an instance of `std::map<int, int>` with a new allocator limited to 10 elements
- fill with 10 elements, where the key is a number from 0 to 9, and the value is the factorial of the key
- print all the values ​​(the key and value are separated by a space) stored in the container
- create an instance of your container to store `int` values
- fill with 10 elements from 0 to 9
- create an instance of your container to store `int` values ​​with a new allocator limited to 10 elements
- fill with 10 elements from 0 to 9
- print all the values ​​stored in the container
