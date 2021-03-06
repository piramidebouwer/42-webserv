//
// Created by jelle on 3/13/2021.
//

#ifndef SERVEREVENTBUS_HPP
#define SERVEREVENTBUS_HPP

#include <unistd.h>
#include <exception>
#include "server/ServerTypes.hpp"

namespace NotApache {

	class ServerEventBus {
	private:
		FD _writeEnd;
		FD _readEnd;

	public:
		enum Events {
			CLIENT_STATE_UPDATED = 'a',
			START_QUEUE = 'b'
		};

		ServerEventBus();
		virtual ~ServerEventBus();
		/// will make a linked copy, posting an event can be read by both
		ServerEventBus(const ServerEventBus &cpy);

		FD getReadFD();
		Events getPostedEvent();
		void postEvent(Events event);

		class PipeCreationFailedException: public std::exception {
		public:
			const char *what() const throw() {
				return "Failed to create pipes for event bus";
			}
		};

		class FailedToGetEvent: public std::exception {
		public:
			const char *what() const throw() {
				return "Failed to read from event bus";
			}
		};

		class FailedToPostEvent: public std::exception {
		public:
			const char *what() const throw() {
				return "Failed to write to event bus";
			}
		};
	};
}

#endif //SERVEREVENTBUS_HPP
