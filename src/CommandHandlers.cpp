/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:17 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/25 00:01:42 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandlers.hpp"
#include "Channel.hpp"
#include "Client.hpp"

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
    std::string channelName = tokens[1];
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
    {
        server.SendMessage(fd, "403 " + channelName + " :No such channel\r\n");
        return;
    }
    if (!channel->HasClient(fd))
    {
        server.SendMessage(fd, "442 " + channelName + " :You're not on that channel\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (client)
        client->LeaveChannel(channelName);
    channel->RemoveClient(fd);
    std::string partMsg = ":" + client->GetNickname() + " PART " + channelName + "\r\n";
    server.BroadcastToChannel(channelName, partMsg, fd);
    server.SendMessage(fd, partMsg);
}

void handlePrivmsg(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 3)
    {
        server.SendMessage(fd, "411 PRIVMSG :No recipient given\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client || !client->IsRegistered())
    {
        server.SendMessage(fd, "451 PRIVMSG :You have not registered\r\n");
        return;
    }
    std::string dest = tokens[1];
    
    // Rebuild le message
    std::string message;
    if (tokens[2][0] == ':')
        message = tokens[2].substr(1);
    else
        message = tokens[2];
    for (size_t i = 3; i < tokens.size(); ++i)
    {
        message += " ";
        message += tokens[i];
    }
    if (message.empty())
    {
        server.SendMessage(fd, "412 PRIVMSG :No text to send\r\n");
        return;
    }
    if (dest[0] == '#' || dest[0] == '&')
    {
        Channel *channel = server.GetChannel(dest);
        if (!channel)
        {
            server.SendMessage(fd, "401 " + dest + " :No such nick/channel\r\n");
            return;
        }
        if (!channel->HasClient(fd))
        {
            server.SendMessage(fd, "404 " + dest + " :Cannot send to channel\r\n");
            return;
        }
        std::string privmsg = ":" + client->GetNickname() + " PRIVMSG " + dest + " :" + message + "\r\n";
        server.BroadcastToChannel(dest, privmsg, fd);
    }
    else
    {
        Client *target = server.GetClientByNick(dest);
        if (!target)
        {
            server.SendMessage(fd, "401 " + dest + " :No such nick/channel\r\n");
            return;
        }
        std::string privmsg = ":" + client->GetNickname() + " PRIVMSG " + dest + " :" + message + "\r\n";
        server.SendMessage(target->GetFd(), privmsg);
    }
}
void handleTopic(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "461 TOPIC :Not enough parameters\r\n");
        return;
    }  
}
void handleKick(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handleInvite(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handleNames(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}
void handleList(Server& server, int fd, const std::vector<std::string>& tokens)
{
    
}