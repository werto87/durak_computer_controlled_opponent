#ifndef E408197E_0D11_4D88_B43B_B5D6B657C114
#define E408197E_0D11_4D88_B43B_B5D6B657C114

#include <cstdint>
#include <durak/game.hxx>
#include <st_tree.h>
#include <vector>

namespace database
{
struct Round;
}

std::vector<durak::HistoryEvent> onlyFirstRound (std::vector<durak::HistoryEvent> const &histories);

class Action
{
public:
  Action () = default;
  Action (std::uint8_t cardPlayed_) : cardPlayed (cardPlayed_) {}
  auto operator<=> (const Action &) const = default;
  std::optional<durak::Card> playedCard () const;

  [[nodiscard ("FALSE if card value invalid")]] bool playCard (durak::Card const &card);

  std::uint8_t value () const;

private:
  std::uint8_t cardPlayed{ 253 };
};

std::optional<Action> historyEventToAction (durak::HistoryEvent const &historyEvent);

std::vector<Action> historyEventsToActions (std::vector<durak::HistoryEvent> const &histories);

template <typename T>
void
sortUniqueErase (T &t)
{
  ranges::sort (t);
  t.erase (ranges::unique (t), t.end ());
}

struct Round
{
  Round (std::vector<durak::Card> const &attackingPlayerCards_, std::vector<durak::Card> const &defendingPlayerCards_, std::vector<std::tuple<boost::optional<durak::Player>, std::vector<durak::HistoryEvent> > > const &resultsAndHistories);
  Round (database::Round const &databaseRound);
  std::vector<durak::Card> attackingPlayerCards{};
  std::vector<durak::Card> defendingPlayerCards{};
  std::vector<std::vector<Action> > attackIsWinning{};
  std::vector<std::vector<Action> > defendIsWinning{};
  std::vector<std::vector<Action> > draw{};
};

bool tableValidForMoveLookUp (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table);

std::optional<std::vector<Action> > tableToActions (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table);

std::ostream &operator<< (std::ostream &os, const Action &action);

std::optional<durak::Card> cardToPlay (Round const &round, std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &cardsOnTable, durak::PlayerRole playerRole);

struct AttackDefendPass
{
  bool attackPass = false;
  bool defendPass = false;
};

std::vector<std::tuple<durak::Game, AttackDefendPass> > startAttack (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses);

std::vector<std::tuple<durak::Game, AttackDefendPass> > addCard (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses);

std::vector<std::tuple<durak::Game, AttackDefendPass> > defend (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses);

std::vector<std::tuple<durak::Game, AttackDefendPass> > attackAction (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses);

std::vector<std::tuple<durak::Game, AttackDefendPass> > defendAction (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses);

std::vector<durak::Game> simulateRound (durak::Game const &game);

std::vector<durak::Game> solve (durak::Game const &game);

Round moveTree (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards, durak::Type trumpType);

enum class Result : uint8_t
{
  Undefined,
  DefendWon,
  Draw,
  AttackWon
};

void setParentResultType (bool isProAttack, Result const &childResult, Result &parentResult);

bool validActionSequence (std::vector<Action> actions, std::vector<durak::Card> const &attackCards);

std::vector<std::vector<Action> > insertDrawCardsAction (std::vector<durak::Card> const &attackCards, std::vector<std::vector<Action> > const &vectorsOfActions);

void vectorWithMovesToTree (std::vector<durak::Card> const &attackCards, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > &tree, std::vector<std::vector<Action> > const &vectorsOfActions, Result result);

st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > createTree (Round const &round);

template <typename T>
void
solveGameTree (T &t)
{
  auto leafs = std::vector<decltype (t.df_pre_begin ())>{};
  for (auto begin = t.df_pre_begin (); begin != t.df_pre_end (); ++begin)
    {
      if (begin->empty ())
        {
          leafs.push_back (begin);
        }
    }
  for (auto &node : leafs)
    {
      auto itr = node->parent ().begin ();
      while (true)
        {
          setParentResultType (std::get<1> (itr->data ()), std::get<0> (itr->data ()), std::get<0> (itr->parent ().data ()));
          if (not itr->parent ().is_root ())
            {
              itr = itr->parent ().parent ().begin ();
            }
          else
            {
              break;
            }
        }
    }
}

Result searchForGameResult (std::vector<uint8_t> const &attackCardsIds, std::vector<uint8_t> const &defendCardsIds, std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > > const &gameResults);
boost::optional<durak::Player> durakInGame (Result result, durak::Game const &game);

std::tuple<std::vector<durak::Card>, std::vector<durak::Card> > attackAndDefendCompressed (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards);

boost::optional<durak::Player> calcGameResult (durak::Game const &game, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> > const &gameLookup);

std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> solveDurak (size_t n, size_t attackCardCount, size_t defendCardCount, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> > const &gameLookup);

#endif /* E408197E_0D11_4D88_B43B_B5D6B657C114 */