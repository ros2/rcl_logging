^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcl_logging_implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

3.4.0 (2026-01-28)
------------------
* update rcl_logging_implementation architecture diagram. (`#137 <https://github.com/ros2/rcl_logging/issues/137>`_)
* rcl logging implementation (`#135 <https://github.com/ros2/rcl_logging/issues/135>`_)
  * 1st draft bring-up for rcl_logging_implementation package.
  * add test_logging_implementation to check dynamic loading.
  * address Copilot review comments.
  * fix: correct visibility macro for DLL export in CMakeLists.txt
  * add visibility control with RCL_LOGGING_IMPLEMENTATION_DEFAULT_VISIBILITY.
  * load the all symbols at the initialization.
  * Use goto pattern to eliminate the cleanup duplication.
  * Add basic design doc of rmw_logging_implementation.
  * use RCPPUTILS_SCOPE_EXIT instead of goto statement.
  * logging visibility macro was incorrect.
  * logging symbols stay until the peocess actually exits.
  ---------
  Co-authored-by: Barry Xu <barry.xu@sony.com>
* Contributors: Tomoya Fujita
