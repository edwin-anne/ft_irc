/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:43 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/13 21:04:42 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>

void Channel::AddClient(int fd) {
	if (!HasClient(fd))
		clients.push_back(fd);
}

void Channel::RemoveClient(int fd) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i] == fd) {
			clients.erase(clients.begin() + i);
			break ;
		}
	}
}

bool Channel::HasClient(int fd) const {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i] == fd)
			return (true);
	}
	return (false);
}
