/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zytrams <zytrams@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/07 18:18:40 by zytrams           #+#    #+#             */
/*   Updated: 2022/06/06 21:09:16 by zytrams          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.h"

HttpRequest::HttpRequest()
	: m_transfer_encoding_status(STATIC)
	, m_is_valid(false)
	, m_need_boundery_checks(false)
{}
