# Documentation > Instructions > stelem_s

Store the value on the stack to the location described on the stack. Expects the address of the array and the index in the array to be on the stack.
This instruction is specialized for types with a smaller size `256` or less.

The stack is expected to look like this:

|   | **Argument** | **size**         |
|---|--------------|------------------|
| 0 | &array       | sizeof(void*)    |
| 4 | index        | sizeof(vm_int32) |
| 8 | value        | sizeof(<type>)   |


## Definition

```
stelem_s <type>
```

## Example

The example below will save the value `10` as an integer to the `values` array found at local variable index `0`

```
locals (values [10]int32)
ldl_a 0
c_i32 0
c_i32 10
stelem_s int32
```
