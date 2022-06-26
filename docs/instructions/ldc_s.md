# Documentation > Instructions > ldc_s

You can push fixed constant values onto the stack using the `ldc_s_` short hand instruction. For example `ldc_s_i16`

## Definition

```
ldc_s_<shorthand> <value>
```

The following allowed types are
* `int8` (shorthand: `i8`)
* `int16` (shorthand: `i16`)
* `int32` (shorthand: `i32`)
* `int64` (shorthand: `i64`)
* `float32` (shorthand: `f32`)
* `float64` (shorthand: `f64`)

The allowed values are `0`, `1` and `-1`

## Example

Push value 1234 onto the stack as an 16 bit integer

```
ldc_s_i16 0
```
