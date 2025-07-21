/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:17 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/21 21:30:59 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandlers.hpp"

void handleNick(Server& server, int fd, const std::vector<std::string>& tokens) 
{
    if (tokens.size() < 2) 
    {
        server.SendMessage(fd, "431 :No nickname given\r\n");
        return;
    }
    std::string newNick = tokens[1];
    if (server.IsNicknameTaken(newNick)) 
    {
        server.SendMessage(fd, "433 " + newNick + " :Nickname is already in use\r\n");
        return;
    }
    Client* client = server.GetClientByFd(fd);
    if (client) 
    {
        client->SetNickname(newNick);
        server.SendMessage(fd, "NICK :" + newNick + "\r\n");
    }
}

/*void handlePass(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) 
    {
        server.SendMessage(fd, "461 PASS :Not enough parameters\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client)
        return;
    if (client->IsAuthenticated())
    {
        server.SendMessage(fd, "462 :You may not reregister\r\n");
        return;
    }
    if (tokens[1] == server.getPassword()) // -> besoin d'implémenter un getter pour un attr privé.
    {
        client->SetAuthenticated(true);
        server.SendMessage(fd, "NOTICE AUTH : Password accepted\r\n");
    } else {
        server.SendMessage(fd, "464 : Password incorrect\r\n");
    }
}*/
void handleUser(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 5)
    {
        server.SendMessage(fd, "461 USER :Not enough parameters\r\n");
        return;
    }   
    Client *client = server.GetClientByFd(fd);
    if (!client)
        return;
    client->SetUsername(tokens[1]);
    std::string realname;
    if (tokens[4][0] == ':')
        realname = tokens[4].substr(1);
    else
        realname = tokens[4];
    for (size_t i = 5; i < tokens.size(); i++)
        realname += " " + tokens[i];
    client->SetRealname(realname);
    if (client->IsRegistered()) 
    {
        server.SendMessage(fd, "462 :You may not reregister\r\n");
        return;
    }
    if (!client->GetNickname().empty())
    {
        client->SetRegistered(true);
        server.SendWelcome(fd);
    }
}

void handlePing(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "409 :No irigin specified\r\n");
        return;
    }
}
/*void handleQuit(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handleJoin(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handlePart(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handlePrivmsg(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}*/