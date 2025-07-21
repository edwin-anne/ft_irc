/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:28 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/21 19:18:12 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include <vector>
#include <string>

// Les premières commandes
void handleNick(Server& server, int fd, const std::vector<std::string>& tokens);
void handlePass(Server& server, int fd, const std::vector<std::string>& tokens);
void handleUser(Server& server, int fd, const std::vector<std::string>& tokens);
void handlePing(Server& server, int fd, const std::vector<std::string>& tokens);
void handleQuit(Server& server, int fd, const std::vector<std::string>& tokens);
void handleJoin(Server& server, int fd, const std::vector<std::string>& tokens);
void handlePart(Server& server, int fd, const std::vector<std::string>& tokens);
void handlePrivmsg(Server& server, int fd, const std::vector<std::string>& tokens);

