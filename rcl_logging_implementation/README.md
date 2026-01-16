# rcl_logging_implementation

## Overview

The `rcl_logging_implementation` package provides dynamic loading capabilities for `rcl` logging implementations, similar to how [rmw_implementation](https://github.com/ros2/rmw_implementation) works for middleware layers.
This allows users to switch logging backends at runtime without rebuilding `rcl`.

> [!NOTE]
> This package is **only used when dynamic loading is enabled** at build time.
> When RCL is built with a specific logging implementation (e.g., `rcl_logging_spdlog`), it links directly to that implementation without this abstraction layer, eliminating function call overhead.

## Features

- **Dynamic Loading**: Load logging implementations (e.g., `rcl_logging_spdlog`, `rcl_logging_noop`) at runtime
- **Runtime Configuration**: Use `RCL_LOGGING_IMPLEMENTATION` environment variable to select implementation
- **Fallback Default**: Defaults to `rcl_logging_spdlog` if no implementation is specified

## Architecture

### Dynamic Loading (Default)

When `rcl` is built **without** specifying `RCL_LOGGING_IMPLEMENTATION`, it uses this package for runtime selection:

```
rcl application
    ↓
rcl (core library)
    ↓
rcl_logging_implementation (this package) ← Dynamic loading happens here
    ├─ Runtime Loading → rcl_logging_spdlog
    ├─ Runtime Loading → rcl_logging_noop
    └─ Runtime Loading → custom implementations
```

### Static Linking (Opt-in)

When `rcl` is built **with** `RCL_LOGGING_IMPLEMENTATION=rcl_logging_spdlog`, it bypasses this package entirely:

```
rcl application
    ↓
rcl (core library)
    ↓
rcl_logging_spdlog ← Direct link, no abstraction layer
```

See more details for [basic design](./doc/basic_design.md).

## Usage

### Runtime Configuration (Dynamic Loading)

Set the `RCL_LOGGING_IMPLEMENTATION` environment variable to select a logging implementation at runtime:

```bash
# Use spdlog (default)
export RCL_LOGGING_IMPLEMENTATION=rcl_logging_spdlog
ros2 run my_package my_node

# Use noop
export RCL_LOGGING_IMPLEMENTATION=rcl_logging_noop
ros2 run my_package my_node

# Use custom implementation
export RCL_LOGGING_IMPLEMENTATION=my_custom_logging
ros2 run my_package my_node
```

If not set, the implementation defaults to `rcl_logging_spdlog` that you can also set via `DEFAULT_RCL_LOGGING_IMPLEMENTATION`.

```bash
# Set the default to noop (still allows runtime override via environment variable)
colcon build --packages-select rcl_logging_implementation --cmake-args -DDEFAULT_RCL_LOGGING_IMPLEMENTATION=rcl_logging_noop
```

### Build-Time Configuration (Static Linking)

For applications that require static linking (e.g., embedded systems, strict deployment requirements), configure `rcl` at build time to link directly to a specific implementation:

```bash
# Build RCL with static linking to spdlog (bypasses rcl_logging_implementation)
colcon build --packages-select rcl --cmake-args -DRCL_LOGGING_IMPLEMENTATION=rcl_logging_spdlog

# Build RCL with static linking to noop (bypasses rcl_logging_implementation)
colcon build --packages-select rcl --cmake-args -DRCL_LOGGING_IMPLEMENTATION=rcl_logging_noop
```
