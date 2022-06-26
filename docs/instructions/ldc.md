# Documentation > Instructions > ldc

You can push fixed constant values onto the stack using the `ldc_` instruction. For example `ldc_i32`

## Definition

```
ldc_<shorthand> <value>
```

The following allowed types are
* `int8` (shorthand: `i8`)
* `int16` (shorthand: `i16`)
* `int32` (shorthand: `i32`)
* `float32` (shorthand: `f32`)


## Example

Push value 1234 onto the stack as an 16 bit integer

```
ldc_i16 0
```
