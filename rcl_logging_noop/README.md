# rcl_logging_noop

Package supporting an implementation of no-op logging functionality.

[rcl_logging_noop](src/rcl_logging_noop.cpp) logging interface implementation can:
 - initialize
 - log a message
 - set the logger level
 - shutdown

## Build

Currently there is no way to select the logging interface implementation without building [rcl](https://github.com/ros2/rcl) with target logging interface implementation.

```bash
export RCL_LOGGING_IMPLEMENTATION=rcl_logging_noop
colcon build --symlink-install --cmake-clean-cache --packages-select rcl_logging_noop rcl
```
