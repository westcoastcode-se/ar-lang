# Data Types

Build-in datatypes are found in the `vm` package - and also available in the global scope.

## bool

Boolean

## byte / int8

One byte

## int16

16-bit integer

## int32

32-bit integer

## int64

64-bit integer

## float32

32-bit decimal number

## float64

64 bit decimal number

## Pointer

Any type with a `*` modifier in front of it. Such as `*int32` which represents a memory location to where the integer
is found

## Array

You can augment `[N]` your type with an array modifier. The number (`N`) of items in the array must be known during compile time