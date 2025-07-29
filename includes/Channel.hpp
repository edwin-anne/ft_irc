/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:27 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/29 21:37:47 by Edwin ANNE       ###   ########.fr       */
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
		std::vector<int> operators; // Liste des opérateurs du channel
		std::vector<int> invited; // Liste des clients invités
		
		// Modes du channel
		bool inviteOnly; // Mode +i : Canal sur invitation uniquement
		bool topicRestricted; // Mode +t : Restrictions TOPIC pour opérateurs
		std::string key; // Mode +k : Clé du canal (mot de passe)
		int userLimit; // Mode +l : Limite d'utilisateurs (-1 = pas de limite)
		
	public:
		Channel(const std::string& channelName);
		
		// GET
		std::string GetName() const;
		std::string GetTopic() const;
		std::vector<int> GetClients() const;
		
		// SET
		void SetTopic(const std::string& newTopic);
		
		// Gestion Clients
		void AddClient(int fd);
		void RemoveClient(int fd);
		bool HasClient(int fd) const;
		size_t GetClientCount() const;
		
		// Gestion Opérateurs
		void AddOperator(int fd);
		void RemoveOperator(int fd);
		bool IsOperator(int fd) const;
		std::vector<int> GetOperators() const;
		
		// Gestion des modes
		// Mode +i/-i (invite only)
		void SetInviteOnly(bool enabled);
		bool IsInviteOnly() const;
		
		// Mode +t/-t (topic restricted)
		void SetTopicRestricted(bool enabled);
		bool IsTopicRestricted() const;
		
		// Mode +k/-k (key/password)
		void SetKey(const std::string& newKey);
		void RemoveKey();
		std::string GetKey() const;
		bool HasKey() const;
		bool CheckKey(const std::string& providedKey) const;
		
		// Mode +l/-l (user limit)
		void SetUserLimit(int limit);
		void RemoveUserLimit();
		int GetUserLimit() const;
		bool HasUserLimit() const;
		bool IsAtUserLimit() const;
		
		// Gestion des invitations
		void AddInvited(int fd);
		void RemoveInvited(int fd);
		bool IsInvited(int fd) const;
};

#endif