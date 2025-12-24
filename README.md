# mathc

`mathc` is an extremely simple arithmetic expression compiler for x64 Linux written in C++.

> This is a toy compiler project and not meant to implement a serious language.

Programs are compiled to native ELF 64-bit executables, typically about 15 kilobytes in size.
The compiler currently only accepts a single file as input.

## Requirements

- 64-bit Linux kernel (most distros should work)
- GCC
- NASM (for assembling the compiler's assembly output)

## Installing

You can download the latest [release here](https://github.com/jakerieger/mathc/releases/latest). The compiler is provided as is. If it works,
great. If not, you are more than welcome to submit a PR. *No seriously, submit a PR. I don't
know what I'm doing*.

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

