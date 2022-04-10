#ifndef E408197E_0D11_4D88_B43B_B5D6B657C114
#define E408197E_0D11_4D88_B43B_B5D6B657C114

#include <durak/game.hxx>
#include <vector>

std::vector<durak::HistoryEvent> onlyFirstRound (std::vector<durak::HistoryEvent> const &histories);

class Action
{
public:
  Action () = default;
  Action (std::uint8_t cardPlayed_) : cardPlayed (cardPlayed_) {}
  auto operator<=> (const Action &) const = default;
  std::optional<durak::Card> playedCard () const;

  [[nodiscard ("FALSE if card value invalid")]] bool playCard (durak::Card const &card);

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

#endif /* E408197E_0D11_4D88_B43B_B5D6B657C114 */