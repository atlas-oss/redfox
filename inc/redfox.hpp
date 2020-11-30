// General
#include <X11/Xlib.h>

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

// Volume
#include <sys/soundcard.h>

// Wifi
#include <netinet/in.h>

#include <arpa/inet.h>
#include <ifaddrs.h>

class Redfox {
	int screen;
	int i_bat = 0;
	int mixer_index;
	int mixer = 0;
	int mixer_devmask = 0;
	std::array<int, 20> cpu_load_old = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0 };

	ifaddrs *if_addr = nullptr;
	ifaddrs *ifa = nullptr;

	in_addr *tmp_ptr = nullptr;

	const char *vol_dev_names[SOUND_MIXER_NRDEVICES] = SOUND_DEVICE_NAMES;

	int detect_mixer();

    public:
	Display *dpy;
	Window root;
	std::string output;

	Redfox()
	{
		if (!(dpy = XOpenDisplay(nullptr))) {
			std::cerr << "Could not open Display." << std::endl;
			throw std::invalid_argument("Could not open Display.");
		}
		screen = DefaultScreen(dpy);
		root = RootWindow(dpy, screen);

		mixer_index = detect_mixer();
	}

	bool date(std::string &date) const;
	bool wifi(std::string &ip);
	bool battery(std::string &state, int &load);
	bool load_cpu_mem(double &cpu, long &mem);
	bool volume(long &vol) const;
};
