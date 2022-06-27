# Documentation > Instructions > sturef_s

Save the top-most value on the stack in the memory address of the next value on the stack. It is assumed that the value has the same size as the
destination pointer location.

This instruction is specialized for types smaller than 256 bytes and also has specialized instructions for types with sizes:

* 1 byte
* 2 bytes
* 4 bytes
* 8 bytes

## Definition

```
sturef_s <type>
sturef_s_<shorthand_type>
```

## Example

The example below will set the value of 10 into the memory location of `item`. This is the same as doing the following in C: `*item = 10;`.

```
fn SetRef(item *int32) () {
	lda 0
	ldc_i32 10
	sturef_s int32
}
```

The same example above can also be written using the shorthand expression:

```
fn SetRef(item *int32) () {
	lda 0
	ldc_i32 10
	sturef_s_i32
}
```

