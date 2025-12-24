# mathc

`mathc` is an extremely simple arithmetic expression compiler for x64 Linux written in C++.
This is a toy compiler project and not meant to implement a serious language.

## Requirements

- 64-bit Linux kernel
- Arch-based distro
- NASM (for assembling the compiler's assembly output)

## Examples

1. Basic expressions

```
23.44 + (4 // 5) / 6 * 3 * 3;
```

2. Variables and compound expressions

```
x = 333.5;
y = 0.334;
z = x * y + (x * y);
```

3. Printing to `stdout`

```
print(10.4);
print(44 * 66);

x = 2 // 4;
print(x);
```


```
```
```
```
```
```
```
