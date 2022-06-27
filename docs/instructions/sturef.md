# Documentation > Instructions > sturef

Save the top-most value on the stack in the memory address of the next value on the stack. It is assumed that the value has the same size as the
destination pointer location.

## Definition

```
sturef <type>
```

## Example

The example below will set the value of 10 into the memory location of `item`. This is the same as doing the following in C: `*item = 10;`.

```
fn SetRef(item *int32) () {
	lda 0
	ldc_i32 10
	sturef int32
}
```

