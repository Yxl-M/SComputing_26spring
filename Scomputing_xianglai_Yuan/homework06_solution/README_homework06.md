# HomeWork 06 - Split the work

This submission implements the DAXPY vector operation

```text
d[i] = a*x[i] + y[i]
```

in C language using chunk-based processing.

## Files

- `homework06_daxpy_chunks.c`: main C program.
- `input.conf`: input configuration file using `Variable = Value` format.
- `Makefile_homework06`: Makefile for compiling and running the program.

## Compile

If HDF5 is installed, compile with:

```bash
h5cc -O2 -Wall -Wextra -o homework06_daxpy_chunks homework06_daxpy_chunks.c -lm
```

or:

```bash
make -f Makefile_homework06
```

## Run

```bash
./homework06_daxpy_chunks input.conf
```

or:

```bash
make -f Makefile_homework06 run
```

## Output

The program prints:

- number of chunks,
- original vector sum,
- chunk-based vector sum,
- vector equality check,
- sum equality check,
- partial sum for each chunk.

It also creates an HDF5 file named `homework06_output.h5` containing:

- `/x`
- `/y`
- `/d`
- `/partial_chunk_sum`
- `/chunk_start`
- `/chunk_end_exclusive`
- `/chunks/chunk_000`, `/chunks/chunk_001`, etc.
