// General
#include <X11/Xlib.h>
#include <iostream>
#include <stdexcept>
#include <vector>

// Wifi
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Redfox
{
	int screen;

	std::vector<std::string> full_list = {
		"energy_full", "charge_full", "current_full"};
	std::vector<std::string> now_list = {"energy_now", "charge_now",
						   "current_now"};

	ifaddrs *if_addr = nullptr;
	ifaddrs *ifa = nullptr;
	in_addr *tmp_ptr = nullptr;

	bool check_dir(const std::string dir) const;

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
	}

  bool date(std::string &date) const;
	bool wifi(std::string &ip);
	bool battery(std::string &state, int &load) const;
	bool load_cpu_mem(double &cpu, long &mem) const;
	bool volume(long &vol) const;
};
