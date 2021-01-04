// General
#include <unistd.h>

// Own
#include "inc/redfox.hpp"

bool bdate = true, bwifi = true, bbattery = true, bcpu_mem = true, bmail = true,
	bvolume = true, bedition = true;

#define HELP \
	" redfox v0.02\n USAGE: redfox [OPTION]\n -m - No memory and CPU usage\n -w - No IP address\n -b - No battery load\n -v - No volume indicator\n -d - No date\n -h - this message."

bool
parse_args(const int argc, const char **argv)
{
	for (int i = 0; i < argc; ++i) {
		switch (argv[i][1]) {
		case 'e': {
			bedition = false;
			break;
		}
		case 'm': {
			bmail = false;
			break;
		}
		case 'c': {
			bcpu_mem = false;
			break;
		}
		case 'w': {
			bwifi = false;
			break;
		}
		case 'b': {
			bbattery = false;
			break;
		}
		case 'v': {
			bvolume = false;
			break;
		}
		case 'd': {
			bdate = false;
			break;
		}
		case 'h': {
			std::cout << HELP << std::endl;
			exit(0);
		}
		}
	}

	return true;
}

int
main(const int argc, const char **argv)
{
	parse_args(argc, argv);
	Redfox red;
	int load;
	long vol, mem;
	std::string date, ip("Wifi offline..."), state;

	while (true) {
		if(bedition) {
			red.output.append("");
		}

		if(bmail) {
			red.output.append("      " + std::to_string(red.mail()));
		}
		
		if (bcpu_mem) {
			red.load_cpu_mem(mem);
			red.output.append(
			    "      " + std::to_string(mem) + "%");
		}

		if (bwifi) {
			red.wifi(ip);
			red.output.append("      " + ip);
		}

		if (bvolume) {
			red.volume(vol);
			red.output.append("      " + std::to_string(vol) + "%");
		}

		if (bbattery) {
			red.battery(state, load);
			red.output.append("      " + state + " " +
			    std::to_string(load).substr(0, 5) + "%");
		}

		if (bdate) {
			red.date(date);
			red.output.append(
			    "      " + date.substr(0, (date.length()) - 1));
		}

		XStoreName(red.dpy, red.root, red.output.c_str());
		XFlush(red.dpy);
		red.output.clear();
		sleep(1);
	}
}
