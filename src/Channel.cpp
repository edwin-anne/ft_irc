/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:43 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/30 10:59:06 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include <algorithm>

Channel::Channel(const std::string& channelName) : name(channelName), topic(""), inviteOnly(false), topicRestricted(false), key(""), userLimit(-1) {}

Channel::Channel(const Channel& other) 
	: name(other.name), topic(other.topic), clients(other.clients), operators(other.operators), invited(other.invited),
	  inviteOnly(other.inviteOnly), topicRestricted(other.topicRestricted), key(other.key), userLimit(other.userLimit) {}

Channel& Channel::operator=(const Channel& other) {
	if (this != &other) {
		name = other.name;
		topic = other.topic;
		clients = other.clients;
		operators = other.operators;
		invited = other.invited;
		inviteOnly = other.inviteOnly;
		topicRestricted = other.topicRestricted;
		key = other.key;
		userLimit = other.userLimit;
	}
	return *this;
}

Channel::~Channel() {
	
}

std::string Channel::GetName() const { 
	return name; 
}

std::string Channel::GetTopic() const { 
	return topic; 
}

std::vector<int> Channel::GetClients() const { 
	return clients; 
}

void Channel::SetTopic(const std::string& newTopic) { 
	topic = newTopic; 
}

size_t Channel::GetClientCount() const { 
	return clients.size(); 
}

std::vector<int> Channel::GetOperators() const { 
	return operators; 
}

void Channel::SetInviteOnly(bool enabled) { 
	inviteOnly = enabled; 
}

bool Channel::IsInviteOnly() const { 
	return inviteOnly; 
}

void Channel::SetTopicRestricted(bool enabled) { 
	topicRestricted = enabled; 
}

bool Channel::IsTopicRestricted() const { 
	return topicRestricted; 
}

void Channel::SetKey(const std::string& newKey) { 
	key = newKey; 
}

void Channel::RemoveKey() { 
	key.clear(); 
}

std::string Channel::GetKey() const { 
	return key; 
}

bool Channel::HasKey() const { 
	return !key.empty(); 
}

bool Channel::CheckKey(const std::string& providedKey) const { 
	return key == providedKey; 
}

void Channel::SetUserLimit(int limit) { 
	userLimit = limit; 
}

void Channel::RemoveUserLimit() { 
	userLimit = -1; 
}

int Channel::GetUserLimit() const { 
	return userLimit; 
}

bool Channel::HasUserLimit() const { 
	return userLimit > 0; 
}

bool Channel::IsAtUserLimit() const { 
	return HasUserLimit() && (int)clients.size() >= userLimit; 
}

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

void Channel::AddInvited(int fd) {
	if (!IsInvited(fd))
		invited.push_back(fd);
}

void Channel::RemoveInvited(int fd) {
	for (size_t i = 0; i < invited.size(); i++) {
		if (invited[i] == fd) {
			invited.erase(invited.begin() + i);
			break;
		}
	}
}

bool Channel::IsInvited(int fd) const {
	for (size_t i = 0; i < invited.size(); i++) {
		if (invited[i] == fd)
			return true;
	}
	return false;
}
