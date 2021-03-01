//
// Created by jelle on 3/1/2021.
//

#include <string>
#include "Logger.hpp"

using namespace log;

Logger::Logger(std::ostream &stream) {
	_streams.push_back(&stream);
}

Logger::Logger(const std::vector<std::ostream*> &streams) {
	_streams = streams;
}

Logger::Logger(const Logger &logger) {
	_streams = logger._streams;
}

void    Logger::log(const LogItem &item) {
	if (item.logType == DEBUG && !(_loggerFlags & Flags::Debug)) return;
	for (std::vector<std::ostream*>::const_iterator first = _streams.begin(); first != _streams.end(); ++first) {
		**first << item.toString(_loggerFlags) << std::endl;
	}
}

Logger &Logger::operator=(const Logger &rhs) {
	_streams = rhs._streams;
	return *this;
}

void Logger::setFlags(Flags::flagType flags) {
	_loggerFlags = flags;
}