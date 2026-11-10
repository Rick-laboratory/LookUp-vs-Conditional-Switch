# Lookup Table vs. Switch

A small C++ experiment comparing two ways to map an integer input to a value: a `switch` statement and an indexed lookup table.

Both implementations return the input for `0..10`, and `0` for other values. The program uses the same reproducible input buffer for both, checks their output against the specified mapping, and reports elapsed time and a checksum.

## Build

Requirements: CMake 3.16+ and a C++17 compiler. The example has no platform-specific dependencies.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Use an **optimized Release build**. With multi-configuration generators such as Visual Studio, the executable is in `build/Release/`; with Ninja or Makefiles, it is in `build/`.

## Run

```sh
./build/dispatch-benchmark          # 100 rounds
./build/dispatch-benchmark 1000     # Longer run
./build/dispatch-benchmark --help
```

On Windows, use the corresponding `dispatch-benchmark.exe` path. Valid round counts are `1..100000`. Each round processes 65,536 inputs; the default completes quickly on a typical desktop. Exit codes are `0` for success, `1` for a checksum mismatch, and `2` for invalid arguments.

## Code guide

| Part | Purpose |
| --- | --- |
| `switch_value` | Explicit cases and a default branch |
| `lookup_value` | Bounds-checked access to the same mapping |
| `measure` | One consistent monotonic clock and a 64-bit checksum |
| `main` | Bounded arguments, reproducible input generation, and result verification |

The fixed seed is `42`; inputs cover both valid entries and the default case. Setup and console output happen outside the measured intervals. Input bytes are read through a volatile pointer so an optimizer cannot replace the entire repeated computation with a constant; this adds a shared input-read cost to both measurements.

## Reading the results

This is an educational microbenchmark, not evidence that either technique is universally faster. A compiler may turn a switch into a table, arithmetic, or branches. This particular identity mapping is deliberately simple and can optimize especially well.

The input buffer is reused, the switch measurement runs first, and there is no statistical sampling or CPU affinity control. Cache state, scheduling, compiler flags, and CPU architecture can affect results. Repeat runs, record the compiler and flags, and inspect generated machine code before drawing conclusions.

## License

This repository does not currently include a license file.
