//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include "server/ServerTypes.hpp"
#include "utils/mutex.hpp"
#include "server/http/HTTPClientData.hpp"
#include <vector>

namespace NotApache {

	enum ClientWriteState {
		IS_WRITING,
		NO_RESPONSE,
		GOT_ASSOCIATED
	};
	enum ClientConnectionState {
		READING,
		WRITING,
		ASSOCIATED_FD,
		CLOSED
	};

	enum ClientResponseState {
		PROXY,
		FILE,
		CGI,
		NONE
	};

	class HTTPClient {
	private:
		FD				_fd;
		int				_port;
		std::vector<FD>	_associatedFds;

	public:
		ClientWriteState		writeState;
		ClientConnectionState	connectionState;
		ClientResponseState		responseState;
		utils::Mutex<bool>		isHandled;
		HTTPClientData			data;

		HTTPClient(FD clientFd, int port);
		~HTTPClient();

		FD	getFd() const;
		int getPort() const;

		void	addAssociatedFd(FD fd);
		void	removeAssociatedFd(FD fd);
		FD		getAssociatedFd(std::vector<FD>::size_type i) const;
		std::vector<FD>::size_type	getAssociatedFdLength() const;
	};

}

#endif //HTTPCLIENT_HPP
