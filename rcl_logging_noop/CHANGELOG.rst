^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcl_logging_noop
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

3.1.1 (2024-12-18)
------------------
* rcl_logging_interface is only valid path with build environment. (`#122 <https://github.com/ros2/rcl_logging/issues/122>`_) (`#123 <https://github.com/ros2/rcl_logging/issues/123>`_)
  * logging impls need to export dependency for 'rcl_logging_interface'.
  (cherry picked from commit 0724aeb90f5d4efd23a75b1a97a4429ec6911a60)
  Co-authored-by: Tomoya Fujita <Tomoya.Fujita@sony.com>
* Contributors: mergify[bot]

3.1.0 (2024-03-28)
------------------

3.0.0 (2024-01-24)
------------------
* add file_name_prefix parameter to external log configuration. (`#109 <https://github.com/ros2/rcl_logging/issues/109>`_)
* Contributors: Tomoya Fujita

2.7.1 (2023-12-26)
------------------

2.7.0 (2023-09-07)
------------------
* Remove the last uses of ament_target_dependencies in this repo. (`#102 <https://github.com/ros2/rcl_logging/issues/102>`_)
* Contributors: Chris Lalancette

2.6.0 (2023-04-27)
------------------

2.5.1 (2023-04-11)
------------------

2.5.0 (2023-02-13)
------------------
* Update rcl_logging to C++17. (`#98 <https://github.com/ros2/rcl_logging/issues/98>`_)
* Contributors: Chris Lalancette

2.4.3 (2022-11-18)
------------------
* Updated maintainers - 2022-11-07 (`#96 <https://github.com/ros2/rcl_logging/issues/96>`_)
* Contributors: Audrow Nash

2.4.2 (2022-11-02)
------------------

2.4.1 (2022-09-13)
------------------

2.4.0 (2022-04-29)
------------------

2.3.0 (2022-03-01)
------------------

2.2.1 (2022-01-14)
------------------
* Update maintainers to Chris Lalancette (`#83 <https://github.com/ros2/rcl_logging/issues/83>`_)
* Contributors: Audrow Nash

2.2.0 (2021-11-18)
------------------

2.1.3 (2021-09-16)
------------------

2.1.2 (2021-04-06)
------------------

2.1.1 (2021-01-25)
------------------

2.1.0 (2020-12-08)
------------------
* Make internal dependencies private (`#60 <https://github.com/ros2/rcl_logging/issues/60>`_)
* Update the maintainers. (`#55 <https://github.com/ros2/rcl_logging/issues/55>`_)
* Contributors: Chris Lalancette, Shane Loretz

2.0.1 (2020-07-21)
------------------
* Remove unused pytest dependency. (`#43 <https://github.com/ros2/rcl_logging/issues/43>`_)
* Contributors: Chris Lalancette

2.0.0 (2020-06-18)
------------------
* Use new package with rcl logging interface (`#41 <https://github.com/ros2/rcl_logging/issues/41>`_)
* Contributors: Chris Lalancette

1.0.0 (2020-05-26)
------------------

0.4.0 (2020-04-24)
------------------
* Fix CMake warnings about using uninitialized variables (`#30 <https://github.com/ros2/rcl_logging/issues/30>`_)
* Contributors: Dirk Thomas

0.3.3 (2019-10-23)
------------------

0.3.2 (2019-10-07)
------------------
* Enable linters for noop and log4cxx. (`#12 <https://github.com/ros2/rcl_logging/issues/12>`_)
* Contributors: Steven! Ragnarök

0.3.1 (2019-10-03)
------------------

0.3.0 (2019-09-26)
------------------
* remove unused 'dependencies' CMake variable (`#16 <https://github.com/ros2/rcl_logging/issues/16>`_)
* fix package.xml schema violations (`#15 <https://github.com/ros2/rcl_logging/issues/15>`_)
* Contributors: Mikael Arguedas

0.2.1 (2019-05-08)
------------------
* Changing the default location for log files to be a local directory instead of /var/log/ros on linux due to permission issues. (`#9 <https://github.com/ros2/rcl_logging/issues/9>`_)
* Prototype to put things in ~/.ros/log
* Change the API to add an allocator to logging initialize. (`#10 <https://github.com/ros2/rcl_logging/issues/10>`_)
* Contributors: Chris Lalancette, Steven! Ragnarök

0.2.0 (2019-03-09)
------------------

0.1.0 (2018-12-07)
------------------
* First release.
* Contributors: Nick Burek, William Woodall
