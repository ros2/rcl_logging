
This document is a declaration of software quality for the `rcl_logging_spdlog` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `rcl_logging_spdlog` Quality Declaration

The package `rcl_logging_spdlog` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy

### Version Scheme
TODO: dump to 1.0

`rcl_logging_spdlog` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#versioning), and is at or above a stable version, i.e. `>= 1.0.0`.

### API Stability Within a Released ROS Distribution

`rcl_logging_spdlog` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution

`rcl_logging_spdlog` contains C++ code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

### Public API Declaration

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

## Change Control Process

`rcl_logging_spdlog` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#change-control-process).

This includes:

- all changes occur through a pull request
- all pull request have two peer reviews
- all pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)
- all pull request must resolve related documentation changes before merging

## Documentation

### Feature Documentation

TODO: Add features and then fix link

`rcl_logging_spdlog` has a [feature list](TODO) and each item in the list links to the corresponding feature documentation.
There is documentation for all of the features, and new features require documentation before being added.

### Public API Documentation

TODO fix link

`rcl_logging_spdlog` has embedded API documentation and it is generated using doxygen and is [hosted](TODO) along side the feature documentation.
There is documentation for all of the public API, and new additions to the public API require documentation before being added.

### License

The license for `rcl_logging_spdlog` is Apache 2.0, and a summary is in each source file, the type is declared in the `package.xml` manifest file, and a full copy of the license is in the `LICENSE` file.

There is an automated test which runs a linter (ament_lint_common) that ensures each file has a license statement.

### Copyright Statements

The copyright holders each provide a statement of copyright in each source code file in `rcl_logging_spdlog`.

There is an automated test which runs a linter (ament_lint_common) that ensures each file has at least one copyright statement.

## Testing

### Feature Testing
TODO: Add unit tests for the package

Each feature in `rcl_logging_spdlog` has corresponding tests which simulate typical usage, and they are located in the `test` directory.
New features are required to have tests before being added.

### Public API Testing
TODO: Add unit tests for the package

Each part of the public API have tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage

`rcl_logging_spdlog` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#coverage), and opts to use line coverage instead of branch coverage.

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Current coverage statistics can be viewed [here](https://ci.ros2.org/job/ci_linux_coverage/55/cobertura/src_ros2_rcl_logging_rcl_logging_spdlog_src/):

### Performance

`rcl_logging_spdlog` follows the recommendations for performance testing of C++ code in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#performance), and opts to do performance analysis on each release rather than each change.

TODO how to run perf tests, where do they live, etc. will this package apply to perf tests?

### Linters and Static Analysis

`rcl_logging_spdlog` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#linters-and-static-analysis).

TODO any qualifications on what "passing" means for certain linters

## Dependencies
TODO: fix links to latest version of Quality declaration of each

`rcl_logging_spdlog` depends on the ROS packages `rcutils` and `spdlog_vendor`. It also depends on the external dependency `spdlog`.

`rcutils` was declared to be Quality Level 1 [here](https://github.com/ros2/rcutils/blob/quality_declaration/QUALITY_DECLARATION.md).

`spdlog_vendor` was declared to be Quality Level 1 [here](TODO).

`spdlog` as external dependency was declared to be Quality Level 1 [here](TODO)

It has several "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

## Platform Support

`rcl_logging_spdlog` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.
