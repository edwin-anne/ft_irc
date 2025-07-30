/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:28 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/26 13:54:58 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include <vector>
#include <string>
#include <sstream>

// Changement du nickname du client
void handleNick(Server& server, int fd, const std::vector<std::string>& tokens);
// Verification du mot de passe du client
void handlePass(Server& server, int fd, const std::vector<std::string>& tokens);
// Initialisation des infos de l'utilisateur
void handleUser(Server& server, int fd, const std::vector<std::string>& tokens);
// Repond au ping du client
void handlePing(Server& server, int fd, const std::vector<std::string>& tokens);
// Deconnecte le client
void handleQuit(Server& server, int fd, const std::vector<std::string>& tokens);
// Permet au client de rejoindre un channel
void handleJoin(Server& server, int fd, const std::vector<std::string>& tokens);
// Permet au client de quitter un channel
void handlePart(Server& server, int fd, const std::vector<std::string>& tokens);
// Envoie un message privé à un utilisateur et/ou channel
void handlePrivmsg(Server& server, int fd, const std::vector<std::string>& tokens);
// Affiche ou modifie le sujet d'un channel
void handleTopic(Server& server, int fd, const std::vector<std::string>& tokens);
// Exclut un utilisateur d'un channel
void handleKick(Server& server, int fd, const std::vector<std::string>& tokens);
// Invite un utilisateur a rejoindre un channel
void handleInvite(Server& server, int fd, const std::vector<std::string>& tokens);
// Lister les utilisateurs present sur un channel
void handleNames(Server& server, int fd, const std::vector<std::string>& tokens);
// Lister les channels existants sur le serveur
void handleList(Server& server, int fd, const std::vector<std::string>& tokens);
//
void handleMode(Server& server, int fd, const std::vector<std::string>& tokens);