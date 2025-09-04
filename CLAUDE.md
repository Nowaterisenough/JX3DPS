# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

JX3DPS is a C++20 DPS (Damage Per Second) calculation and simulation library for the Chinese MMORPG "剑网3" (JX3/Jian Wang 3), with an optional Qt-based GUI application. The project provides accurate damage simulations for different character classes and their skill rotations.

## Build Commands

### Prerequisites
- CMake 3.11+
- C++20 compatible compiler
- Qt5/6 (optional, for GUI application)

### Quick Start
```bash
# Clone with submodules (required for dependencies)
git clone --recurse-submodules git@github.com:Nowaterisenough/JX3DPS.git

# Basic build (core library only)
cmake -S . -B ./build
cmake --build ./build --target install --config Release

# Build with GUI app and tests
cmake -S . -B ./build -DBUILD_TESTS=ON -DBUILD_APP=ON
cmake --build ./build --config Release
```

### Common Development Commands
```bash
# Run tests (if built with -DBUILD_TESTS=ON)
./build/bin/tests

# Run benchmarks
./build/bin/benchmark

# Apply code formatting
clang-format -i --style=file <file>

# Static analysis with clang-tidy (configured in .clang-tidy)
clang-tidy <file> -- -I./include -I./src
```

## Architecture Overview

### Core Structure
- **src/**: Core library implementing DPS simulation engine
- **app/**: Qt-based GUI application (optional)
- **include/**: Public API headers
- **modules/**: Reusable components (ThreadPool, custom Qt widgets)
- **config/**: Game data configuration files (JSON format)

### Key Components

**Character Classes** (`src/Class/`):
- Each class (TaiXuJianYi, FenShanJing, MoWen, ZiXiaGong) implements specific JX3 character gameplay
- Classes handle skill rotations, damage calculations, and buff management

**Core Systems**:
- **Attribute System**: Character stats and attribute calculations
- **Damage System**: Core damage calculation engine with complex formulas
- **Buff System**: Comprehensive buff/debuff management with stacking rules
- **Timeline System**: Simulation timeline with keyframe-based event scheduling
- **Expression Parser**: Custom expression evaluation for flexible damage formulas

**Threading**: Custom ThreadPool implementation for multi-threaded simulations with progress callbacks

### API Design
- Core library exports C API for integration
- Progress callback system for long-running simulations
- JSON-based configuration system for game data
- Static library as primary output

## Development Practices

### Code Style
- Google C++ Style Guide base with customizations (see `.clang-format`)
- Chinese comments and documentation (project targets Chinese game)
- C++20 features encouraged
- UTF-8 encoding enforced

### Build System
- CMake with conditional compilation (app/tests can be disabled)
- Multi-platform support (Windows MSVC/MinGW, Ubuntu, macOS)
- Automatic Qt detection with graceful degradation
- CI/CD via GitHub Actions for all platforms

### Project Conventions
- All dependencies via git submodules in `3rd/`
- Configuration files in JSON format under `config/`
- Game mechanics knowledge embedded in class implementations
- Performance-oriented design with benchmarking support

## Common Tasks

### Adding New Character Classes
1. Create new directory under `src/Class/`
2. Implement class-specific skill system and rotations
3. Add configuration entries in `config/` directory
4. Update CMakeLists.txt to include new source files

### Modifying Game Mechanics
- Damage formulas in `src/Damage/`
- Attribute calculations in `src/Attribute/`
- Buff effects and stacking rules throughout class implementations
- Configuration files for numerical constants

### GUI Development
- Main application in `app/MainWidget.cpp`
- Custom Qt widgets in `modules/Widgets/`
- QCustomPlot integration for damage visualization
- Requires Qt5/6 installation

## Dependencies Management

**Required Dependencies** (via submodules):
- nlohmann/json: Configuration parsing
- spdlog: Logging framework

**Optional Dependencies**:
- GoogleTest: Unit testing framework
- Qt5/6: GUI framework (auto-detected)
- QCustomPlot: Plotting widgets

Update submodules when pulling changes:
```bash
git submodule update --recursive
```