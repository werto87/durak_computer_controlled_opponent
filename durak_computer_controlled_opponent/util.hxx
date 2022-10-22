#ifndef A082A940_2718_473D_8151_50956A0E5B42
#define A082A940_2718_473D_8151_50956A0E5B42

#include "solve.hxx"
#include <durak/card.hxx>
#include <magic_enum.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <durak/print.hxx>

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

std::string indent_padding(unsigned n);

template <typename Iterator, typename Stream>
void serialize_indented(const Iterator& F, const Iterator& L, Stream& s, unsigned indent=2) {
  for (Iterator j(F);  j != L;  ++j) {
      s << indent_padding(j->ply() * indent);
      s <<"type: " <<  magic_enum::enum_name(j->key()()) << " ";
      if(j->key()()==Action::Category::PlayCard){
          durak::Card  card=j->key().playedCard().value();
          s <<"card: " <<  card << " ";
        }
      auto [result,someBool]=j->data();
      s <<"result: " <<  magic_enum::enum_name(result) << " ";
      s <<"attack turn: " <<  someBool << " ";
      s << "\n";
    }
}

}
#endif /* A082A940_2718_473D_8151_50956A0E5B42 */
