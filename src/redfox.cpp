// General
#include <unistd.h>
#include <fstream>
#include <memory>

// Date
#include <ctime>
#include <sys/stat.h>

// Volume
#include <alsa/asoundlib.h>

// Own
#include "../inc/redfox.hpp"

bool Redfox::date(std::string& date)
{
  time_t time;
  struct tm* time_inf;

  std::time(&time);
  time_inf = std::localtime(&time);
  date = (asctime(time_inf));

  return true;
}

bool Redfox::wifi(std::string& ip)
{
  getifaddrs(&if_addr);

  for (ifa = if_addr; ifa != nullptr; ifa = ifa->ifa_next)
  {
    if (!ifa->ifa_addr)
      continue;

    if (ifa->ifa_addr->sa_family == AF_INET)
    {
      tmp_ptr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
      char addr_buf[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmp_ptr, addr_buf, INET_ADDRSTRLEN);
      ip = addr_buf;
    }
  }
  if (if_addr)
    freeifaddrs(if_addr);

  return true;
}

bool Redfox::battery(std::string& state, double& load)
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

  for (auto i = now_list.begin(); i != now_list.end(); ++i)
  {
    e_now_f.open(battery_path + *i);
    if (e_now_f.is_open())
    {
      e_now_f >> n;
      e_now_f.close();
      break;
    }
  }
  for (auto i = full_list.begin(); i != full_list.end(); ++i)
  {
    e_full_f.open(battery_path + *i);
    if (e_full_f.is_open())
    {
      e_full_f >> f;
      e_full_f.close();
      break;
    }
  }

  load = (n / f) * 100;

  return true;
}

bool Redfox::collect_info(std::string& path)
{
  for (auto i = path_list.begin(); i != path_list.end(); ++i)
  {
    if (check_dir(*i))
    {
      path = *i;
      return true;
    }
  }
  return false;
}

bool Redfox::check_dir(const std::string dir)
{
  struct stat directory;

  stat(dir.c_str(), &directory);

  return S_ISDIR(directory.st_mode);
}

bool Redfox::volume(long& vol)
{
  long minv, maxv;
  snd_mixer_t* handle;
  snd_mixer_elem_t* elem;
  snd_mixer_selem_id_t* sid;

  const std::string mix_name("Master");
  const std::string card("pulse");
  int mix_index = 0;

  snd_mixer_selem_id_alloca(&sid);

  snd_mixer_selem_id_set_index(sid, mix_index);
  snd_mixer_selem_id_set_name(sid, mix_name.c_str());

  if ((snd_mixer_open(&handle, 0)) < 0)
    return false;

  if ((snd_mixer_attach(handle, card.c_str())) >= 0)
  {
    if ((snd_mixer_selem_register(handle, NULL, NULL)) >= 0)
      if (snd_mixer_load(handle) >= 0)
      {
        elem = snd_mixer_find_selem(handle, sid);
        if (elem)
        {
          snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

          if (snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol) >= 0)
          {
            /* make the value bound to 100 */
            vol -= minv;
            maxv -= minv;
            minv = 0;
            vol = 100 * vol / maxv;  // make the value bound from 0 to 100
	    snd_mixer_close(handle);
	    return false;
          }
        }
      }
    snd_mixer_close(handle);
  }

  return true;
}
