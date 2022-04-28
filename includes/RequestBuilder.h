/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/28 23:05:52 by marvin            #+#    #+#             */
/*   Updated: 2022/04/28 23:05:52 by marvin           ###   ########.fr       */
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
    OTHER
};

inline const char* ToString(HttpMethod value)
{
    switch (value)
    {
        case GET:       return "GET";
        case POST:      return "POST";
        case DELETE:    return "DELETE";
        //case PUT:       return "PUT";
        default:        return "OTHER";
    }
}

class HttpRequestBuilder
{
    private:
        typedef std::string type_tag;
        typedef std::string type_value;
        typedef std::map<type_tag, type_value> RequestHeader;
        typedef RequestHeader::iterator header_iterator;

        typedef HttpMethod e_http_method;

        const char* c_message_line = "/r/n/";
        const char* c_message_end = "/r/n/r/n";

        // TODO make private map header
        class HttpRequest : public RequestHeader
        {
            private:
                size_t m_size;
                std::vector<char> m_body; // maybe std::array due no dynamic change
                
            public:
                HttpRequest(
                    size_t size,
                    std::vector<char>& body,
                    e_http_method http_method)
                : m_size(m_size)
                , m_body(body)
                {
                    // TODO Add all headers tags
                    this->emplace("METHOD", ToString(http_method));
                }
                
                std::vector<char>& GetBody() 
                {
                    return m_body;
                }
        };

    public:
        typedef HttpRequest http_request;

        const HttpRequestBuilder& GetInstance()
        {
            static HttpRequestBuilder m_instance;
            return m_instance;
        }

        http_request BuildHttpRequest(const char* raw_msg, size_t msg_size)
        {
            //http_request req;
            //add additional tags parsing
        }

    private:
        HttpRequestBuilder();
        HttpRequestBuilder(HttpRequestBuilder const&);
        void operator=(HttpRequestBuilder const&);


};