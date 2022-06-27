# Documentation > Instructions > stl

Store the value on the top of the stack and put it into a local variable. The amount of bytes moved from the stack into the local variable
is the same as the local variable size.


## Definition

```
stl <index>
```

## Example

The example below will save the value of `0` into the local variable `value`

```
locals (value int32)
ldc_s_i4 0
stl 0
```
