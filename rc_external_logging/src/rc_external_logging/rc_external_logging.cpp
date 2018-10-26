// Copyright 2018 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <rc_external_logging/rc_external_logging.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/file.h>

/**
 *  Maps the logger name to the log4cxx logger. If the name is null or empty it will map to the
 *  root logger.
 */
static const log4cxx::LoggerPtr get_logger(const char * name) {
    if (nullptr == name ||  '\0' == name[0]) {
        return log4cxx::Logger::getRootLogger();
    }
    return log4cxx::Logger::getLogger(name);
}

static const log4cxx::LevelPtr map_external_log_level_to_library_level(int external_level) {
    log4cxx::LevelPtr level;
    if (external_level <= RC_EXTERNAL_LOGGING_SEVERITY_DEBUG) {
        level = log4cxx::Level::getDebug();
    } else if (external_level <= RC_EXTERNAL_LOGGING_SEVERITY_INFO) {
        level = log4cxx::Level::getInfo();
    } else if (external_level <= RC_EXTERNAL_LOGGING_SEVERITY_WARN) {
        level = log4cxx::Level::getWarn();
    } else if (external_level <= RC_EXTERNAL_LOGGING_SEVERITY_ERROR) {
        level = log4cxx::Level::getError();
    } else if (external_level <= RC_EXTERNAL_LOGGING_SEVERITY_FATAL) {
        level = log4cxx::Level::getFatal();
    }
    return level;
}

rc_external_logging_ret_t rc_external_logging_initialize(const char * config_file) {
    log4cxx::File file(config_file);
    bool use_default_config = false;
    rc_external_logging_ret_t status = RC_EXTERNAL_LOGGING_RET_OK;
    if (!file.exists()) {
        use_default_config = true;
        status = RC_EXTERNAL_LOGGING_RET_CONFIG_FILE_DOESNT_EXIST;
    } else {
        try {
            log4cxx::PropertyConfigurator::configure(file);
        } catch (std::exception& ex) {
            use_default_config = true;
            status = RC_EXTERNAL_LOGGING_RET_CONFIG_FILE_INVALID;
        }
    }

    if (use_default_config) {
        log4cxx::BasicConfigurator::configure();
        //TODO: Add default ROS configuration (coming in next CR)
    }

    return status;
}

rc_external_logging_ret_t rc_external_logging_shutdown() {
    log4cxx::BasicConfigurator::resetConfiguration();
    return RC_EXTERNAL_LOGGING_RET_OK;
}

void rc_external_logging_log(int severity, const char * name, const char * msg) {
    log4cxx::LoggerPtr  logger(get_logger(name));
    log4cxx::LevelPtr level(map_external_log_level_to_library_level(severity));
    logger->log(level, msg);
}

rc_external_logging_ret_t rc_external_logging_set_logger_level(const char * name, int level) {
    log4cxx::LoggerPtr  logger(get_logger(name));
    logger->setLevel(map_external_log_level_to_library_level(level));
    return RC_EXTERNAL_LOGGING_RET_OK;
}
