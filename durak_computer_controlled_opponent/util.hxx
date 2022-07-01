#ifndef A082A940_2718_473D_8151_50956A0E5B42
#define A082A940_2718_473D_8151_50956A0E5B42

#include <durak/card.hxx>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
namespace durak_computer_controlled_opponent
{
template <typename T>
std::string
vectorToString (T const &vec)
{
  auto ss = std::stringstream{};
  for (auto i = size_t{}; i < vec.size (); i++)
    {
      ss << static_cast<int> (vec.at (i));
      if (i != vec.size () - 1)
        {
          ss << ',';
        }
    }
  return ss.str ();
}

std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type> attackAndDefendCardsAndTrump (std::string const &gameState);

std::vector<durak::Card> stringToCards (std::string const &cardsAsString);
}
#endif /* A082A940_2718_473D_8151_50956A0E5B42 */
