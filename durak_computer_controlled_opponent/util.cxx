#include "util.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include <boost/algorithm/find_backward.hpp>
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
  static const unsigned ns = sizeof (spaces) / sizeof (*spaces);
  if (n >= ns) n = ns - 1;
  return spaces + (ns - 1 - n);
}

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded (Ts...) -> overloaded<Ts...>;

AttackDefendAssistCards
calcCardsAtRoundStart (const durak::Game &game)
{
  auto attackCards = game.getAttackingPlayer () ? game.getAttackingPlayer ()->getCards () : std::vector<durak::Card>{};
  auto defendCards = game.getDefendingPlayer () ? game.getDefendingPlayer ()->getCards () : std::vector<durak::Card>{};
  auto assistCards = game.getAssistingPlayer () ? game.getAssistingPlayer ()->getCards () : std::vector<durak::Card>{};
  auto const &gameHistory = game.getHistory ();
  auto startOfCurrentRound = boost::algorithm::find_if_backward (gameHistory.begin (), gameHistory.end (), [] (auto const &history) { return std::same_as<decltype (history), durak::HistoryEvent>; });
  if (startOfCurrentRound != gameHistory.end ())
    {
      for (auto const &gameEvent : std::vector<durak::HistoryEvent>{ startOfCurrentRound, gameHistory.end () })
        {
          // clang-format off
          std::visit(overloaded {
                          [](auto const&) {/*ignore other game events*/},
                          [&](durak::StartAttack const& startAttack)   {attackCards.insert(attackCards.end(), startAttack.cards.begin(), startAttack.cards.end());},
                          [&](durak::AssistAttack const& assistAttack)  {assistAttack.playerRole==durak::PlayerRole::attack?attackCards.insert(attackCards.end(), assistAttack.cards.begin(), assistAttack.cards.end()):assistCards.insert(assistCards.end(), assistAttack.cards.begin(), assistAttack.cards.end());},
                          [&](durak::Defend const& defend)        {defendCards.push_back(defend.card);}
                      }, gameEvent);
          // clang-format on
        }
    }
  return { .attackCards = attackCards, .defendCards = defendCards, .assistCards = assistCards };
}

std::tuple<std::vector<std::tuple<uint8_t, durak::Card> >, std::vector<std::tuple<uint8_t, durak::Card> > >
calcCompressedCardsForAttackAndDefend (durak::Game const &game)
{
  using namespace durak_computer_controlled_opponent;
  auto [attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
  attackCards.insert (attackCards.end (), defendCards.begin (), defendCards.end ());
  auto cardsAsIds = cardsToIds (compress (attackCards));
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