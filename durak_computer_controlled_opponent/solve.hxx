#ifndef E408197E_0D11_4D88_B43B_B5D6B657C114
#define E408197E_0D11_4D88_B43B_B5D6B657C114

#include <cstdint>
#include <durak/game.hxx>
#include <small_memory_tree/smallMemoryTree.hxx>
#include <st_tree.h>
#include <vector>

namespace durak_computer_controlled_opponent
{

std::vector<durak::HistoryEvent> onlyFirstRound (std::vector<durak::HistoryEvent> const &histories);

class Action
{
public:
  enum struct Category
  {
    Undefined,
    PlayCard,
    PassOrTakeCard
  };

  Action () = default;
  explicit Action (std::uint8_t value) : _value (value) {}
  // clang-format off
  auto operator<=> (const Action &) const = default;
  // clang-format on
  [[nodiscard]] Category operator() () const;
  [[nodiscard]] std::optional<durak::Card> playedCard () const;

  [[nodiscard]] std::uint8_t value () const;

private:
  std::uint8_t _value{ 253 };
};

std::optional<Action> historyEventToAction (durak::HistoryEvent const &historyEvent);

std::vector<Action> historyEventsToActions (std::vector<durak::HistoryEvent> const &histories);

template <typename T>
void
sortUniqueErase (T &t)
{
  std::ranges::sort (t);
  t.erase (std::unique (t.begin (), t.end ()), t.end ());
}

struct Round
{
  Round (std::vector<durak::Card> attackingPlayerCards_, std::vector<durak::Card> const &defendingPlayerCards_, std::vector<std::tuple<boost::optional<durak::Player>, std::vector<durak::HistoryEvent> > > const &resultsAndHistories);
  std::vector<durak::Card> attackingPlayerCards{};
  std::vector<durak::Card> defendingPlayerCards{};
  std::vector<std::vector<Action> > attackIsWinning{};
  std::vector<std::vector<Action> > defendIsWinning{};
  std::vector<std::vector<Action> > draw{};
};

std::optional<std::vector<Action> > tableToActions (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table);

std::ostream &operator<< (std::ostream &os, const Action &action);

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
std::optional<Action> nextActionForRole (const std::vector<std::tuple<Action, Result> > &nextActions, const durak::PlayerRole &playerRole);

std::vector<std::tuple<Action, Result> > nextActionsAndResults (std::vector<Action> const &actions, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint64_t> const &moveResults);

bool validActionSequence (std::vector<Action> actions, std::vector<durak::Card> const &attackCards);

std::vector<std::vector<Action> > insertDrawCardsAction (std::vector<durak::Card> const &attackCards, std::vector<std::vector<Action> > const &vectorsOfActions);

void vectorWithMovesToTree (std::vector<durak::Card> const &attackCards, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > &tree, std::vector<std::vector<Action> > const &vectorsOfActions, Result result);

st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > createTree (Round const &round);

void solveGameTree (st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > &t);

Result searchForGameResult (std::vector<uint8_t> const &attackCardsIds, std::vector<uint8_t> const &defendCardsIds, std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint64_t> > const &gameResults);
boost::optional<durak::Player> durakInGame (Result result, durak::Game const &game);

std::tuple<std::vector<durak::Card>, std::vector<durak::Card> > attackAndDefendCompressed (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards);

boost::optional<durak::Player> calcGameResult (durak::Game const &game, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint64_t> >, 4> > const &gameLookup);

bool tableValidForMoveLookUp (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table);

std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint64_t> >, 4> solveDurak (size_t n, size_t attackCardCount, size_t defendCardCount, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint64_t> >, 4> > const &gameLookup);
}
#endif /* E408197E_0D11_4D88_B43B_B5D6B657C114 */