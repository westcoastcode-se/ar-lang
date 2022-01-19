# Documentation > Memory

This design document describes how memory is used by the virtual machine

## Primitives

|Keyword|Size (bytes)|Description|
|-------|------------|-----------|
|i8     |1           |Signed 8 bit integer|
|u8     |1           |Unsigned 8 bit integer|
|i16    |2           |Signed 16 bit integer|
|u16    |2           |Unsigned 16 bit integer|
|i32    |4           |Signed 32 bit integer|
|u32    |4           |Unsigned 32 bit integer|
|i64    |8           |Signed 64 bit integer|
|u64    |8           |Unsigned 64 bit integer|
|f32    |4           |A 32 bit real number|
|f64    |8           |A 64 bit real number|
|addr   |4/8         |A memory address. The size depends on if we build the VM in 32- or 64 bit|

## Memory blocks

All memory is, by default, stored in fixed blocks of memory. The size of each block depends on if you compile your application
for a 32- or 64 bit CPU:

|CPU|Block Size (bytes)|
|---|------------------|
|32bit|4|
|64bit|8|

This means that a 16 bit integer will still reserve 4 bytes, instead of 2, if the application is compiled for a 32 bit CPU.

We are doing this because:

1. Lessens the number of operations needed by the virtual machine
2. No need to keep track of variable type for operations, such as `add`. Only specialized operations for f32, f64 and 64 bit integers are needed
3. Streamlines the stack

This also means that we have to add support for casting/converting a value from a higher-bit memory block into a smaller one. 

## Stack

??

## Local variables

When accessing a function, the stack reserves memory for all local variables.

Consider the following bytecode:

```
var (i8,i8,i32)
pushc 100
convi i8
store $0
```

This will reserve 12 bytes on a 32 bit CPU instead of 6 bytes. This is because the `i8` will reserve 4 bytes and not 1. It will then set the value to `100` to the first local variable.

THe actual steps are that we push the constant value of 100 to the stack. Remember, all values on the stack are in blocks of integers. We then have to convert
the integer into a smaller 8 bit integer. We can then take the value from the stack and setting it to the local variables.

Accessing local variables can be done using `$<num>` syntax. The `<num>` is the number which is the beginning of the memory where the value is located.

### Local variables offset

Consider the following bytecode:

```
var (i64,i32)
```

How do you access the second variable on a 64 bit CPU when all memory is placed in 32 bit memory blocks. Since the first variable will take two blocks, then
that means that you have to use `$2`, because the first variable uses the `$0` and `$1` blocks.

Pushing multi-block items to the stack requires specialized operators:

```
var (i64)
pushlc 100
storel $0
```

## Variables

Variables are stored in neutral blocks. Each block will have the size of a neutral integer:

|CPU|Block Size (bytes)|
|---|------------------|
|32bit|4|
|64bit|8|

This means that a 16 bit integer will still reserve memory for a 32 bit variable. But why are we doing this? This is, essentially, a type of optimization.
When compiling a C/C++ application; the CPU will still put

This simplifies how we handle calculations, such as `add`. The
reason for why we still allow smaller integer types is because of:

1. Binary serialization and deserialization.
2. Third-party libraries might use non-integer types



