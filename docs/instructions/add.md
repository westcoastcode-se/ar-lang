# Documentation > Instructions > Add

Add two values from the stack and push the result to the stack. 
The type has to be known beforehand and the two values has to be of the same type. You can
convert one type to another type using the [conv.md](conv) instruction

## Definition

```
add <type>
```

The following allowed types are:

* int32

## Example

If you want to do a `10 + 20` then you do the following. Please note the order of values pushed on the stack

```
const int32 10
const int32 20
add int32
```

