//
// Created by jelle on 3/8/2021.
//

#ifndef ROUTEBLOCK_HPP
#define ROUTEBLOCK_HPP

#include "config/AConfigBlock.hpp"
#include "regex/Regex.hpp"
#include "config/validators/UrlValidator.hpp"

namespace config {

	class RouteBlock: public AConfigBlock {
	protected:
		static const validatorsMapType	_validators;
		static const validatorListType	_blockValidators;
		static const std::string		_allowedBlocks[];

		const validatorsMapType	&getValidators() const;
		const validatorListType	&getBlockValidators() const;
		const std::string		*getAllowedBlocks() const;

	public:
		RouteBlock(const ConfigLine &line, int lineNumber, AConfigBlock *parent = 0);
		static void	cleanup();
		void parseData();

		std::string getType() const;

	private:
		regex::Regex				_location;
	    bool                        _shouldRewrite;
		std::vector<std::string>	_allowedMethods;
		std::vector<std::string>	_acceptLanguage;
		std::vector<std::string>	_plugins;
		std::string					_root;
		bool						_directoryListing;
		std::string					_index;
		std::string					_cgi;
		std::string					_cgiExt;
		std::string					_saveUploads;
		std::string					_authBasic;
		std::vector<std::string>	_authorized;
		UrlValidator::urlParsed		_proxyUrl;

	public:
		regex::Regex &getLocation();
	    bool shouldLocationRewrite() const;
		const std::vector<std::string> &getAllowedMethods() const;
		const std::string &getRoot() const;
		bool isDirectoryListing() const;
		const std::string &getIndex() const;
		const std::string &getCgi() const;
		const std::string &getCgiExt() const;
		const std::string &getSaveUploads() const;
		const UrlValidator::urlParsed &getProxyUrl() const;
		const std::vector<std::string> &getAcceptLanguage() const;
		const std::vector<std::string> &getAuthorized() const;
		const std::string &getAuthBasic() const;
		/// return if it should serve files (if false -> proxy)
		bool shouldDoFile() const;
		/// return if it cgi is enabled
		bool shouldDoCgi() const;

		bool isAllowedMethod(const std::string &method) const;

	};

}

#endif //ROUTEBLOCK_HPP
