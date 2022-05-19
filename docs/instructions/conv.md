# Documentation > Instructions > conv

???

## Definition

```
conv <from_type> <to_type>
```

The allowed `from_type` types are:

* int16
* ???

The allowed `to_type` types are:

* int32
* ???

## Example

If the top-most value on the stack is a 16-bit integer with value 10, then we can convert it into
a 32 bit integer with the value 10

```
const int16 10
conv int16 int32
```

