# Documentation > Instructions > copy_s

Copy the top-most value on the stack and push the result. This instruction has to be augmented with
what type of value is on the stack at this time.

## Definition

```
copy_s <type>
```

The following allowed types are:

* int32

## Example

If you want to do a `10 + 10` but what to use `copy_s`

```
const int32 10
copy_s int32
add int32
```

