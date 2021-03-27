//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include "server/http/HTTPClient.hpp"

namespace NotApache {

	class HTTPResponder {
	public:
		static void	generateResponse(HTTPClient &client);

		void generateAssociatedResponse(HTTPClient &client);
	};

}

#endif //HTTPRESPONDER_HPP
