#include "util.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <charconv>
#include <magic_enum.hpp>
namespace durak_computer_controlled_opponent
{
std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>
attackAndDefendCardsAndTrump (std::string const &gameState)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>{};
  auto &[attack, defend, trump] = result;
  std::vector<std::string> splitMessage{};
  boost::algorithm::split (splitMessage, gameState, boost::is_any_of (";"));
  if (splitMessage.size () == 3)
    {
      attack = stringToCards (splitMessage.at (0));
      defend = stringToCards (splitMessage.at (1));
      trump = magic_enum::enum_cast<durak::Type> (std::stoi (splitMessage.at (2))).value ();
    }
  else
    {
      throw std::logic_error{ "gameState wrong format should only have 2 times ; gameStateAsString: " + gameState };
    }
  return result;
}
std::vector<durak::Card>
stringToCards (std::string const &cardsAsString)
{
  auto result = std::vector<durak::Card>{};
  std::vector<std::string> splitMessage{};
  boost::algorithm::split (splitMessage, cardsAsString, boost::is_any_of (","));
  for (auto const &value : splitMessage)
    {
      uint8_t tmp{};
      auto [ptr, ec]{ std::from_chars (value.data (), value.data () + value.size (), tmp) };
      if (ec == std::errc ())
        {
          result.push_back (idToCard (tmp));
        }
      else if (ec == std::errc::invalid_argument)
        {
          throw std::logic_error{ "That isn't a number" };
        }
      else if (ec == std::errc::result_out_of_range)
        {
          throw std::logic_error{ "This number is larger than an uint8_t" };
        }
    }
  return result;
}
std::string
indent_padding (unsigned int n)
{
  static char const spaces[] = "                                                                   ";
  static const unsigned ns = sizeof(spaces)/sizeof(*spaces);
  if (n >= ns) n = ns-1;
  return spaces + (ns-1-n);
}

std::tuple<std::vector<durak::Card>, std::vector<durak::Card> >
calcCardsAtRoundStart (const durak::Game &game)
{
  //  TODO use table and game history to restore cards at round start

  throw std::logic_error{"IMPLEMENT THIS"};
  return {};
}

std::tuple<std::vector<std::tuple<uint8_t, durak::Card> >, std::vector<std::tuple<uint8_t, durak::Card> > >
calcCompressedCardsForAttackAndDefend (durak::Game const &game)
{
  throw std::logic_error{"IMPLEMENT THIS"};
  using namespace durak_computer_controlled_opponent;
  auto [cards, defendingCards] = calcCardsAtRoundStart (game);
  cards.insert (cards.end (), defendingCards.begin (), defendingCards.end ());
  auto cardsAsIds = cardsToIds (compress (cards));
  auto attackingCardsAsIds = std::vector<uint8_t>{ cardsAsIds.begin (), cardsAsIds.begin () + cardsAsIds.size () / 2 };
  auto attackingCardsAsIdsAndAsCards = std::vector<std::tuple<uint8_t, durak::Card> >{};
  pipes::mux (attackingCardsAsIds, game.getAttackingPlayer ()->getCards ()) >>= pipes::transform ([] (auto const &x, auto const &y) { return std::tuple<uint8_t, durak::Card>{ x, y }; }) >>= pipes::push_back (attackingCardsAsIdsAndAsCards);
  ranges::sort (attackingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  auto defendingCardsAsIds = std::vector<uint8_t>{ cardsAsIds.begin () + cardsAsIds.size () / 2, cardsAsIds.end () };
  auto defendingCardsAsIdsAndAsCards = std::vector<std::tuple<uint8_t, durak::Card> >{};
  pipes::mux (defendingCardsAsIds, game.getDefendingPlayer ()->getCards ()) >>= pipes::transform ([] (auto const &x, auto const &y) { return std::tuple<uint8_t, durak::Card>{ x, y }; }) >>= pipes::push_back (defendingCardsAsIdsAndAsCards);
  ranges::sort (defendingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  return { attackingCardsAsIdsAndAsCards, defendingCardsAsIdsAndAsCards };
}

}