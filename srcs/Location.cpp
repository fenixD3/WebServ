#include "Location.h"


bool Location::IsMethodAllowed(HttpMethod method) const {
	return std::find(allowed_methods.begin(), allowed_methods.end(), method) != allowed_methods.end();
}

bool Location::IsUrlMatchLocation(std::string url) {
	return url.rfind(pattern, 0) == 0;
}

std::string Location::GetErrorPage(int code) const {
	if (error_pages.find(code) == error_pages.end()) {
		std::stringstream sstm;  
  		sstm << default_error_page_dir << code << ".html";  
		return sstm.str();
	}
	return error_pages.at(code);
}

bool Location::operator<(const Location& other_location) const
{
	if (other_location.pattern.rfind(pattern, 0) == 0) {
		return other_location.pattern.size() < pattern.size();
	}
	// Может быть достаточно отсортировать в лексографическом порядке?
	return (other_location.pattern < pattern);
}