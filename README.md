# tapi — A testing framework for C17

A testing framework for C17 using libcapstone. `tapi` is a simple, yet powerful testing 
framework that supports unit testing, mocking, and stream capturing for various architectures.  


---

## Features

- Unit testing,
- Easy logging and debugging,
- Grouping tests into suites,
- POSIX file stream capturing,
- Proper assertion and error handling,
- Runtime mocking patching call targets.

---

### Dependencies

- A compiler with C17 support,
- `libcapstone` (included as a submodule).

---

## Build

To get started with `tapi`, clone the repository and build it using make.

```bash
# Clone the repository.
git clone https://github.com/Codeca-Labs/tapi.git
cd tapi

# Gather and build dependencies.
sudo ./get-deps

# Build tapi for a specific architecture,
make all arch=target_architecture # x86_64, x86, arm32, arm64

# Or build it for your native architecture.
make all
```

---

## Roadmap

Planned improvements:

- Support for C++,
- Multi-threaded testing using mocks (ie. mock application with multiple threads),
- Support for even more little-endian architectures (e.g. POWERPC, and RISC-V for both 32-bit and 64-bit).

