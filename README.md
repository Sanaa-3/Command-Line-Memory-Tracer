# Assignment #3: Command Line Memory Tracer
This C program reads command lines from standard input, stores them dynamically in an array and a linked list,
and traces memory allocation and deallocation using custom `malloc`, `realloc`, and `free` functions. 
The program also includes a sample 2D dynamic array creation and extension to demonstrate memory tracing.

## Features
- Dynamic allocation of command lines
- Linked list creation and recursive traversal
- Custom memory trace with function stack
- Output redirected to `memtrace.out`
- Demonstrates `malloc`, `realloc`, and `free` tracing

## File Structure
- `main()` reads input and dynamically stores commands.
- `make_extend_array()` builds and extends a 2D int array.
- Memory tracing functions log all allocations and deallocations.
- All output is redirected to `memtrace.out`.

## How to Compile
gcc mem_tracer.c -o mem_tracer 


## How to Run
./mem_tracer
(Example input:)
ls -l
echo Hello, world!
gcc main.c -o main
(After entering input, press Ctrl+D to signal end of input and execute the rest of the program.)

All output (including memory allocation logs and linked list contents) 
will be saved in a file called memtrace.out in the same directory.

## Expected Output Sample (from memtrace.out)
File main.c, line 200, function main allocated new memory segment at address 0x562a0e0 with size 80
File main.c, line 228, function main allocated new memory segment at address 0x562a110 with size 16
File main.c, line 260, function main freed memory segment at address 0x562a110
Linked list nodes: 
Line 1: ls -l
Line 2: echo Hello, world!
Line 3: gcc main.c -o main
...

