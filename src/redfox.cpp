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
#include <sys/audioio.h>
#include <sys/ioctl.h>

// CPU & MEM usage
#include <sys/types.h>

// System API
#include <sys/types.h>
#include <sys/sensors.h>
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
Redfox::detect_battery()
{
	struct sensordev sendev_ = {};
	size_t len = sizeof(sendev_);

	mib_[0] = CTL_HW;
	mib_[1] = HW_SENSORS;

	for (int i = 0; i < 10; ++i) {
		mib_[2] = i;

		if (sysctl(mib_, 3, &sendev_, &len, NULL, 0) == -1)
			continue;

		if (std::string(sendev_.xname).find("bat", 0) !=
		    std::string::npos)
			return true;
	}

	return false;
}

bool
Redfox::battery(std::string &state, int &load)
{
	struct sensor sen_ = {};
	int state_ = 0;
	long current_ = 0, full_ = 0;
	size_t len = 0;

	mib_[3] = SENSOR_WATTHOUR;
	mib_[4] = 0;

	// Full Capacity
	len = sizeof(sen_);
	if (sysctl(mib_, 5, &sen_, &len, NULL, 0) == -1)
		return false;

	full_ = sen_.value;
	sen_ = {};

	// Current Capacity
	mib_[4] = 3;
	if (sysctl(mib_, 5, &sen_, &len, NULL, 0) == -1)
		return false;

	current_ = sen_.value;
	sen_ = {};

	// State
	mib_[3] = SENSOR_INTEGER;
	mib_[4] = 0;
	if (sysctl(mib_, 5, &sen_, &len, NULL, 0) == -1)
		return false;

	state_ = sen_.value;
	sen_ = {};

	load = (int)(((double)current_ / (double)full_) * 100);

	switch (state_) {
	case 1:
		switch (i_bat) {
		case 0:
			state = "Discharging.  ";
			i_bat++;
			break;
		case 1:
			state = "Discharging.. ";
			i_bat++;
			break;
		case 2:
			state = "Discharging...";
			i_bat = 0;
			break;
		}
		break;
	case 2:
		switch (i_bat) {
		case 0:
			state = "Charging.  ";
			i_bat++;
			break;
		case 1:
			state = "Charging.. ";
			i_bat++;
			break;
		case 2:
			state = "Charging...";
			i_bat = 0;
			break;
		}
		break;
	case 0:
		state = "Full";
		break;
	default:
		state = "Unkown?";
	}

	return false;
}

// bool
// Redfox::volume(long &vol) const
// {
// 	mixer_ctrl_t mixer = {};

// 	FILE *fd = fopen("/dev/audioctl0", "r");
// 	if (fd)
// 		if (ioctl(fileno(fd), AUDIO_MIXER_READ, &mixer) < 0)
// 			return false;

// 	vol = 0;
// 	std::cout << mixer.un.value.level << std::endl;

// 	fclose(fd);
// 	return true;
// }
