// General
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <array>
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

// Mail
#include <xapian.h>

// Date
#include <sys/stat.h>

#include <ctime>

// Volume
#include <sys/soundcard.h>

// CPU & MEM usage
#include <sys/types.h>
//#include <sys/sysinfo.h>

// System API
#include <sys/sysctl.h>

#include <dev/acpica/acpiio.h>

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
Redfox::battery(std::string &state, int &load)
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
	case ACPI_BATT_STAT_CHARGING:
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
	case ACPI_BATT_STAT_DISCHARG:
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
	case 0:
		state = "Full";
		break;
	default:
		state = "Unkown?";
	}

	return true;
}

bool
Redfox::volume(long &vol) const
{
	int res = -1;

	if (ioctl(mixer, MIXER_READ(mixer_index), &res) == -1)
		throw std::runtime_error("Could not read volume.");

	vol = res & 0x7f;

	return true;
}

int
Redfox::detect_mixer()
{
	if ((mixer = open("/dev/mixer", O_RDONLY)) < 0)
		throw std::runtime_error("Could not open mixer device.");

	if (ioctl(mixer, SOUND_MIXER_READ_DEVMASK, &mixer_devmask) == -1)
		throw std::runtime_error("Could not read mixer devmask.");

	for (int i = 0; i < SOUND_MIXER_NRDEVICES; ++i) {
		if (strcmp(vol_dev_names[i], "vol") == 0)
			return i;
	}

	return -1;
}

bool
Redfox::load_cpu_mem(long &mem)
{
	// These variables only belong to the memory calculation
	long page_size_ = 0;
	long mem_all_ = 0;
	long mem_inactive_ = 0;
	long mem_cache_ = 0;
	long mem_free_ = 0;
	long total_mem_ = 0;

	size_t len_ = sizeof(mem_all_);
	sysctlbyname("hw.physmem", &mem_all_, &len_, NULL, 0);
	sysctlbyname("hw.pagesize", &page_size_, &len_, NULL, 0);
	sysctlbyname(
	    "vm.stats.vm.v_inactive_count", &mem_inactive_, &len_, NULL, 0);
	sysctlbyname("vm.stats.vm.v_cache_count", &mem_cache_, &len_, NULL, 0);
	sysctlbyname("vm.stats.vm.v_free_count", &mem_free_, &len_, NULL, 0);
	
	mem_inactive_ = mem_inactive_ * page_size_;
	mem_cache_ = mem_cache_ * page_size_;
	mem_free_ = mem_free_ * page_size_;

	// We consinder inactive, cached and free memory as available memory.
	total_mem_ = (mem_all_ - (mem_inactive_ + mem_cache_ + mem_free_));

	mem = ((double)total_mem_ / (double)mem_all_) * 100;
	return true;
}

int
Redfox::mail() const
{
	int mails = 0;
	Xapian::Database db = Xapian::Database(xapian_db);
	Xapian::QueryParser qparser;
	Xapian::Enquire qenquire(db);

	qparser.add_prefix("flag", "B");

	// Maybe flag:new AND NOT flag:seen?
	Xapian::Query query = qparser.parse_query("flag:unread");
	qenquire.set_query(query);

	Xapian::MSet hit = qenquire.get_mset(0, 10);

	mails = hit.get_matches_estimated();

	return mails;
}
