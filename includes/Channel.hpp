/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:27 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/15 11:54:30 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <iostream>


class Channel
{
	private:
		std::string name; // Nom du channel
		std::string topic; // Sujet du channel
		std::vector<int> clients; // Liste des descripteurs de fichiers des clients connectés au channel
		
	public:
		Channel(const std::string& channelName) : name(channelName) {}
		
		// GET
		std::string GetName() const { return name; }
		std::string GetTopic() const { return topic; }
		std::vector<int> GetClients() const { return clients; }
		
		// SET
		void SetTopic(const std::string& newTopic) { topic = newTopic; }
		
		// Gestion Clients
		void AddClient(int fd);
		void RemoveClient(int fd);
		bool HasClient(int fd) const;
		size_t GetClientCount() const { return clients.size(); }
};

#endif