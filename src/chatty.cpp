/**
 * @author  Alexander Beloglazov <abelogla@buffalo.edu>
 * @version 0.1
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 */


#include "../include/global.h"

#include "server/server.cpp"
#include "client/client.cpp"


using namespace std;

runtime_params *params;
map<string, Machine*> *ip2machine;

/* 
 * Method prints usage message if command line arguments are not valid
 * @input exec name of the executable
 */
void print_usage(const char *exec)
{
	std::cout << "\nUSAGE: " << exec << " {c|s} <port>\n\n"
			  << "Options:\n"
			  << "\tc\tExecute in the client mode;\n"
			  << "\ts\tExecute in the server mode;\n"
			  << "\t<port>\tPort number to listen for connections on;\n\n";
	exit(1);
};

/* 
 * Method obtains external IP address of the machine along with its hostname
 * and saves them in the global structure params.
 */
void get_public_address()
{
	int socket_fd;
	struct sockaddr_in serv_addr, self_addr;
	socklen_t addr_len = sizeof(self_addr);
	
	if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("ERROR creating UDP socket");
		exit(1);
	}
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(LOOKUP_PORT);
	inet_pton(AF_INET, LOOKUP_IP, &(serv_addr.sin_addr));
	
	if (connect(socket_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror((std::string("ERROR connecting to ") + std::string(LOOKUP_IP)).c_str());
		exit(1);
	}
	
	getsockname(socket_fd, (sockaddr *)&self_addr, &addr_len);
	
	params->ip_address = extract_ip(self_addr);
	
	struct hostent *host_info = gethostbyaddr(
		&(self_addr.sin_addr), sizeof(struct in_addr), AF_INET);
	
	if (host_info == NULL) {
		perror("Could not get hostname");
		exit(1);
	}
	
	params->hostname = std::string(host_info->h_name);
	close(socket_fd);
}

int main(int argc, char **argv)
{

	if (argc != 3)
	{
		print_usage(argv[0]);
	}

	// instantiate new IP address to Machine map
	ip2machine = new map<string, Machine *>();

	// make a new instance of runtime_params structure
	params = new runtime_params();
	params->is_server = string(argv[1]) == "s";
	params->is_logged = 0;
	params->port = argv[2];

	// get hostname and public IP and save them in params
	get_public_address();

	(params->is_server) ? server::run() : client::run();

	return 0;
}
