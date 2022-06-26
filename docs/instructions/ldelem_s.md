# Documentation > Instructions > ldelem_s

Load the value from an array and put the result on the stack to the. This instruction is specialized for types with a smaller size `256` or less.

The stack is expected to look like this:

|   | **Argument** | **size**         |
|---|--------------|------------------|
| 0 | index        | sizeof(vm_int32) |
| 4 | &array       | sizeof(void*)    |

## Definition

```
ldelem_s <type>
```

## Example

The example below will load a value from the `values` array found at local variable index `0`

```
locals (values [10]int32)
// ...
c_i32 0
ldl_a 0
ldelem int32
```
