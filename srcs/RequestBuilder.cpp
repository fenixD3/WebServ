/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 18:20:39 by zytrams           #+#    #+#             */
/*   Updated: 2022/05/19 21:15:51 by zytrams          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestBuilder.h"

HttpRequestBuilder& HttpRequestBuilder::GetInstance()
{
	static HttpRequestBuilder m_instance;
	return m_instance;
}

bool HttpRequestBuilder::ParseInitialFields(HttpRequestBuilder::http_request& req, const std::string msg)
{
	size_t	i, j;
	std::string	line;

	// METHOD
	i = msg.find_first_of('\n');
	line = msg.substr(0, i);
	i = line.find_first_of(' ');

	if (i == std::string::npos)
	{
		std::cerr << "RFL no space after method" << std::endl;
		return false;
	}
	req.m_method.assign(line, 0, i);
	if (ToHttpMethod(req.m_method) == UNKNOWN)
	{
		std::cerr << "Invalid method requested" << std::endl;
		return false;
	}

	// PATH
	if ((j = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		std::cerr << "No PATH / HTTP version" << std::endl;
		return false;
	}
	if ((i = line.find_first_of(' ', j)) == std::string::npos)
	{
		std::cerr <<  "No HTTP version" << std::endl;
		return false;
	}
	req.m_path.assign(line, j, i - j);

	// VERSION
	if ((i = line.find_first_not_of(' ', i)) == std::string::npos)
	{
		std::cerr << "No HTTP version" << std::endl;
		return false;
	}
	if (line[i] == 'H' && line[i + 1] == 'T' && line[i + 2] == 'T' && line[i + 3] == 'P' && line[i + 4] == '/')
		req.m_version.assign(line, i + 5, 3);
	if (req.m_version != "1.0" && req.m_version != "1.1")
	{
		std::cerr << "BAD HTTP VERSION (" << req.m_version << ")" << std::endl;
		return false;
	}

	return true;
}

std::string HttpRequestBuilder::GetNext(const std::string& msg, size_t& cur)
{
	std::string		ret;
	size_t			j;

	if (cur == std::string::npos)
		return "";
	j = msg.find_first_of('\n', cur);
	ret = msg.substr(cur, j - cur);
	if (ret[ret.size() - 1] == '\r')
	{
		if (ret.size())
			ret.resize(ret.size() - 1);
	}
	cur = (j == std::string::npos ? j : j + 1);
	return ret;
}

void HttpRequestBuilder::GetQuery(HttpRequestBuilder::http_request& req)
{
	size_t			i;

	i = req.m_path.find_first_of('?');
	if (i != std::string::npos)
	{
		req.m_query.assign(req.m_path, i + 1, std::string::npos);
		req.m_path = req.m_path.substr(0, i);
	}
}

void HttpRequestBuilder::ParseKey(std::string& key, const  std::string& line)
{
	std::string	ret;

	size_t	i = line.find_first_of(':');
	key.append(line, 0 , i);
}

void HttpRequestBuilder::ParseValue(std::string& value, const std::string& line)
{
	size_t i;
	std::string	ret;

	i = line.find_first_of(':');
	i = line.find_first_not_of(' ', i + 1);
	if (i != std::string::npos)
		value.append(line, i, std::string::npos);
}

std::string HttpRequestBuilder::MakeHeaderForCGI(std::string& key)
{
	std::transform(key.begin(), key.end(), key.begin(), CGIFormatter);
	return "HTTP_" + key;
}

HttpRequestBuilder::http_request HttpRequestBuilder::BuildHttpRequestHeader(const std::string& msg)
{
	HttpRequestBuilder::http_request http_req;
	std::string current;
	std::string key;
	std::string value;
	bool is_valid = true;
	size_t cur_size = 0;
	std::cerr << "Start building request" << std::endl;

	is_valid = ParseInitialFields(http_req, GetNext(msg, cur_size));

	while ((current = GetNext(msg, cur_size)) != "\r" && current != "" && is_valid)
	{
		ParseKey(key, current);
		ParseValue(value, current);
		if (key.find("Secret") != std::string::npos)
			http_req.m_cgi_env[MakeHeaderForCGI(key)] = value;
		else
		{
			http_req[key] = value;
		}
	}
	
	header_iterator itWWWAuth = http_req.find("Www-Authenticate");
	if (itWWWAuth != http_req.end())
	{
		http_req.m_cgi_env["Www-Authenticate"] = itWWWAuth->second;
	}

	// Если нет Transfer-Encoding , то Content-Length должен быть, если тело запроса ненулевое
	// Если нет Transfer-Encoding , но Content-Length отсутствует, то тело запроса нулевое (вроде так, монжно перепроверить, правда ли Content-Length может не быть в этом случае)
	// Если есть Transfer-Encoding: chunked, то на Content-Length вообще пофиг (его и не должно там быть). Тогда там свой паттерн нахождение конца тела '0\r\n\r\n'
	// Для других Transfer-Encoding не нашел инфы
	header_iterator itTransferEncoding = http_req.find("Transfer-Encoding");
	if (itTransferEncoding == http_req.end())
	{
		const size_t last_sequence_size = 2; /// For the last \r\n
		header_iterator itConLen = http_req.find("Content-Length");
		if (itConLen == http_req.end())
		{
			http_req.m_body_size = 0;
		}
		else
		{
			http_req.m_body_size = std::stoi(itConLen->second);
		}
		http_req.m_body_size += last_sequence_size;
	}
	else
	{
		http_req.m_transfer_encoding_status = ToTransferEncoding(itTransferEncoding->second);
	}

	http_req.m_is_valid = is_valid;
	http_req.m_header_size = cur_size;
	std::cerr << "Read request header with validity status: " + std::to_string(http_req.m_is_valid)  << std::endl;
	return http_req;
}

void HttpRequestBuilder::BuildHttpRequestBody(HttpRequestBuilder::http_request& http_req, const std::string& msg)
{
	std::string current;

	if (http_req.m_is_valid)
	{
		current = msg.substr(http_req.m_header_size, std::string::npos);
		http_req.m_body = std::vector<char>(current.begin(), current.end());
		std::cerr << "Read request body with size: " << std::to_string(http_req.m_body.size())  << std::endl;
		GetQuery(http_req);
	}
}