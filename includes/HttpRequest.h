/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 16:51:15 by zytrams           #+#    #+#             */
/*   Updated: 2022/06/06 22:51:46 by zytrams          ###   ########.fr       */
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
	HEAD,
	PUT,
	UNKNOWN
};

inline const char* ToString(HttpMethod value)
{
	switch (value)
	{
		case GET:       return "GET";
		case POST:      return "POST";
		case DELETE:    return "DELETE";
		case HEAD:      return "HEAD";
		case PUT:       return "PUT";
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
	else if (value == "HEAD")
		return HEAD;
    else if (value == "PUT")
        return PUT;
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

class HttpRequest : public RequestHeader
{
	friend class HttpRequestBuilder;

private:
	typedef HttpMethod e_http_method;
	typedef TransferEncoding e_transfer_encoding;
	typedef CGIEnvironment t_cgi_environment;

	t_cgi_environment m_cgi_env;
	e_transfer_encoding m_transfer_encoding_status;
	size_t m_header_size;
	size_t m_body_size;
	std::vector<char> m_body;
	std::string m_path;
	std::string m_query;
	std::string m_version;
	std::string m_method;
	std::string m_boundary;
	bool m_is_valid;

public:

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

	std::string& GetBoundary()
	{
		return m_boundary;
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

	std::string ToString()
	{
		std::string out = "";

		out += "****** Metadata ******\n";
		out += "Method : " + m_method + "\n";
		out += "Path : " + m_path + "\n";
		out += "Version : " + m_version + "\n";
		out += "Query : " + m_query + "\n";
		out += "Header Size : " + std::to_string(m_header_size) + "\n";
		out += "Body Size : " + std::to_string(m_body_size) + "\n";
		out += "Boundary : " + m_boundary + "\n";
		
		out += "****** Header ******\n";
		header_iterator itCur = this->begin();
		while (itCur != this->end())
		{
			out += itCur->first + " : " + itCur->second + "\n";
			itCur++;
		}
		out += "****** Body ******\n";
		std::vector<char>::iterator itBodyCur = m_body.begin();
		while (itBodyCur != m_body.end())
		{
			out += *itBodyCur;
			if (itBodyCur + 1 == m_body.end())
			{
				out += '\n';
			}
			itBodyCur++;
		}
		out += "****** End ******\n";
		return out;
	};

};