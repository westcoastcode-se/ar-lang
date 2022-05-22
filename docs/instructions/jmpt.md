# Documentation > Instructions > jmpt

Jump to the supplied destination address if the top-most `int32` value on the stack is true. 
The value is not removed from the stack automatically. You have to `free_s` the value manually.

You are only allowed to jump to destinations in the same function. A destination can be marked with
a memory marker. Memory markers starts with a bracket-character `#`.

## Definition

```
jmpt <destination>
```

## Example

Let's jump to a destination and pop the value from the stack

```
const int32 1
jmpt dest
#dest ret
free_s 4	// The boolean is saved as an int32
```
