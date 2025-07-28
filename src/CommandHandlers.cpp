/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandlers.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 18:59:17 by loribeir          #+#    #+#             */
/*   Updated: 2025/07/28 10:07:18 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandlers.hpp"
#include "Channel.hpp"
#include "Client.hpp"


/**
 * handleNick
 * cmd NICK pour changer le pseudo d'un client.
 * Vérifie la disponibilité du pseudo et met à jour le client.
 */
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

/**
 * handlePass
 * cmd PASS pour l'authentification par mot de passe.
 * Vérifie le mot de passe et authentifie le client.
 */
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

/**
 * handleUser
 * cmd USER pour définir le nom d'utilisateur et le vrai nom.
 * Enregistre le client si toutes les informations sont fournies.
 */
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

/**
 * handlePing
 * cmd PING pour vérifier la connexion du client.
 * Répond PONG.
 */
void handlePing(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "409 :No origin specified\r\n");
        return;
    }
    server.SendMessage(fd, "PONG " + tokens[1] + "\r\n");
}

/**
 * handleQuit
 * cmd QUIT pour déconnecter un client.
 * Envoie un message de fermeture et supprime le client du serveur.
 */
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

/**
 * handleJoin
 * cmd JOIN pour rejoindre un channel.
 * Crée le channel si nécessaire et ajoute le client.
 */
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

/**
 * handlePart
 * cmd PART pour quitter un channel.
 * Retire le client du channel et informe les autres membres.
 */
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

/**
 * handlePrivmsg
 * cmd PRIVMSG pour envoyer un message privé à un utilisateur ou un channel.
 * Vérifie la validité du destinataire et du message.
 */
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

/**
 * handleTopic
 * cmd TOPIC pour afficher ou modifier le sujet d'un channel.
 * Vérifie les droits et met à jour le topic si nécessaire.
 */
