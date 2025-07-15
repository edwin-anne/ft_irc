/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:45 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/13 21:04:44 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <algorithm>

void Client::JoinChannel(const std::string& channelName) {
	if (!IsInChannel(channelName))
		channels.push_back(channelName);
}

void Client::LeaveChannel(const std::string& channelName) {
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i] == channelName) {
			channels.erase(channels.begin() + i);
			break;
		}
	}
}

bool Client::IsInChannel(const std::string& channelName) const {
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i] == channelName)
			return (true);
	}
	return (false);
}
