/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: loribeir <loribeir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 19:03:50 by Edwin ANNE        #+#    #+#             */
/*   Updated: 2025/07/24 23:29:44 by loribeir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "irc.hpp"
#include "CommandHandlers.hpp"

bool Server::Signal = false;
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void Server::CloseFds() {
	for(size_t i = 0; i < clients.size(); i++) {
		std::cout << RED << "Client <" << clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
		close(clients[i].GetFd());
	}
	if (SerSocketFd != -1) {
		std::cout << RED << "Server <" << SerSocketFd << "> Disconnected" << WHI << std::endl;
		close(SerSocketFd);
	}
}

void Server::SerSocket() {
	struct sockaddr_in add;
	struct pollfd NewPoll;
	add.sin_family = AF_INET;
	add.sin_port = htons(this->Port);
	add.sin_addr.s_addr = INADDR_ANY;

	SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if(SerSocketFd == -1)
		throw(std::runtime_error("faild to create socket"));

	int en = 1;
	if(setsockopt(SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	if (fcntl(SerSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));
	if (bind(SerSocketFd, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("faild to bind socket"));
	if (listen(SerSocketFd, SOMAXCONN) == -1)
		throw(std::runtime_error("listen() faild"));

	NewPoll.fd = SerSocketFd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;
	fds.push_back(NewPoll);
}

void Server::ClearClients(int fd) {
	Client *client = GetClientByFd(fd);
	if (client) {
		std::vector<std::string> clientChannels = client->GetChannels();
		for (size_t i = 0; i < clientChannels.size(); i++) {
			Channel *channel = GetChannel(clientChannels[i]);
			if (channel) {
				std::string quitMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " QUIT :Client disconnected\r\n";
				BroadcastToChannel(clientChannels[i], quitMsg, fd);
				channel->RemoveClient(fd);
			}
		}
	}
	
	for (size_t i = 0; i < fds.size(); i++) {
		if (fds[i].fd == fd) {
			fds.erase(fds.begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].GetFd() == fd) {
			clients.erase(clients.begin() + i);
			break;
		}
	}
}

void Server::ServerInit(int port, char *password) {
	this->Port = port;
	this->Password = password;
	(void)password;
	SerSocket();

	std::cout << GRE << "Server <" << SerSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::AcceptNewClient() {
	Client cli;
	struct sockaddr_in cliaddr;
	struct pollfd NewPoll;
	socklen_t len = sizeof(cliaddr);

	int incofd = accept(SerSocketFd, (sockaddr *)&cliaddr, &len);
	if (incofd == -1) {
		std::cout << "accept() failed" << std::endl;
		return;
	}

	if (fcntl(incofd, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "fcntl() failed" << std::endl;
		close(incofd);
		return;
	}

	NewPoll.fd = incofd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	cli.SetFd(incofd);
	cli.setIpAdd(inet_ntoa(cliaddr.sin_addr));
	clients.push_back(cli);
	fds.push_back(NewPoll);

	std::cout << GRE << "Client <" << incofd << "> Connected" << WHI << std::endl;
}

void Server::ReceiveNewData(int fd) {
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	if (bytes <= 0) {
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	} else {
		buff[bytes] = '\0';
		std::cout << YEL << "Client <" << fd << "> Data: " << buff << WHI;
		std::string message(buff);
		ParseCommand(fd, message);
	}
}

void Server::ParseCommand(int fd, std::string &cmd) {
	size_t pos = cmd.find_first_of("\r\n");
	if (pos != std::string::npos)
		cmd = cmd.substr(0, pos);
	if (cmd.empty())
		return;
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream iss(cmd);
	while (iss >> token)
		tokens.push_back(token);
	if (tokens.empty())
		return;
	std::string command = tokens[0];
	for (size_t i = 0; i < command.length(); i++)
		command[i] = std::toupper(command[i]);

	// Une commande est reçue
	// Les informations a la suite de la commande sont dans tokens
	if (command == "PASS")
		handlePass(*this, fd, tokens);
	else if (command == "NICK")
		handleNick(*this, fd, tokens);
	else if (command == "USER")
		handleUser(*this, fd, tokens);
	else if (command == "PING")
		handlePing(*this, fd, tokens);
	else if (command == "QUIT")
		handleQuit(*this, fd, tokens);
	else if (command == "JOIN")
		handleJoin(*this, fd, tokens);
	else if (command == "PART")
		;
	else if (command == "PRIVMSG")
		handlePrivmsg(*this, fd, tokens);
	else
		SendMessage(fd, "421 " + command + " :Unknown command\r\n");
}

void Server::SendMessage(int fd, const std::string &message) {
	send(fd, message.c_str(), message.length(), 0);
}

void Server::SendWelcome(int fd) {
	Client *client = GetClientByFd(fd);
	if (!client)
		return;
	std::string nick = client->GetNickname();
	SendMessage(fd, "001 " + nick + " :Welcome to the IRC Network " + nick + "\r\n");
	SendMessage(fd, "002 " + nick + " :Your host is ircserv, running version 1.0\r\n");
	SendMessage(fd, "003 " + nick + " :This server was created today\r\n");
	SendMessage(fd, "004 " + nick + " ircserv 1.0 o o\r\n");
	std::cout << GRE << "Client <" << fd << "> (" << nick << ") registered successfully" << WHI << std::endl;
}

Client* Server::GetClientByFd(int fd) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].GetFd() == fd)
			return &clients[i];
	}
	return NULL;
}

bool Server::IsNicknameTaken(const std::string &nickname) {
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (clients[i].GetNickname() == nickname)
			return true;
	}
	return false;
}

std::string Server::getPassword() const {
	return Password;
}

Channel* Server::GetChannel(const std::string& channelName) {
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i].GetName() == channelName)
			return &channels[i];
	}
	return NULL;
}

Channel* Server::CreateChannel(const std::string& channelName) {
	Channel newChannel(channelName);
	channels.push_back(newChannel);
	return &channels.back();
}

void Server::BroadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd) {
	Channel* channel = GetChannel(channelName);
	if (!channel)
		return;

	std::vector<int> clients = channel->GetClients();
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i] != excludeFd)
			SendMessage(clients[i], message);
	}
}

Client* Server::GetClientByNick(const std::string& nickname) {
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].GetNickname() == nickname)
			return &clients[i];
	}
	return NULL;
}

void Server::ServerLoop() {
	while (!Server::Signal) {
		if (poll(&fds[0], fds.size(), -1) == -1 && !Server::Signal)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < fds.size(); i++) {
			if (fds[i].revents & POLLIN) {
				if (fds[i].fd == SerSocketFd)
					AcceptNewClient();
				else
					ReceiveNewData(fds[i].fd);
			}
		}
	}
}
