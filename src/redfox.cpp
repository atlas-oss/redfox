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
#if defined(__FreeBSD__)
#include <sys/soundcard.h>
#elif defined(__OpenBSD__)
#include <sys/audioio.h>
#include <sys/ioctl.h>
#elif defined(__linux__)
#include <alsa/asoundlib.h>
#endif

// CPU & MEM usage
#include <sys/types.h>
#if defined(__linux__)
#include <sys/sysinfo.h>
#endif

// System API
#include <sys/sysctl.h>

#if defined(__FreeBSD__)
#include <dev/acpica/acpiio.h>
#elif defined(__OpenBSD__)
#include <sys/sensors.h>
#include <sys/sysctl.h>
#endif

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

#if defined(__FreeBSD__)
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

#elif defined(__OpenBSD__)
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
#elif defined(__linux__)
bool
Redfox::collect_info(std::string &path) const
{
	for (auto i = path_list.begin(); i != path_list.end(); ++i) {
		if (check_dir(*i)) {
			path = *i;
			return true;
		}
	}
	return false;
}

bool
Redfox::check_dir(const std::string dir) const
{
	struct stat directory;

	stat(dir.c_str(), &directory);

	return S_ISDIR(directory.st_mode);
}

bool
Redfox::battery(std::string &state, double &load) const
{
	std::ifstream state_f(battery_path + "status");

	if (state_f.is_open())
		state_f >> state;
	else
		state = "Unkown";

	state_f.close();

	std::ifstream e_full_f;
	std::ifstream e_now_f;
	double f = 0;
	double n = 0;

	for (auto i = now_list.begin(); i != now_list.end(); ++i) {
		e_now_f.open(battery_path + *i);
		if (e_now_f.is_open()) {
			e_now_f >> n;
			e_now_f.close();
			break;
		}
	}
	for (auto i = full_list.begin(); i != full_list.end(); ++i) {
		e_full_f.open(battery_path + *i);
		if (e_full_f.is_open()) {
			e_full_f >> f;
			e_full_f.close();
			break;
		}
	}

	load = (n / f) * 100;

	return true;
}
#endif

#if defined(__FreeBSD__)
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
#elif defined(__OpenBSD__)
bool
Redfox::volume(long &vol) const
{
// 	mixer_ctrl_t mixer = {};

// 	FILE *fd = fopen("/dev/audioctl0", "r");
// 	if (fd)
// 		if (ioctl(fileno(fd), AUDIO_MIXER_READ, &mixer) < 0)
// 			return false;

// 	vol = 0;
// 	std::cout << mixer.un.value.level << std::endl;

// 	fclose(fd);
// 	return true;
	vol = 0;
	return true;
}
#elif defined(__linux__)
bool Redfox::volume(long &vol) const
{
	long minv, maxv;
	snd_mixer_t *handle;
	snd_mixer_elem_t *elem;
	snd_mixer_selem_id_t *sid;

	const std::string mix_name("Master");
	const std::string card("pulse");
	int mix_index = 0;

	snd_mixer_selem_id_alloca(&sid);

	snd_mixer_selem_id_set_index(sid, mix_index);
	snd_mixer_selem_id_set_name(sid, mix_name.c_str());

	if ((snd_mixer_open(&handle, 0)) < 0)
		return false;

	if ((snd_mixer_attach(handle, card.c_str())) >= 0) {
		if ((snd_mixer_selem_register(handle, NULL, NULL)) >= 0)
			if (snd_mixer_load(handle) >= 0) {
				elem = snd_mixer_find_selem(handle, sid);
				if (elem) {
					snd_mixer_selem_get_playback_volume_range(
						elem, &minv, &maxv);

					if (snd_mixer_selem_get_playback_volume(
						    elem,
						    SND_MIXER_SCHN_FRONT_LEFT,
						    &vol)
					    >= 0) {
						/* make the value bound to 100
						 */
						vol -= minv;
						maxv -= minv;
						minv = 0;
						vol = 100 * vol
						      / maxv; // make the value
							      // bound from 0 to
							      // 100
						snd_mixer_close(handle);
						return false;
					}
				}
			}
		snd_mixer_close(handle);
	}

	return true;
}
#endif

#if defined(__FreeBSD__)
bool
Redfox::load_mem(long &mem)
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
#elif defined(__OpenBSD__)
bool
Redfox::load_mem(long &mem)
{
	mem = 0;
	return true;
}
#elif defined(__linux__)
bool
Redfox::load_mem(long &mem) const
{
  std::ifstream meminfo_stream("/proc/meminfo");
  std::stringstream meminfo;
  meminfo << meminfo_stream.rdbuf();
  meminfo_stream.close();
  std::string cached_string(meminfo.str().substr(meminfo.str().find("Cached:") + 15, 10));

  cached_string.erase(std::remove_if(cached_string.begin(), cached_string.end(), isspace), cached_string.end());

  long cached = std::stol(cached_string) * 1000;
 
  mem = (((cpu_info.totalram - cpu_info.freeram - cached) * cpu_info.mem_unit) * 100) / cpu_info.totalram;
  
  return true;
}

#endif

#if defined(FOUND_MU)
int
Redfox::mail() const
{
	int mails = 0;
	Xapian::Database db = Xapian::Database(xapian_db);
	Xapian::QueryParser qparser;
	Xapian::Enquire qenquire(db);

	qparser.add_prefix("flag", "G");

	Xapian::Query query = qparser.parse_query("flag:n AND NOT flag:s");
	qenquire.set_query(query);

	Xapian::MSet hit = qenquire.get_mset(0, 10);

	mails = hit.get_matches_estimated();

	return mails;
}
#endif
