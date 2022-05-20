#include "HttpResponse.h"

#include <sstream>

std::string HttpResponse::get_as_raw_string() {
	std::stringstream sstm;

	sstm << "HTTP/1.1 " << code << " " << status  << "\r\n";

	ResponseHeader::iterator it;
	for (it = header.begin(); it != header.end(); it++) {
		sstm << it->first << ": " << it->second << "\r\n";
	}
	sstm << "\r\n";
	if (body.size()) {
		sstm << body;
	}
	return sstm.str();
}
