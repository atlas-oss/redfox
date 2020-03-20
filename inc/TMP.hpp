/**
 * Some template meta programming for resolving the correct path of the battery etc.
 */

#include <array>

template<size_t N>
constexpr std::array<std::string, N> inverse(const std::array<std::string, N> &a) {
	std::array<std::string, N> path_list{};
	for(size_t i = 0; i < N; ++i)
		path_list[a[i]] = std::string("/sys/class/power_supply/BAT") + std::to_string(i) + "/";

	return path_list;
}

template <bool Condiation, typename TrueResult, typename FalseResult>
struct if_;

template <typename TrueResult, typename FalseResult>
struct if_<true, TrueResult, FalseResult>
{
  typedef TrueResult result;
};

template <typename TrueResult, typename FalseResult>
struct if_<false, TrueResult, FalseResult>
{
  typedef FalseResult result;
};
