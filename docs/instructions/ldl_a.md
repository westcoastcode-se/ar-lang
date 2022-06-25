# Documentation > Instructions > ldl_a

Load the address of a local variable. Used if you want to work with pointers


## Definition

```
ldl_a <index>
```

## Example

The example below will push the address to the local variable `value` at the top of the stack

```
locals (value int32)
ldl_a 0
```
