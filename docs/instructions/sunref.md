# Documentation > Instructions > sunref

Save the top-most value on the stack in the memory address of the next value on the stack. It is assumed that the values are of the same type


## Definition

```
sunref_<type>
```

The following allowed types are:

* int32

## Example

The example below will set the value of 10 into the memory location of `item`. This is the same as doing the following in C: `*item = 10;`.

```
fn SetRef(item *int32) () {
	load_a 0
	c_i32 10
	sunref_i32
}
```

