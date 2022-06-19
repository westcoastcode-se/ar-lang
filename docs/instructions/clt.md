# Documentation > Instructions > clt

Pop the top-most values on the stack (`value1` and `value2`) and compares them. If `value1` is less than `value2` then
push a `1` (true), otherwise it pushes `0`.

It is assumed that the two values are `4` bytes of the type `int32`. 

If your values are not `int32` then you have to convert them into `int32`

## Definition

```
clt
```

## Example

Let's pretend that you want to evaulate `10 < 20` then you do the following:

```
c_i32 20
c_i32 10
clt
```
