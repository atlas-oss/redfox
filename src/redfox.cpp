// General
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

// Network
#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

// Date
#include <sys/stat.h>

#include <ctime>

// Volume
//#include <alsa/asoundlib.h>

// CPU & MEM usage
#include <sys/types.h>
//#include <sys/sysinfo.h>

// System API
#include <sys/sysctl.h>

// Own
#include "../inc/redfox.hpp"

bool
Redfox::wifi(std::string &ip)
{
	getifaddrs(&if_addr);

	for (ifa = if_addr; ifa != nullptr; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr)
			continue;

		if (ifa->ifa_addr->sa_family == AF_INET) {
			tmp_ptr =
			    &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addr_buf[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmp_ptr, addr_buf, INET_ADDRSTRLEN);
			if (std::string(addr_buf).compare("127.0.0.1") == 0)
				continue;

			ip = addr_buf;
		}
	}
	if (ip.empty())
		ip = "Offline...";

	if (if_addr)
		freeifaddrs(if_addr);

	return true;
}

bool
Redfox::date(std::string &date) const
{
	time_t time;
	struct tm *time_inf;

	std::time(&time);
	time_inf = std::localtime(&time);
	date = (asctime(time_inf));

	return true;
}

bool
Redfox::battery(std::string &state, int &load) const
{
	int life_, state_;
	size_t len = 0;
	// Load
	len = sizeof(life_);
	sysctlbyname("hw.acpi.battery.life", &life_, &len, NULL, 0);
	load = life_;

	len = 0;

	// State
	len = sizeof(state_);
	sysctlbyname("hw.acpi.battery.state", &state_, &len, NULL, 0);
	switch (state_) {
	case 1:
		state = "Discharging...";
		break;
	}

	return true;
}

bool
Redfox::check_dir(const std::string dir) const
{
	struct stat directory;

	stat(dir.c_str(), &directory);

	return S_ISDIR(directory.st_mode);
}
