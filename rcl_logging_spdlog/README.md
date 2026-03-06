# rcl_logging_spdlog

Package supporting an implementation of logging functionality using `spdlog`.

[rcl_logging_spdlog](src/rcl_logging_spdlog.cpp) logging interface implementation can:
 - initialize
 - log a message
 - set the logger level
 - shutdown

## Environment Variables

### `RCL_LOGGING_SPDLOG_FLUSH_PERIOD_SECONDS`

Controls the periodic flush interval for log files. By default, logs are flushed every 5 seconds and immediately on error-level messages.

| Value | Behavior |
|-------|----------|
| Not set | Default: flush every 5 seconds + on error |
| `0` | Immediate flush on every log message (unbuffered) |
| `N` (positive integer) | Flush every N seconds + on error |

### `RCL_LOGGING_SPDLOG_EXPERIMENTAL_OLD_FLUSHING_BEHAVIOR`

When set to `1`, disables all automatic flushing configuration (legacy behavior). This takes precedence over `RCL_LOGGING_SPDLOG_FLUSH_PERIOD_SECONDS`.

## Quality Declaration

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](./QUALITY_DECLARATION.md) for more details.
