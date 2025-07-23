/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:43 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/23 13:13:13 by Edwin ANNE       ###   ########.fr       */
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

void Channel::AddOperator(int fd) {
	if (!IsOperator(fd))
		operators.push_back(fd);
}

void Channel::RemoveOperator(int fd) {
	for (size_t i = 0; i < operators.size(); i++) {
		if (operators[i] == fd) {
			operators.erase(operators.begin() + i);
			break;
		}
	}
}

bool Channel::IsOperator(int fd) const {
	for (size_t i = 0; i < operators.size(); i++) {
		if (operators[i] == fd)
			return (true);
	}
	return (false);
}
