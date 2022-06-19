# Documentation > Instructions > const

You can push a constant value onto the stack using the `c_` short hand instruction. For example `c_i32`

## Definition

```
c_<shorthand> <value>
```

The following allowed types are:
* `int16` (shorthand: `i16`)
* `int32` (shorthand: `i32`)

## Example

Push value 1234 onto the stack as an 32 bit integer

```
c_i32 1234
```
