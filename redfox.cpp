// General 
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <string>

// Date
#include <ctime>

// Wifi
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static Display *dpy;
static Window root;
static int screen;
static std::string str;

static struct ifaddrs *if_addr = nullptr;
static struct ifaddrs *ifa = nullptr;

std::string date();
std::string wifi();

int main(void)
{
	dpy = XOpenDisplay(nullptr);
	
	if(!dpy)
	{
		std::cerr << "Could not open Display." << std::endl;
		return -1;
	}

	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);

	while(true)
	{
		str = " ||  " + wifi() + " ||  " + date();
		XStoreName(dpy, root, str.c_str());
		XFlush(dpy);
		sleep(1);
	}
}

std::string wifi()
{
	std::string ip("Wifi offline...");;
	struct in_addr *tmp_ptr;
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

std::string date()
{
	time_t time;
	struct tm *time_inf;

	std::time(&time);
	time_inf = std::localtime(&time);
	std::string tmp(asctime(time_inf));

	return tmp.substr(0, tmp.size()-1);
}
