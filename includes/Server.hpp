/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Edwin ANNE <eanne@student.42lehavre.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 19:03:40 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/29 21:37:47 by Edwin ANNE       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <vector>
#include <string>
#include <iostream>

#include "Client.hpp"
#include "Channel.hpp"


class Server
{
	private:
		int Port; // Port découte
		int SerSocketFd; // fd du socket du serveur
		std::string Password; // Mot de passe du serveur
		static bool Signal; // Variable pour gérer les signaux
		std::vector<Client> clients; // Vecteur de clients connectés
		std::vector<struct pollfd> fds; //fd pour envoyer des données aux clients
		
	public:
		std::vector<Channel> channels; // Vecteur de channels
		
	public:
		Server() : SerSocketFd(-1) {}

		// Gestion Serveur
		void ServerInit(int port, char *password); // Initialisation du serveur
		void SerSocket(); // Création du socket du serveur
		void ServerLoop(); // Boucle principale du serveur pour le laisser tourner tout au long du programme

		// Gestion Clients
		void AcceptNewClient(); // Accepte un nouveau client
		void ReceiveNewData(int fd); // Reçoit des données d'un client
		void ClearClients(int fd); // Un client a quitter Supprime un client de la liste des clients connectés
		
		// Gestion Commandes
		void ParseCommand(int fd, std::string &cmd); // Analyse et traite les commandes reçues des clients
		
		// Gestion Channels
		Channel* GetChannel(const std::string& channelName); // Récupère un channel par son nom
		Channel* CreateChannel(const std::string& channelName); // Crée un nouveau channel
		void BroadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd = -1); // Envoie un message à tous les clients d'un channel, sauf celui spécifié
		
		// Utils
		void SendMessage(int fd, const std::string &message); // Envoie un message à un client spécifique
		void SendWelcome(int fd); // Envoie un message de bienvenue à un client
		Client* GetClientByFd(int fd); // Récupère un client par son descripteur de fichier
		Client* GetClientByNick(const std::string& nickname); // Récupère un client par son pseudo
		bool IsNicknameTaken(const std::string &nickname); // Vérifie si un pseudo est déjà pris
		std::string getPassword() const; // Getter pour le mot de passe du serveur

		// Signal
		static void SignalHandler(int signum); // Gestion des signaux
		void CloseFds();	 // Ferme les descripteurs de fichiers ouverts
};

#endif