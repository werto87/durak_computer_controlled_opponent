#ifndef A082A940_2718_473D_8151_50956A0E5B42
#define A082A940_2718_473D_8151_50956A0E5B42

#include <durak/card.hxx>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>


namespace durak_computer_controlled_opponent
{

durak::Card idToCard (uint8_t id);

uint8_t cardToId (durak::Card const &card);

std::vector<uint8_t> cardsToIds (const std::vector<durak::Card> &cards);

std::vector<durak::Card> idsToCards (const std::vector<uint8_t> &ids);

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

}
#endif /* A082A940_2718_473D_8151_50956A0E5B42 */
