// General 
#include <iostream>
#include <unistd.h>

// Date
#include <ctime>

// Own
#include "../inc/redfox.hpp"


std::string Redfox::date()
{
	time_t time;
	struct tm *time_inf;

	std::time(&time);
	time_inf = std::localtime(&time);
	std::string tmp(asctime(time_inf));

	return tmp.substr(0, tmp.size()-1);
}

std::string Redfox::wifi()
{	
	std::string ip("Wifi offline...");;

	getifaddrs(&if_addr);

	for(ifa = if_addr; ifa != nullptr; ifa = ifa->ifa_next)
	{
		if(!ifa->ifa_addr) continue;

		if(ifa->ifa_addr->sa_family == AF_INET) {
			tmp_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addr_buf[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmp_ptr, addr_buf, INET_ADDRSTRLEN);
			ip = addr_buf;
		}
	}
	if(if_addr) freeifaddrs(if_addr);
	
	return ip;
}
