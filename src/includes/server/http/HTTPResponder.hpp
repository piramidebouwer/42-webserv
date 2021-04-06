//
// Created by jelle on 3/13/2021.
//

#ifndef HTTPRESPONDER_HPP
#define HTTPRESPONDER_HPP

#include <config/blocks/ServerBlock.hpp>
#include "server/http/HTTPClient.hpp"
#include "env/env.hpp"
#include "utils/base64.hpp"
#include <climits>
#include <cstdlib>
#include <sys/stat.h>

#ifdef BUILD_APPLE
	#define STAT_TIME_FIELD st_mtimespec
#endif
#ifndef STAT_TIME_FIELD
	#define STAT_TIME_FIELD st_mtim
#endif

namespace NotApache {
	class HTTPResponder {
	public:
		static void		generateResponse(HTTPClient &client);

		static void generateAssociatedResponse(HTTPClient &client);

		static void serveFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const std::string &file);

		static void handleError(HTTPClient &client, config::ServerBlock *server, int code, bool doErrorPage = true);
		static void handleError(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route, int code, bool doErrorPage = true);

		static void	serveDirectory(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &directoryStat, const std::string &dirPath);
		static void	prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const utils::Uri &file, int code = 200);
		static void	prepareFile(HTTPClient &client, config::ServerBlock &server, config::RouteBlock &route, const struct ::stat &buf, const utils::Uri &file, int code = 200);

		static void handleProxy(HTTPClient &client, config::ServerBlock *server, config::RouteBlock *route);

		static void setEnv(HTTPClient& client, CGIenv::env& envp, std::string& uri, const std::string& f);
		static void runCGI(HTTPClient& client, const std::string &f, const std::string& cgi);
		static bool checkCredentials(const std::string& authFile, const std::string& credentials);
		
		class ReadFail : public std::exception {
			public:
				virtual const char *what() const throw() { return "Read fail"; }
		};
		class OpenFail : public ReadFail {
			public:
				virtual const char *what() const throw() { return "Open fail"; }
		};
		class MaxFileSize : public ReadFail {
			public:
				virtual const char *what() const throw() { return "Reached max file size"; }
		};
		class AuthHeader : public ReadFail {
			public:
				virtual const char *what() const throw() { return "Invalid authentication header"; }
		};
	};
}

#endif //HTTPRESPONDER_HPP
