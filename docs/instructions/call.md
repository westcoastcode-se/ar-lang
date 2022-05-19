# Documentation > Instructions > Call

Arguments, and memory for the return value, are placed on the stack in the reverse order of the function signature.

## Definition

```
call <signature>
```

## Example

Given the following example:

```
call Add(lhs int32, rhs int32)(int32)
```

Then the function called `Add` that takes two arguments and return one value is called. 

Then you values on the stack have to be pushed in the following order:

| Byte  | **Argument** |
|-------|--------------|
| ebp+0 | ret0         |
| ebp+4 | rhs          |
| ebp+8 | lhs          |

When the `call` is returned then only the return values are left on the stack:

| Byte  | **Argument** |
|-------|--------------|
| ebp+0 | ret0         |

