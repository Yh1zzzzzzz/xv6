# 🚀 Project Labs Implementation

---

## 🗂️ Organization

> This repository contains solutions for multiple labs, with each solution maintained in its own Git branch.  
> This structure allows:

- ✅ **Clean separation** of different lab implementations  
- ✅ **Easy access** to specific solutions without file conflicts  
- ✅ **Clear version history** for each lab  

---

## 📘 Accessing Solutions

To view any lab implementation:

1. **List all available branches**  
```bash
git branch -a       # List all branches (local and remote)
git checkout cow    # Switch to the 'cow' branch
```

## About XV6

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix Version 6 (v6).
It loosely follows the structure and style of v6, but is implemented for a modern RISC-V multiprocessor using ANSI C.

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition.
See also: MIT 6.1810 Course Site

🧑‍💻 Contributors
Some notable contributors:

Russ Cox (context switching, locking)

Cliff Frey, Xiao Yu (multiprocessor support)

Nickolai Zeldovich, Austin Clements


🛠️ Building and Running xv6
Requirements
A RISC-V "newlib" toolchain from:
https://github.com/riscv/riscv-gnu-toolchain

QEMU compiled for riscv64-softmmu