void handleTopic(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "461 TOPIC :Not enough parameters\r\n");
        return;
    }
    std::string channelName = tokens[1];
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
    {
        server.SendMessage(fd, " 403 " + channelName + " :No such channel\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client || !channel->HasClient(fd))
    {
        server.SendMessage(fd, "442 " + channelName + " :You're not on that channel\r\n");
        return;
    }
    if (tokens.size() < 3)
    {
        std::string topic = channel->GetTopic();
        if (topic.empty())
            server.SendMessage(fd, "331 " + client->GetNickname() + " " + channelName + " :No topic is set\r\n");
        else
            server.SendMessage(fd, "332 " + client->GetNickname() + " " + channelName + " :" + topic + "\r\n");
        return;
    }
    std::string newTopic;
    if (tokens[2][0] == ':')
        newTopic = tokens[2].substr(1);
    else
        newTopic = tokens[2];
    for (size_t i = 3; i < tokens.size(); i++)
    {
        newTopic += " ";
        newTopic += tokens[i];
    }
    // TODO: Vérifier le mode +t ici

}

/**
 * handleKick
 * cmd KICK pour exclure un utilisateur d'un channel.
 * Vérifie les droits et retire le client ciblé du channel.
 */
void handleKick(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 3)
    {
        server.SendMessage(fd, "461 KICK :Not enough parameters\r\n");
        return;
    }
    std::string channelName = tokens[1];
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
    {
        server.SendMessage(fd, "403 " + channelName + ": No such channel\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client || !channel->HasClient(fd))
    {
        server.SendMessage(fd, "442 " + channelName + " :You're not on that channel\r\n");
        return;    
    }
    if (!channel->IsOperator(fd))
    {
        server.SendMessage(fd, "482 " + channelName + ": You're not channel operator\r\n");
        return;
    }
    std::string targetNick = tokens[2];
    Client *target = server.GetClientByNick(targetNick);
    if (!target || !channel->HasClient(target->GetFd()))
    {
        server.SendMessage(fd, "441 " + targetNick + " " + channelName + " :They aren't on that channel\r\n");
        return;
    }
    std::string comment = client->GetNickname();
    if (tokens.size() > 3)
    {
        if (tokens[3][0] == ':')
            comment = tokens[3].substr(1);
        else
            comment = tokens[3];
        for (size_t i = 4; i < tokens.size(); ++i)
        {
            comment += " ";
            comment += tokens[i];
        }
    }
    std::string kickMsg = ":" + client->GetNickname() + "KICK " + channelName + " " + targetNick + " :" + comment + "\r\n";
    server.BroadcastToChannel(channelName, kickMsg, -1);
    server.SendMessage(target->GetFd(), kickMsg);
    channel->RemoveClient(target->GetFd());
    target->LeaveChannel(channelName);

}

/**
 * handleInvite
 * cmd INVITE pour inviter un utilisateur dans un channel.
 * Vérifie les droits et ajoute l'utilisateur à la liste des invités.
 */
void handleInvite(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 3)
    {
        server.SendMessage(fd, "461 INVITE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = tokens[1];
    std::string inviterNick = tokens[2];
    
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
    {
        server.SendMessage(fd, "403 " + channelName + " :No such channel\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client || !channel->HasClient(fd))
    {
        server.SendMessage(fd, "442 " + channelName + " :You're not on that channel\r\n");
        return;
    }
    
    Client *inviter = server.GetClientByNick(inviterNick);
    if (!inviter)
    {
        server.SendMessage(fd, "401 " + inviterNick + " :No such nick/channel\r\n");
        return;
    }
    if (channel->HasClient(inviter->GetFd()))
    {
        server.SendMessage(fd, "443 " + inviterNick + " " + channelName + " :is already on channel\r\n");
        return;
    }

    channel->AddClient(inviter->GetFd());
    std::string inviteMsg = ":" + client->GetNickname() + " INVITE " + inviterNick + " :" + channelName + "\r\n";
    server.SendMessage(inviter->GetFd(), inviteMsg);
    server.SendMessage(fd, "341 " + client->GetNickname() + " " + inviterNick + " " + channelName + "\r\n");
}

/**
 * handleNames
 * cmd NAMES pour lister les membres d'un channel ou de tous les channels visibles.
 * Envoie la liste des pseudos au client.
 */
void handleNames(Server& server, int fd, const std::vector<std::string>& tokens)
{
    std::string channelName;
    if (tokens.size() > 1)
        channelName = tokens[1];

    Client *client = server.GetClientByFd(fd);
    std::string nick = client ? client->GetNickname() : "*";
    if (!channelName.empty())
    {
        Channel *channel = server.GetChannel(channelName);
        if (!channel)
        {
            server.SendMessage(fd, "366 " + nick + " " + channelName + " :End of /NAMES list.\r\n");
            return;
        }
        if ((channel->IsInviteOnly() || channel->IsTopicRestricted()) && !channel->HasClient(fd))
        {
            server.SendMessage(fd, "366 " + nick + " " + channelName + " :End of /NAMES list.\r\n");
            return;
        }
        std::vector<int> membersList = channel->GetClients();
        std::string namesList;
        for (size_t i = 0; i < membersList.size(); i++)
        {
            Client *member = server.GetClientByFd(membersList[i]);
            if (!member)
                continue;
            if (channel->IsOperator(membersList[i]))
                namesList += "@";
            namesList += member->GetNickname();
            if (i + 1 < membersList.size())
                namesList += " ";
        }
        server.SendMessage(fd, "353 " + nick + " = " + channelName + " :" + namesList + "\r\n");
        server.SendMessage(fd, "366 " + nick + " " + channelName + " :End of /NAMES list.\r\n");
    }
    else
    {
        for (size_t k = 0; k < server.channels.size(); ++k)
        {
            Channel &channel = server.channels[k];
            if ((channel.IsInviteOnly() || channel.IsTopicRestricted()) && !channel.HasClient(fd))
                continue;
            std::vector<int> membersList = channel.GetClients();
            std::string namesList;
            for (size_t i = 0; i < membersList.size(); i++)
            {
                Client *member = server.GetClientByFd(membersList[i]);
                if (!member)
                    continue;
                if (channel.IsOperator(membersList[i]))
                    namesList += "@";
                namesList += member->GetNickname();
                if (i + 1 < membersList.size())
                    namesList += " ";
            }
            server.SendMessage(fd, "353 " + nick + " = " + channel.GetName() + " :" + namesList + "\r\n");
            server.SendMessage(fd, "366 " + nick + " " + channel.GetName() + " :End of /NAMES list.\r\n");
        }
    }
}

/**
 * handleList
 * cmd LIST pour afficher la liste des channels et leur sujet.
 * Filtre selon les droits d'accès du client.
 */
void handleList(Server& server, int fd, const std::vector<std::string>& tokens)
{
    Client *client = server.GetClientByFd(fd);
    std::string nick;
    if (client)
        nick = client->GetNickname();
    else
        nick = "*";

    server.SendMessage(fd, "321 " + nick + " Channel :Users  Name\r\n");

    // si le channel est précisé
    if (tokens.size() > 1)
    {
        std::string channelName = tokens[1];
        Channel *channel = server.GetChannel(channelName);
        if (channel)
        {
            if ((channel->IsInviteOnly() || channel->IsTopicRestricted()) && !channel->HasClient(fd))
                return;
            std::string topic = channel->GetTopic();
            int userCount = channel->GetClientCount();
            std::ostringstream oss;
            oss << userCount;
            server.SendMessage(fd, "322 " + nick + " " + channel->GetName() + " " + oss.str() + " :" + topic + "\r\n");
        }
    }
    else
    {
        // sinon, listing de tous les channels visible
        for (size_t i = 0; i < server.channels.size(); i++)
        {
            Channel &channel = server.channels[i];
            if ((channel.IsInviteOnly() || channel.IsTopicRestricted()) && !channel.HasClient(fd))
                continue;
            std::string topic = channel.GetTopic();
            int userCount = channel.GetClientCount();
            std::ostringstream oss;
            oss << userCount;
            server.SendMessage(fd, "322 " + nick + " " + channel.GetName() + " " + oss.str() + " :" + topic + "\r\n");
        }
    }
    server.SendMessage(fd, "323 " + nick + " :End of /LIST\r\n");
}

/** 
 * handleMode
 * cmd MODE pur gérer les mode : +i, +t, +k, +l
 */
void handleMode(Server& server, int fd, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        server.SendMessage(fd, "461 MODE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = tokens[1];
    Channel *channel = server.GetChannel(channelName);
    if (!channel)
    {
        server.SendMessage(fd, "482 " + channelName + " :No such channel\r\n");
        return;
    }
    Client *client = server.GetClientByFd(fd);
    if (!client || !channel->IsOperator(fd))
    {
        server.SendMessage(fd, "482 " + channelName + " :You're not channel operator\r\n");
        return;
    }
    if (tokens.size() == 2)
    {
        std::string modes = "+";
        if (channel->IsInviteOnly()) modes += "i";
        if (channel->IsTopicRestricted()) modes += "t";
        if (channel->HasKey()) modes += "k";
        if (channel->GetUserLimit() > 0) modes += "l";
        server.SendMessage(fd, "324 " + client->GetNickname() + " " + channelName + " " + modes + "\r\n");
        return;
    }
    
}