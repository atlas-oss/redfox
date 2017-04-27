// General
#include <unistd.h>

// Own
#include "inc/redfox.hpp"


int main()
{
	Redfox red;
	double load;
	std::string date, ip("Wifi offline..."), state;
	

	while(true)
	{
		red.date(date);
		red.wifi(ip);
		red.battery(state, load);
		
	   	red.output =
			">  "
			+ ip
			+ "  >  "
			+ state
			+ "  "
			+ std::to_string(load).substr(0,5)
			+ "%  >  "
			+ date.substr(0, (date.length())-1);
		XStoreName(red.dpy, red.root, red.output.c_str());
		XFlush(red.dpy);
		sleep(1);
	}
}
