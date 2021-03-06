#include <iostream>
#include "log/Logger.hpp"
#include "server/Server.hpp"
#include "config/ConfigParser.hpp"
#include <csignal>
#include "utils/ArgParser.hpp"
#include "server/handlers/ThreadHandler.hpp"

using namespace NotApache;

class MainProcess {
public:
	logger::Logger			logger;
	config::ConfigParser	parser;
	Server					server;
	utils::ArgParser		args;
	config::RootBlock		*config;
	static Server			*globalServerInstance;
	static int 				exitCode;

	static void	kill(int signo) {
		exitCode = signo;
		if (!globalServerInstance) return;
		globalServerInstance->shutdownServer();
	}

	static void handleSignals() {
		// pipe errors need to ignored, can happen with connections that get closed
		std::signal(SIGPIPE, SIG_IGN);

		// graceful exit of kill signals
		std::signal(SIGINT, MainProcess::kill);
		std::signal(SIGHUP, MainProcess::kill);
		std::signal(SIGQUIT, MainProcess::kill);
		std::signal(SIGTERM, MainProcess::kill);
	}

	MainProcess(): logger(std::cout), config(0) {
		globalServerInstance = &server;
	};

	int	run(int argc, char *argv[]) {
		exitCode = 0;
		try {
			args = utils::ArgParser(argc, argv);
		} catch (const utils::ArgParser::ArgParserException &e) {
			std::cout << utils::ArgParser::printUsage() << std::endl;
			logger.log(logger::LogItem(logger::ERROR, e.what()));
			return 1;
		} catch (const std::exception &e) {
			logger.log(logger::LogItem(logger::ERROR, std::string("Unhandled exception: ") + e.what()));
			return 1;
		}

		// setup loggers
		logger::Flags::flagType flags = 0;
		if (args.colorPrint())
			flags |= logger::Flags::Color;
		if (args.verbosePrint())
			flags |= logger::Flags::Debug;
		logger.setFlags(flags);

		// parse configuration
		parser.setLogger(logger);
		try {
			config = parser.parseFile(args.configFile());
		} catch (const std::exception &e) {
			logger.log(logger::LogItem(logger::ERROR, std::string("Config could not be parsed: ") + e.what()));
			return 1;
		}

		// validate only mode
		if (args.onlyValidate()) {
			logger.log(logger::LogItem(logger::INFO, "Configuration is valid, exiting!"));
			return 0;
		}

		// create server
		server.setLogger(logger);

		// add server listeners
		{
			// only add unique host+port pairs
			std::vector<std::pair<long, int> >	uniquePairs;
			for (std::vector<config::ServerBlock *>::const_iterator i = config->getServerBlocks().begin(); i != config->getServerBlocks().end(); ++i) {
				bool alreadyAdded = false;
				for (std::vector<std::pair<long, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
					if (j->first == (*i)->getHost() && j->second == (*i)->getPort()) {
						alreadyAdded = true;
						break;
					}
				}
				if (!alreadyAdded)
					uniquePairs.push_back(std::pair<long, int>((*i)->getHost(), (*i)->getPort()));
			}

			for (std::vector<std::pair<long, int> >::const_iterator j = uniquePairs.begin(); j != uniquePairs.end(); ++j) {
				server.addListener(new TCPListener(j->second, j->first));
			}
		}

		// add workers
		if (config->getWorkerCount() == -1) // only use main thread
			server.addHandler(new StandardHandler());
		else {
			for (int i = 0; i < config->getWorkerCount(); ++i) {
				server.addHandler(new ThreadHandler());
			}
		}

		// start server
		handleSignals();
		try {
			server.startServer(config);
		} catch (std::exception &e) {
			logger.log(logger::LogItem(logger::ERROR, e.what()));
			return 1;
		}
		return exitCode;
	}

	~MainProcess() {
		globalServerInstance = 0;
		config::RootBlock::cleanup();
		config::RouteBlock::cleanup();
		config::ServerBlock::cleanup();
	}
};

Server			*MainProcess::globalServerInstance = 0;
int 			MainProcess::exitCode = 0;

int main(int argc, char *argv[]) {
	MainProcess	process;
	return process.run(argc, argv);
}
