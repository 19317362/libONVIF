#include "CmdLineParser.h"


void CmdLineParser::setup(QCommandLineParser &parser) {

	parser.setApplicationDescription("ONVIFinfo is a command line ONVIF client");
	parser.addHelpOption();
	parser.addVersionOption();

	parser.addOptions({
		{{"d", "discover"},
		QCoreApplication::translate("main", "Search for ONVIF hosts. Ignores the host if specified.")
		},
		{"host",
		QCoreApplication::translate("main", "The ONVIF <host>: e.g.: http://localhost. Leave empty if you want to search for ONVIF hosts."),
		QCoreApplication::translate("main", "host"),
		},
		{"verbose",
		QCoreApplication::translate("main", "Prints all the raw SOAP messages. Very noisy.")
		},
		{"path",
		QCoreApplication::translate("main", "The ONVIF 'device' web service endpoint <path>."),
		QCoreApplication::translate("main", "path"),
		"/onvif/device_service"
		},
		{"port",
		QCoreApplication::translate("main", "The <port> of the 'device' web service endpoint. Defaults to 80 for http and 443 for https."),
		QCoreApplication::translate("main", "port")
		},
		{{"u", "user"},
		QCoreApplication::translate("main", "The <user> to authenticate against the ONVIF device."),
		QCoreApplication::translate("main", "user")
		},
		{{"p", "pwd"},
		QCoreApplication::translate("main", "The <password> to authenticate against the ONVIF device."),
		QCoreApplication::translate("main", "password")
		}
	});
}

ArbitraryResponse<CmdOptions> CmdLineParser::parse(const QCommandLineParser &parser) {

	auto response = ArbitraryResponse<CmdOptions>();
	auto options = CmdOptions();

	if(parser.isSet("host")) {
		options.capabilities = true;
		options.discover = false;
		options.host = parser.value("host");
		if(options.host.endsWith('/')) options.host = options.host.remove(options.host.length() - 2, 1);
		if(options.host.startsWith("http:")) {
			options.scheme = "http";
			options.port = 80;
		}
		else if(options.host.startsWith("https:")) {
			options.scheme = "https";
			options.port = 443;
		}
	}
	else {
		options.discover = true;
		options.capabilities = false;
	}

	if(parser.isSet("discover")) {
		options.discover = true;
		options.capabilities = false;
	}

	if(!options.discover) {
		if(parser.isSet("path")) {
			options.path = parser.value("path");
			if(!options.path.startsWith('/')) options.path = "/" + options.path;
		}
		else {
			options.path = "/onvif/device_service";
		}

		if(parser.isSet("verbose")) {
			options.verbose = true;
		}
		else {
			options.verbose = false;
		}

		if(parser.isSet("port")) {
			bool success = false;
			options.port = parser.value("port").toInt(&success);
			if(!success) {
				return ArbitraryResponse<CmdOptions>(GENERIC_FAULT, QCoreApplication::translate("main", "Couldn't parse port: %1").arg(parser.value("port")));
			}
		}

		if(parser.isSet("user")) {
			options.user = parser.value("port");
		}

		if(parser.isSet("pwd")) {
			options.pwd = parser.value("pwd");
		}

		options.endpointUrl = QUrl::fromUserInput(options.host + ":" + QString::number(options.port) + options.path);
		if(!options.endpointUrl.isValid()) {
			return ArbitraryResponse<CmdOptions>(GENERIC_FAULT, QCoreApplication::translate("main", "The endpoint is invalid: %1").arg(options.endpointUrl.errorString()));
		}
	}
	else {
		return ArbitraryResponse<CmdOptions>(GENERIC_FAULT, QCoreApplication::translate("main", "ONVIF device discover not yet implemented."));
	}

	response.SetResultObject(options);
	return response;
}
