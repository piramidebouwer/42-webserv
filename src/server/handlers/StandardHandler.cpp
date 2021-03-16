//
// Created by jelle on 3/3/2021.
//

#include "server/handlers/StandardHandler.hpp"
#include "server/global/GlobalLogger.hpp"
#include <unistd.h>

using namespace NotApache;

const int	StandardHandler::_bufferSize = 1024;

void StandardHandler::read(HTTPClient &client) {
	char	buf[_bufferSize+1];

	ssize_t	ret = ::read(client.getFd(), buf, _bufferSize);
	switch (ret) {
		case 0:
			// connection closed
			client.isHandled.lock();
			client.connectionState = CLOSED;
			_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
			client.isHandled.setNoLock(false);
			client.isHandled.unlock();
			return;
		case -1:
			globalLogger.logItem(logger::DEBUG, "Failed to read from client");
			client.isHandled = false;
			return;
		default:
			// packet found, reading
			buf[ret] = 0; // make cstr out of it by setting 0 as last char
			client.data.request.appendRequestData(buf);
			break;
	}

	// parsing
	client.isHandled.lock();
	if (_parser->parse(client) == HTTPParser::READY_FOR_WRITE) {
		client.connectionState = WRITING;
		_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
	}
	std::cout << client.data.request << std::endl; //
	client.isHandled.setNoLock(false);
	client.isHandled.unlock();
}

void StandardHandler::write(HTTPClient &client) {
	if (client.writeState == NO_RESPONSE) {
		_responder->generateResponse(client);
		client.writeState = IS_WRITING;
	}

	if (client.writeState == IS_WRITING) {
		const std::string		&response = client.data.response.getResponse();
		std::string::size_type	pos = client.data.response.getProgress();
		std::string::size_type	len = response.length() - pos;
		ssize_t ret = ::write(client.getFd(), response.c_str() + pos, len);
		switch (ret) {
			case -1:
				globalLogger.logItem(logger::DEBUG, "Failed to write to client");
				client.isHandled = false;
				return;
			case 0:
				// zero bytes is unlikely to happen, dont do anything if it does happen
				break;
			default:
				client.data.response.setProgress(client.data.response.getProgress()+ret);
				if (len == (std::string::size_type)ret) {
					// wrote entire response, closing
					client.isHandled.lock();
					client.connectionState = CLOSED;
					client.isHandled.setNoLock(false);
					_eventBus->postEvent(ServerEventBus::CLIENT_STATE_UPDATED);
					client.isHandled.unlock();
					return;
				}
				break;
		}
	}
	client.isHandled = false;
}

StandardHandler::StandardHandler(): AHandler() {}
