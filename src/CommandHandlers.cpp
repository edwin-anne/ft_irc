/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:17 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/21 22:52:02 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandlers.hpp"
#include "Channel.hpp"

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

void handlePass(Server& server, int fd, const std::vector<std::string>& tokens)
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
    if (tokens[1] == server.getPassword())
    {
        client->SetAuthenticated(true);
        server.SendMessage(fd, "NOTICE AUTH : Password accepted\r\n");
    } else {
        server.SendMessage(fd, "464 : Password incorrect\r\n");
    }
}

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
        server.SendMessage(fd, "409 :No origin specified\r\n");
        return;
    }
    server.SendMessage(fd, "PONG " + tokens[1] + "\r\n");
}
void handleQuit(Server& server, int fd, const std::vector<std::string>& tokens)
{
    Client *client = server.GetClientByFd(fd);
    if (!client)
        return;
    std::string quitMsg = "Client quit";
    if (tokens.size() > 1)
    {
        quitMsg = tokens[1];
        for (size_t i = 2; i < tokens.size(); i++)
            quitMsg += " " + tokens[i];
    }
    server.SendMessage(fd, "ERROR :Closing Link: " + quitMsg + "\r\n");
    server.ClearClients(fd);
}
void handleJoin(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "461 JOIN :Not enough parameters\r\n");
        return;
    }
    std::string channelName = tokens[1];
    if (channelName.empty() || (channelName[0] != '#' && channelName[0] != '&'))
    {
        server.SendMessage(fd, "476 " + channelName + " :Invalid channel name\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client)
        return;
    if (!client->IsRegistered())
    {
        server.SendMessage(fd, "451 JOIN :You have not registered\r\n");
        return;
    }
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
        channel = server.CreateChannel(channelName);
    channel->AddClient(fd);
    server.SendMessage(fd, ":" + client->GetNickname() + " JOIN " + channelName + "\r\n");
}
void handlePart(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "461 PART :Not enough parameters\r\n");
        return;
    }
    
}
/*void handlePrivmsg(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}*/