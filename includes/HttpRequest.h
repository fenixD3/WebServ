/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 16:51:15 by zytrams           #+#    #+#             */
/*   Updated: 2022/05/18 21:36:48 by zytrams          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>
#include <vector>

enum HttpMethod
{
	GET,
	POST,
	DELETE,
	//PUT,
	UNKNOWN
};

inline const char* ToString(HttpMethod value)
{
	switch (value)
	{
		case GET:       return "GET";
		case POST:      return "POST";
		case DELETE:    return "DELETE";
		//case PUT:       return "PUT";
		default:        return "UNKNOWN";
	}
}


inline HttpMethod ToHttpMethod(std::string value)
{
	if (value == "GET")
		return GET;
	else if (value == "POST")
		return POST;
	else if (value == "DELETE")
		return DELETE;
	else
		return UNKNOWN;
}

enum TransferEncoding
{
	CHUNKED,
	COMPRESS,
	DEFLATE,
	GZIP,
	STATIC
};

inline TransferEncoding ToTransferEncoding(std::string value)
{
	if (value == "chunked")
		return CHUNKED;
	else if (value == "compress")
		return COMPRESS;
	else if (value == "deflate")
		return DEFLATE;
	else if (value == "gzip")
		return GZIP;
	else
		return STATIC;
}


typedef std::map<std::string, std::string> RequestHeader;
typedef RequestHeader::iterator header_iterator;
typedef std::map<std::string, std::string> CGIEnvironment;
typedef RequestHeader::iterator cgi_iterator;

class HttpRequest : private RequestHeader
{
	friend class HttpRequestBuilder;

private:
	typedef HttpMethod e_http_method;
	typedef TransferEncoding e_transfer_encoding;
	typedef CGIEnvironment t_cgi_environment;

	t_cgi_environment m_cgi_env;
	e_transfer_encoding m_transfer_encoding_status = e_transfer_encoding::STATIC;
	size_t m_header_size;
	size_t m_body_size;
	std::vector<char> m_body;
	std::string m_path;
	std::string m_query;
	std::string m_version;
	std::string m_method;
	bool m_is_valid = false;

public:
	std::string request_address;

	HttpRequest();

	std::vector<char>& GetBody() 
	{
		return m_body;
	}

	std::string& GetPath()
	{
		return m_path;
	};

	std::string& GetVersion()
	{
		return m_version;
	};

	std::string& GetMethod()
	{
		return m_method;
	};

	std::string& GetQuery()
	{
		return m_query;
	};

	bool IsValid()
	{
		return m_is_valid;
	};

	int GetContentLength()
	{
		return m_body_size;
	};

	e_transfer_encoding GetTransferEncoding()
	{
		return m_transfer_encoding_status;
	};

};