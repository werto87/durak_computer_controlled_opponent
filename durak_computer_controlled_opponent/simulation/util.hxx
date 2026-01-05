#pragma once

#include "solve.hxx"
#include <durak/card.hxx>
#include <magic_enum/magic_enum.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace durak_computer_controlled_opponent
{

std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type> attackAndDefendCardsAndTrump (std::string const &gameState);

std::vector<durak::Card> stringToCards (std::string const &cardsAsString);

std::string indent_padding (unsigned n);

template <typename Iterator, typename Stream>
void
serialize_indented (const Iterator &F, const Iterator &L, Stream &s, unsigned indent = 2)
{
  for (Iterator j (F); j != L; ++j)
    {
      s << indent_padding (j->ply () * indent);
      s << "key value: " << int{ j->key ().value () } << " ";
      s << "type: " << magic_enum::enum_name (j->key () ()) << " ";
      if (j->key () () == Action::Category::PlayCard)
        {
          durak::Card card = j->key ().playedCard ().value ();
          s << "card: " << card << " ";
        }
      auto [result, someBool] = j->data ();
      s << "result: " << magic_enum::enum_name (result) << " ";
      s << "attack turn: " << someBool << " ";
      s << "\n";
    }
}
struct AttackDefendAssistCards
{
  std::vector<durak::Card> attackCards{};
  std::vector<durak::Card> defendCards{};
  std::vector<durak::Card> assistCards{};
};
AttackDefendAssistCards calcCardsAtRoundStart (durak::Game const &game);

struct AttackDefendAssistCardsAndIds
{
  std::vector<std::tuple<uint8_t, durak::Card> > attackCardsAndIds{};
  std::vector<std::tuple<uint8_t, durak::Card> > defendCardsAndIds{};
  std::vector<std::tuple<uint8_t, durak::Card> > assistCardsAndIds{};
};

// TODO this should be working for attack defend and assist??
AttackDefendAssistCardsAndIds calcIdAndCompressedCardsForAttackAndDefend (durak::Game const &game);

struct AttackDefendAssistCardsToCompressedCards
{
  std::vector<std::tuple<durak::Card, durak::Card> > attackCardsAndCompressedCards{};
  std::vector<std::tuple<durak::Card, durak::Card> > defendCardsAndCompressedCards{};
  std::vector<std::tuple<durak::Card, durak::Card> > assistCardsAndCompressedCards{};
};

AttackDefendAssistCardsToCompressedCards calcCardsAndCompressedCardsForAttackAndDefend (durak::Game const &game);

std::vector<Action> historyEventsToActionsCompressedCards (std::vector<durak::HistoryEvent> const &histories, AttackDefendAssistCardsToCompressedCards const &attackDefendAssistCardsToCompressedCards);



}
