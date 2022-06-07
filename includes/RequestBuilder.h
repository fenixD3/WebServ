/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestBuilder.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/28 23:05:52 by marvin            #+#    #+#             */
/*   Updated: 2022/06/06 22:19:16 by zytrams          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "HttpRequest.h"

class HttpRequestBuilder
{
friend class HttpRequest;

private:
	typedef std::string type_tag;
	typedef std::string type_value;

	typedef HttpMethod e_http_method;

	const char* c_message_line;
	const char* c_message_end;

public:
	typedef HttpRequest http_request;

	static HttpRequestBuilder& GetInstance();

	std::pair<bool, std::string> BuildHttpRequestHeader(const std::string& msg, http_request& http_req);
	void BuildHttpRequestBody(HttpRequestBuilder::http_request& http_req, const std::string& msg);

private:
	HttpRequestBuilder();
	HttpRequestBuilder(HttpRequestBuilder const&);
	void operator=(HttpRequestBuilder const&);

	bool ParseInitialFields(HttpRequestBuilder::http_request& req, const std::string msg);
	std::string GetNext(const std::string& msg, size_t& cur);
	void GetQuery(HttpRequestBuilder::http_request& req);
	void ParseKey(std::string& key, const  std::string& line);
	void ParseValue(std::string& key, const  std::string& line);
	bool ParseBoundary(std::string& boundary, const std::string& line);
	std::string MakeHeaderForCGI(std::string& key);
	
	inline static int CGIFormatter(int value)
	{
		if (value == '-')
			return '_';
		else
			return std::toupper(value);
	}
};