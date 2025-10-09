# Archive from refactor Branch

This directory contains a complete snapshot of the `origin/refactor` branch for reference and comparison purposes.

## Purpose

This archive preserves the refactored version of the codebase to facilitate:
- Comparison with the current `develop` branch implementation
- Reference during feature migration
- Understanding architectural differences between branches
- Code review and analysis

## Contents

Complete file structure from the `refactor` branch, excluding:
- `3rd/` directory (third-party dependencies)

Included directories:
- `src/` - Core source code
- `include/` - Public headers  
- `modules/` - Modular components
- `tests/` - Test files
- `cmake/` - Build configuration
- `scripts/` - Utility scripts
- `assets/` - Resource files
- `example/` - Example code
- `performance/` - Performance benchmarks

## Source Branch

- **Branch**: origin/refactor
- **Archived Date**: 2025-10-09
- **Purpose**: Refactored codebase with architectural improvements

## Usage

These files serve as a reference implementation and should **not** be directly modified. For integrating features from this archive:

1. Review the implementation in this directory
2. Adapt the code for the current `develop` branch structure
3. Test thoroughly before merging

## Comparison

To compare specific files with the current branch:
```bash
# Compare a specific file
diff old/src/some_file.cpp src/some_file.cpp

# Or use a visual diff tool
code --diff old/src/some_file.cpp src/some_file.cpp
```
