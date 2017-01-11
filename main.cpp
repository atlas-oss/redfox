// General
#include <unistd.h>

// Own
#include "inc/redfox.hpp"


int main()
{
	Redfox red;

	while(true)
	{
		red.output = " ||  " + red.wifi() + " ||  " + red.date();
		XStoreName(red.dpy, red.root, red.output.c_str());
		XFlush(red.dpy);
		sleep(1);
	}
}
