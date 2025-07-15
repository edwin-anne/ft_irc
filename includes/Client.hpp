/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/11 21:03:32 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/15 11:54:07 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <iostream>


class Client
{
	private:
		int Fd;
		std::string IPadd; // Adresse IP du client
		std::string nickname; // Pseudo du client
		std::string username; // Nom d'utilisateur du client
		std::string realname; // Nom réel du client
		bool authenticated; // Indique si le client est authentifié
		bool registered; // Indique si le client est enregistré
		std::vector<std::string> channels; // Liste des channels auxquels le client est inscrit
		
	public:
		Client() : Fd(-1), authenticated(false), registered(false) {}
		
		// GET
		int GetFd() const { return Fd; }
		std::string GetIPadd() const { return IPadd; }
		std::string GetNickname() const { return nickname; }
		std::string GetUsername() const { return username; }
		std::string GetRealname() const { return realname; }
		bool IsAuthenticated() const { return authenticated; }
		bool IsRegistered() const { return registered; }
		std::vector<std::string> GetChannels() const { return channels; }

		// SET
		void SetFd(int fd) { Fd = fd; }
		void setIpAdd(const std::string& ipadd) { IPadd = ipadd; }
		void SetNickname(const std::string& nick) { nickname = nick; }
		void SetUsername(const std::string& user) { username = user; }
		void SetRealname(const std::string& real) { realname = real; }
		void SetAuthenticated(bool auth) { authenticated = auth; }
		void SetRegistered(bool reg) { registered = reg; }
		
		// Gestion des channels
		void JoinChannel(const std::string& channelName);
		void LeaveChannel(const std::string& channelName);
		bool IsInChannel(const std::string& channelName) const;
};


#endif