// General
#include <X11/Xlib.h>
#include <string>
#include <iostream>
#include <stdexcept>

// Wifi
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Redfox {
	int screen;

	ifaddrs *if_addr = nullptr;
	ifaddrs *ifa = nullptr;
	in_addr *tmp_ptr = nullptr;

public:
	Display *dpy;
	Window root;
	std::string output;
	
	Redfox()
	{
		if(!(dpy = XOpenDisplay(nullptr)))
		{
			std::cerr << "Could not open Display." << std::endl;
			throw std::invalid_argument("Could not open Display.");
		}
		screen = DefaultScreen(dpy);
		root = RootWindow(dpy, screen);
	}
	
	std::string date();
	std::string wifi();
};
