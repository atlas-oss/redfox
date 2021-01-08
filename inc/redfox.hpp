// General
#include <X11/Xlib.h>

#include <iostream>
#include <stdexcept>
#include <vector>

// Volume
#if defined(__FreeBSD__)
#include <sys/soundcard.h>
#endif

// Wifi
#include <netinet/in.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

class Redfox {
	// Shared Code
	int screen;
	ifaddrs *if_addr = nullptr;
	ifaddrs *ifa = nullptr;
	in_addr *tmp_ptr = nullptr;

#if defined(__FreeBSD__)
	int i_bat = 0;
	int mixer_index;
	int mixer = 0;
	int mixer_devmask = 0;
	const char *vol_dev_names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

	int detect_mixer();

#elif defined(__OpenBSD__)

#elif defined(__linux)
	std::vector<std::string> path_list = { "/sys/class/power_supply/BAT0/",
		"/sys/class/power_supply/BAT1/",
		"/sys/class/power_supply/BAT2/",
		"/sys/class/power_supply/BAT3/" };

	std::vector<std::string> full_list = { "energy_full", "charge_full",
		"current_full" };
	std::vector<std::string> now_list = { "energy_now", "charge_now",
		"current_now" };

	std::string battery_path;

	bool check_dir(const std::string dir) const;
	bool collect_info(std::string &path) const;
#endif

#if defined(FOUND_MU)
	const std::string xapian_db;
#endif

    public:
	Display *dpy;
	Window root;
	std::string output;

#if defined(__OpenBSD__)
	int i_bat = 0;
	int mib_[5] = {};
#endif

#if defined(FOUND_MU)
	Redfox()
	    : xapian_db(std::string(getenv("HOME")) + "/.cache/mu/xapian/")
#else
	Redfox()
#endif
	{
		if (!(dpy = XOpenDisplay(nullptr))) {
			std::cerr << "Could not open Display." << std::endl;
			throw std::invalid_argument("Could not open Display.");
		}
		screen = DefaultScreen(dpy);
		root = RootWindow(dpy, screen);

#if defined(__FreeBSD__)
		mixer_index = detect_mixer();
#elif defined(__OpenBSD__)
		detect_battery();
#elif defined(__linux__)
		collect_info(battery_path);
#endif
	}

#if defined(__OpenBSD__)
	bool detect_battery();
#endif
	bool date(std::string &date) const;
	bool wifi(std::string &ip);
	bool battery(std::string &state, int &load);
	bool load_mem(long &mem) const;
	bool volume(long &vol) const;
	int mail() const;
};
