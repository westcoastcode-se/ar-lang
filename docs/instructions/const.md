# Documentation > Instructions > const

You can push a constant value onto the stack using the `c_` short hand instruction. For example `c_i32`

## Definition

```
c_<shorthand> <value>
```

The following allowed types are
* `int8` (shorthand: `i8`)
* `int16` (shorthand: `i16`)
* `int32` (shorthand: `i32`)
* `int64` (shorthand: `i64`)
* `float32` (shorthand: `f32`)
* `float64` (shorthand: `f64`)

## Example

Push value 1234 onto the stack as an 32 bit integer

```
c_i32 1234
```
