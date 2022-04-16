#ifndef A082A940_2718_473D_8151_50956A0E5B42
#define A082A940_2718_473D_8151_50956A0E5B42

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename T>
std::string
vectorToString (T const &vec)
{
  auto ss = std::stringstream{};
  for (auto i = size_t{}; i < vec.size (); i++)
    {
      ss << vec.at (i);
      if (i != vec.size () - 1)
        {
          ss << ',';
        }
    }
  return ss.str ();
}

std::string gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &gameState);

#endif /* A082A940_2718_473D_8151_50956A0E5B42 */
