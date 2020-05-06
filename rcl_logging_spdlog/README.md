# rcl_logging_spdlog

Package supporting an implementation of logging functionality using `spdlog`.

The `rcl_logging` interface allows [rcl_logging_spdlog/logging_interface.h](include/rcl_logging_spdlog/logging_interface.h):
 - initialize
 - log a message
 - set the logger level
 - shutdown

Further still there are some useful abstractions and utilities:
- Macros for controlling symbol visibility on the library
 - [rcl_logging_spdlog/visibility_control.h](include/rcl_logging_spdlog/visibility_control.h)

## Quality Declaration

This package claims to be in the **Quality Level 4** category, see the [Quality Declaration](./Quality_Declaration.md) for more details.
