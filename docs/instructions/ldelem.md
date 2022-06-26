# Documentation > Instructions > ldelem

Load the value from an array and put the result on the stack to the. 

The stack is expected to look like this:

|   | **Argument** | **size**         |
|---|--------------|------------------|
| 0 | index        | sizeof(vm_int32) |
| 4 | &array       | sizeof(void*)    |

## Definition

```
ldelem <type>
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
