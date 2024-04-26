#include "util.hxx"
#include "compressCard.hxx"
#include <boost/algorithm/find_backward.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <charconv>
#include <magic_enum/magic_enum.hpp>
namespace durak_computer_controlled_opponent
{
std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>
attackAndDefendCardsAndTrump (std::string const &gameState)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>{};
  std::vector<std::string> splitMessage{};
  boost::algorithm::split (splitMessage, gameState, boost::is_any_of (";")); // NOLINT //NOLINT is used to supress a clang tidy false positive warning
  if (splitMessage.size () == 3)
    {
      auto &[attack, _defend, trump] = result;
      attack = stringToCards (splitMessage.at (0));
      _defend = stringToCards (splitMessage.at (1));
      trump = magic_enum::enum_cast<durak::Type> (std::stoi (splitMessage.at (2))).value ();
      return result;
    }
  else
    {
      throw std::logic_error{ "gameState wrong format should only have 2 times ; gameStateAsString: " + gameState };
    }
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
  auto startOfCurrentRound = boost::algorithm::find_if_backward (gameHistory.begin (), gameHistory.end (), [] (auto const &history) { return std::holds_alternative<durak::RoundInformation> (history); });
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

AttackDefendAssistCardsAndIds
calcIdAndCompressedCardsForAttackAndDefend (durak::Game const &game)
{
  auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
  auto cards = attackCards;
  cards.insert (cards.end (), defendCards.begin (), defendCards.end ());
  auto cardsAsIds = cardsToIds (compress (cards));
  auto attackingCardsAsIds = std::vector<uint8_t>{ cardsAsIds.begin (), cardsAsIds.begin () + boost::numeric_cast<int64_t> (attackCards.size ()) };
  auto attackingCardsAsIdsAndAsCards = std::vector<std::tuple<uint8_t, durak::Card> >{};
  std::ranges::transform (attackingCardsAsIds,std::back_inserter (attackingCardsAsIdsAndAsCards),[i=size_t{}, &attackCards] (auto const &x)mutable {
    auto result=std::tuple<uint8_t, durak::Card>{ x, attackCards.at (i) };
    i++;
    return result;
  });
  std::ranges::sort (attackingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  auto defendingCardsAsIds = std::vector<uint8_t>{ cardsAsIds.begin () + boost::numeric_cast<int64_t> (attackCards.size ()), cardsAsIds.end () };
  auto defendingCardsAsIdsAndAsCards = std::vector<std::tuple<uint8_t, durak::Card> >{};
  std::ranges::transform (defendingCardsAsIds,std::back_inserter (defendingCardsAsIdsAndAsCards),[i=size_t{}, &defendCards] (auto const &x)mutable {
    auto result=std::tuple<uint8_t, durak::Card>{ x, defendCards.at (i) };
    i++;
    return result;
  });
  std::ranges::sort (defendingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  return { attackingCardsAsIdsAndAsCards, defendingCardsAsIdsAndAsCards };
}
AttackDefendAssistCardsToCompressedCards
calcCardsAndCompressedCardsForAttackAndDefend (const durak::Game &game)
{
  auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
  auto cards = attackCards;
  cards.insert (cards.end (), defendCards.begin (), defendCards.end ());
  auto compressedCards = compress (cards);
  auto attackingCardsAsIds = std::vector<durak::Card>{ compressedCards.begin (), compressedCards.begin () + boost::numeric_cast<int64_t> (attackCards.size ()) };
  auto attackingCardsAsIdsAndAsCards = std::vector<std::tuple<durak::Card, durak::Card> >{};
  std::ranges::transform (attackingCardsAsIds,std::back_inserter (attackingCardsAsIdsAndAsCards),[i=size_t{}, &attackCards] (auto const &x)mutable {
    auto result=std::tuple<durak::Card, durak::Card>{ x, attackCards.at (i) };
    i++;
    return result;
  });
  std::ranges::sort (attackingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  auto defendingCardsAsIds = std::vector<durak::Card>{ compressedCards.begin () + boost::numeric_cast<int64_t> (attackCards.size ()), compressedCards.end () };
  auto defendingCardsAsIdsAndAsCards = std::vector<std::tuple<durak::Card, durak::Card> >{};
  std::ranges::transform (defendingCardsAsIds,std::back_inserter (defendingCardsAsIdsAndAsCards),[i=size_t{}, &defendCards] (auto const &x)mutable {
    auto result=std::tuple<durak::Card, durak::Card>{ x, defendCards.at (i) };
    i++;
    return result;
  });
  std::ranges::sort (defendingCardsAsIdsAndAsCards, [] (auto const &x, auto const &y) { return std::get<0> (x) < std::get<0> (y); });
  return { attackingCardsAsIdsAndAsCards, defendingCardsAsIdsAndAsCards };
}

void
transformFromLookUp (std::vector<durak::Card> &cardsToCompress, std::vector<std::tuple<durak::Card, durak::Card> > const &lookUp)
{
  for (auto &card : cardsToCompress)
    {
      if (auto cardAndCompressedCardItr = std::ranges::find_if (lookUp, [&card] (auto const &cardAndCompressedCard) { return card == std::get<1> (cardAndCompressedCard); }); cardAndCompressedCardItr != lookUp.end ())
        {
          card = std::get<0> (*cardAndCompressedCardItr);
        }
      else
        {
          throw std::logic_error{ "card not in lookup " };
        }
    }
}

void
transformFromLookUp (durak::Card &cardToCompress, std::vector<std::tuple<durak::Card, durak::Card> > const &lookUp)
{
  auto cardsToCompress = std::vector<durak::Card>{ cardToCompress };
  transformFromLookUp (cardsToCompress, lookUp);
  cardToCompress = cardsToCompress.front ();
}

std::vector<Action>
historyEventsToActionsCompressedCards (std::vector<durak::HistoryEvent> const &histories, AttackDefendAssistCardsToCompressedCards const &attackDefendAssistCardsToCompressedCards)
{
  auto [attackCards, defendCards, assistCards] = attackDefendAssistCardsToCompressedCards;
  auto lookUp = std::vector<std::tuple<durak::Card, durak::Card> >{};
  lookUp.reserve (attackCards.size () + defendCards.size () + assistCards.size ());
  lookUp.insert (lookUp.end (), attackCards.begin (), attackCards.end ());
  lookUp.insert (lookUp.end (), defendCards.begin (), defendCards.end ());
  lookUp.insert (lookUp.end (), assistCards.begin (), assistCards.end ());
  auto lastRoundHistoryBeginItr = boost::algorithm::find_if_backward (histories, [] (durak::HistoryEvent const &historyEvent) { return std::holds_alternative<durak::RoundInformation> (historyEvent); });
  if (lastRoundHistoryBeginItr != histories.end ())
    {
      auto lastRoundHistory = std::vector<durak::HistoryEvent> (lastRoundHistoryBeginItr, histories.end ());
      std::transform (lastRoundHistory.begin (), lastRoundHistory.end (), lastRoundHistory.begin (), [&lookUp] (durak::HistoryEvent gameEvent) {
        // clang-format off
      std::visit(overloaded {
                    [](auto const&) {/*ignore other game events*/},
                    [&lookUp](durak::StartAttack & startAttack)   {transformFromLookUp(startAttack.cards,lookUp);},
                    [&lookUp](durak::AssistAttack  &assistAttack)  {transformFromLookUp(assistAttack.cards,lookUp);},
                    [&lookUp](durak::Defend & defend)        {transformFromLookUp(defend.card,lookUp);}
                }, gameEvent);
        // clang-format on
        return gameEvent;
      });
      auto actions = historyEventsToActions (lastRoundHistory);
      return actions;
    }
  else
    {
      throw std::logic_error{ "could not find round information in history" };
    }
}

}