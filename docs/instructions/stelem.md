# Documentation > Instructions > stelem

Store the value on the stack to the location described on the stack.

The stack is expected to look like this:

|   | **Argument** | **size**         |
|---|--------------|------------------|
| 0 | &array       | sizeof(void*)    |
| 4 | index        | sizeof(vm_int32) |
| 8 | value        | sizeof(<type>)   |

## Definition

```
stelem <type>
```

## Example

The example below will save the value `10` as an integer to the `values` array found at local variable index `0`

```
locals (values [10]int32)
ldl_a 0
c_i32 0
c_i32 10
stelem int32
```
