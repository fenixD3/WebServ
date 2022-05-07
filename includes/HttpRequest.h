/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 16:51:15 by zytrams           #+#    #+#             */
/*   Updated: 2022/05/07 20:29:37 by zytrams          ###   ########.fr       */
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

typedef std::map<std::string, std::string> RequestHeader;
typedef RequestHeader::iterator header_iterator;

class HttpRequest : private RequestHeader
{
    friend class HttpRequestBuilder;

    private:
        typedef HttpMethod e_http_method;

        size_t m_size;
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

        bool IsValid()
        {
            return m_is_valid;
        }
};