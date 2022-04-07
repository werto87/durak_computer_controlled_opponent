#include "solve.hxx"
#include "src/cxx/compressCard.hxx"
#include <cstddef>
#include <durak/game.hxx>
#include <range/v3/algorithm/find_if.hpp>
#include <vector>
using Histories = std::vector<durak::HistoryEvent>;
using Ids = std::vector<uint8_t>;
using ResultAndHistory = std::tuple<boost::optional<durak::Player>, Histories>;
Histories
onlyFirstRound (std::vector<durak::HistoryEvent> const &histories)
{
  auto results = histories;
  auto isRoundInformation = [] (durak::HistoryEvent const &historyEvent) { return std::holds_alternative<durak::RoundInformation> (historyEvent); };
  if (auto firstRoundInformation = ranges::find_if (results, isRoundInformation); firstRoundInformation != results.end ())
    {
      if (auto secondRoundInformation = ranges::find_if (std::next (firstRoundInformation), results.end (), isRoundInformation); secondRoundInformation != results.end ())
        {
          results.erase (secondRoundInformation, results.end ());
        }
    }
  return results;
}

std::optional<Action>
historyEventToAction (durak::HistoryEvent const &historyEvent)
{
  if (std::holds_alternative<durak::RoundInformation> (historyEvent))
    {
      return std::nullopt;
    }
  else if (std::holds_alternative<durak::StartAttack> (historyEvent))
    {
      return Action{ cardToId (get<durak::StartAttack> (historyEvent).cards.at (0)) };
    }
  else if (std::holds_alternative<durak::AssistAttack> (historyEvent))
    {
      return Action{ cardToId (get<durak::AssistAttack> (historyEvent).cards.at (0)) };
    }
  else if (std::holds_alternative<durak::Pass> (historyEvent))
    {
      return Action{};
    }
  else if (std::holds_alternative<durak::Defend> (historyEvent))
    {
      return Action{ cardToId (get<durak::Defend> (historyEvent).card) };
    }
  else if (std::holds_alternative<durak::DrawCardsFromTable> (historyEvent))
    {
      return Action{};
    }
  else
    {
      std::cout << "not handled HistoryEvent" << std::endl;
      abort ();
      return std::nullopt;
    }
}

std::vector<Action>
historyEventsToActions (Histories const &histories)
{
  auto result = std::vector<Action>{};
  for (auto history : histories)
    {
      if (auto action = historyEventToAction (history))
        {
          if (action->playedCard ())
            {
              result.push_back (action.value ());
            }
        }
    }
  return result;
}

bool
tableValidForMoveLookUp (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table)
{
  auto findNotBeatenCard = [] (std::pair<durak::Card, boost::optional<durak::Card> > const &cardAndCardToBeat) {
    auto const &[card, cardToBeat] = cardAndCardToBeat;
    return not cardToBeat.has_value ();
  };
  if (auto notBeaten = ranges::find_if (table, findNotBeatenCard); notBeaten != table.end ())
    {
      if (auto notBeaten2 = ranges::find_if (std::next (notBeaten), table.end (), findNotBeatenCard); notBeaten2 != table.end ())
        {
          return false;
        }
    }
  return true;
}

std::optional<std::vector<Action> >
tableToActions (std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &table)
{
  if (tableValidForMoveLookUp (table))
    {
      auto result = std::vector<Action>{};
      for (auto const &[card, cardToBeat] : table)
        {
          result.push_back (cardToId (card));
          if (cardToBeat)
            {
              result.push_back (cardToId (cardToBeat.value ()));
            }
        }
      return result;
    }
  else
    {
      return std::nullopt;
    }
}

std::ostream &
operator<< (std::ostream &os, const Action &action)
{
  if (auto card = action.playedCard ())
    {
      os << static_cast<size_t> (cardToId (card.value ()));
    }
  else
    {
      os << "'Take Cards'";
    }
  return os;
}

std::optional<durak::Card>
cardToPlay (Round const &round, std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &cardsOnTable, durak::PlayerRole playerRole)
{
  if (auto actionsDone = tableToActions (cardsOnTable))
    {
      auto const &winningMoves = (playerRole == durak::PlayerRole::attack) ? round.attackIsWinning : round.defendIsWinning;
      auto const &playerCards = (playerRole == durak::PlayerRole::attack) ? round.attackingPlayerCards : round.defendingPlayerCards;
      if (auto winningCombination = ranges::find_if (winningMoves, [&actionsDone] (auto const &combi) { return ranges::starts_with (combi, actionsDone.value ()); }); winningCombination != winningMoves.end ())
        {
          if (actionsDone->size () < winningCombination->size ())
            {
              auto cardToPlay = winningCombination->at (actionsDone->size ()).playedCard ();
              if (ranges::find (playerCards, cardToPlay) != playerCards.end ())
                {
                  return cardToPlay;
                }
              else
                {
                  return std::nullopt;
                }
            }
          else
            {
              return std::nullopt;
            }
        }
      else
        {
          auto const &drawMoves = round.draw;
          if (auto combination = ranges::find_if (drawMoves, [&actionsDone] (auto const &combi) { return ranges::starts_with (combi, actionsDone.value ()); }); combination != drawMoves.end ())
            {
              return std::nullopt;
            }
          else
            {
              return std::nullopt;
            }
        }
    }
  else
    {
      return std::nullopt;
    }
}

std::vector<std::tuple<durak::Game, AttackDefendPass> >
startAttack (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto result = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  auto allowedMoves = game.getAllowedMoves (PlayerRole::attack);
  if (ranges::find (allowedMoves, Move::startAttack) != allowedMoves.end ())
    {
      auto results = std::vector<Game>{};
      for (auto const &card : game.getAttackingPlayer ()->getCards ())
        {
          auto &[gameToStart, passes] = result.emplace_back (gameWithPasses);
          if (gameToStart.playerStartsAttack ({ card }))
            {
            }
          else
            {
              std::cout << "error starting round" << std::endl;
              abort ();
            }
        }
    }
  return result;
}

std::vector<std::tuple<durak::Game, AttackDefendPass> >
addCard (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto result = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  auto allowedMoves = game.getAllowedMoves (PlayerRole::attack);
  if (ranges::find (allowedMoves, Move::addCard) != allowedMoves.end ())
    {
      for (auto const &card : game.getAttackingPlayer ()->getCards ())
        {
          auto gameToAnalyze = game;
          if (gameToAnalyze.playerAssists (PlayerRole::attack, { card }))
            {
              result.emplace_back (std::tuple<durak::Game, AttackDefendPass>{ std::move (gameToAnalyze), attackDefendPass });
            }
        }
    }
  return result;
}

std::vector<std::tuple<durak::Game, AttackDefendPass> >
attackAction (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto allowedMovesAttack = game.getAllowedMoves (PlayerRole::attack);
  auto results = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  if (ranges::find (allowedMovesAttack, Move::startAttack) != allowedMovesAttack.end ())
    {
      results = startAttack (gameWithPasses);
    }
  else if (ranges::find (allowedMovesAttack, Move::addCard) != allowedMovesAttack.end ())
    {
      results = addCard (gameWithPasses);
    }
  else if (ranges::find (allowedMovesAttack, Move::pass) != allowedMovesAttack.end () or attackDefendPass.defendPass)
    {
      results.push_back ({ game, { true, attackDefendPass.defendPass } });
    }
  // auto tmpResults = results;
  // for (auto &[tmpResult, pass] : tmpResults)
  //   {
  //     if (pass.attackPass == false)
  //       {
  //         pass.attackPass = true;
  //         results.push_back ({ tmpResult, pass });
  //       }
  //   }
  // TODO think about taking cards and adding cards and passing
  //  when def passes attack can give cards but do not has to
  //  maybe first implementation with dump all cards??

  return results;
}

std::vector<durak::Game>
simulateRound (durak::Game const &game)
{
  using namespace durak;
  auto startedGamesWithPass = attackAction ({ game, {} });
  auto resultsWithPass = std::vector<std::tuple<Game, AttackDefendPass> >{};
  auto gamesToPlay = std::vector<std::tuple<Game, AttackDefendPass> >{};
  while (not startedGamesWithPass.empty ())
    {
      auto tmpResults = std::vector<std::tuple<Game, AttackDefendPass> >{};
      auto const finishedRoundAndNewResult = [&resultsWithPass] (std::tuple<Game, AttackDefendPass> const &gameAndDefPass) {
        return std::get<1> (gameAndDefPass).attackPass == true && std::get<1> (gameAndDefPass).defendPass == true
               && (ranges::find_if (resultsWithPass,
                                    [&game = std::get<0> (gameAndDefPass)] (std::tuple<Game, AttackDefendPass> const &resultWithPass) {
                                      // TODO find out why std::equal does not work with game.getHistory () == std::get<0> (resultWithPass).getHistory ()
                                      std::vector<HistoryEvent> events1 = std::get<0> (resultWithPass).getHistory ();
                                      std::vector<HistoryEvent> events2 = game.getHistory ();
                                      if (events1.size () == events2.size ())
                                        {
                                          auto equal = true;
                                          for (size_t i = 0; i < events1.size (); i++)
                                            {
                                              if (events1.at (i) != events2.at (i))
                                                {
                                                  equal = false;
                                                  break;
                                                }
                                            }
                                          return equal;
                                        }
                                      else
                                        {
                                          return false;
                                        }
                                    })
                   == resultsWithPass.end ());
      };
      auto const notFinishedRound = [] (std::tuple<Game, AttackDefendPass> const &gameAndDefPass) { return not(std::get<1> (gameAndDefPass).attackPass == true && std::get<1> (gameAndDefPass).defendPass == true); };
      for (auto startedGameWithPass : startedGamesWithPass)
        {
          auto defendResults = defendAction (startedGameWithPass);
          ranges::copy_if (defendResults, ranges::back_inserter (tmpResults), notFinishedRound);
          ranges::copy_if (defendResults, ranges::back_inserter (resultsWithPass), finishedRoundAndNewResult);
        }
      auto myResults = std::vector<std::tuple<Game, AttackDefendPass> >{};
      for (auto tmpResult : tmpResults)
        {
          auto attackResults = attackAction (tmpResult);
          ranges::copy_if (attackResults, ranges::back_inserter (myResults), notFinishedRound);
          ranges::copy_if (attackResults, ranges::back_inserter (resultsWithPass), finishedRoundAndNewResult);
        }
      startedGamesWithPass = myResults;
    }
  for (auto &[result, pass] : resultsWithPass)
    {
      result.nextRound (result.countOfNotBeatenCardsOnTable () != 0);
    }
  auto results = std::vector<Game>{};
  ranges::transform (resultsWithPass, ranges::back_inserter (results), [] (auto const &resultWithPass) { return std::get<0> (resultWithPass); });
  return results;
}

std::vector<std::tuple<durak::Game, AttackDefendPass> >
defendAction (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto result = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  // if (attackDefendPass.defendPass)
  //   {
  //     return { gameWithPasses };
  //   }
  // else
  //   {
  auto allowedMovesDefend = game.getAllowedMoves (PlayerRole::defend);
  if (ranges::find (allowedMovesDefend, Move::defend) != allowedMovesDefend.end ())
    {
      auto tryDefendResults = defend (gameWithPasses);
      result.insert (result.end (), tryDefendResults.begin (), tryDefendResults.end ());
    }
  if (ranges::find (allowedMovesDefend, Move::takeCards) != allowedMovesDefend.end ())
    {
      auto &[gameToStart, attackDefendPassToSet] = result.emplace_back (gameWithPasses);
      attackDefendPassToSet.defendPass = true;
    }
  // TODO implement something for defend wants to take cards. attack has the possibility to add cards when defend takes cards
  return result;
  // }
}

std::vector<std::tuple<durak::Game, AttackDefendPass> >
defend (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto result = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  auto allowedMoves = game.getAllowedMoves (PlayerRole::defend);
  if (ranges::find (allowedMoves, Move::defend) != allowedMoves.end ())
    {
      auto results = std::vector<Game>{};
      for (auto const &card : game.getDefendingPlayer ()->getCards ())
        {
          auto &[gameToStart, attackDefendPassToSet] = result.emplace_back (gameWithPasses);
          auto table = gameToStart.getTable ();
          if (auto firstCardToBeat = ranges::find_if (table, [] (auto cardToBeatAndCard) { return not cardToBeatAndCard.second.has_value (); }); firstCardToBeat != table.end ())
            {
              gameToStart.playerDefends (firstCardToBeat->first, card);
            }
          else
            {
              std::cout << "no card to beat" << std::endl;
              abort ();
            }
        }
    }
  return result;
}

std::optional<durak::Card>
Action::playedCard () const
{
  if (cardPlayed == 255)
    {
      return std::nullopt;
    }
  else
    {
      return idToCard (cardPlayed);
    }
}

bool
Action::playCard (durak::Card const &card)
{
  auto cardAsId = cardToId (card);
  if (cardAsId < 255)
    {
      cardPlayed = cardAsId;
      return true;
    }
  else
    {
      return false;
    }
}

Round::Round (std::vector<durak::Card> const &attackingPlayerCards_, std::vector<durak::Card> const &defendingPlayerCards_, std::vector<ResultAndHistory> const &resultsAndHistories) : attackingPlayerCards{ attackingPlayerCards_ }, defendingPlayerCards{ defendingPlayerCards_ }
{
  for (auto [result, histories] : resultsAndHistories)
    {
      auto historyEventsToActionsTmp = historyEventsToActions (histories);
      if (result.has_value ())
        {
          if (result->id == "a")
            // the id is the id of the LOSER
            {
              defendIsWinning.push_back (historyEventsToActions (histories));
            }
          else
            {
              attackIsWinning.push_back (historyEventsToActions (histories));
            }
        }
      else
        {
          draw.push_back (historyEventsToActions (histories));
        }
    }
  sortUniqueErase (attackIsWinning);
  sortUniqueErase (defendIsWinning);
  sortUniqueErase (draw);
}

std::vector<durak::Game>
solve (durak::Game const &game)
{
  // TODO rethink this algorithm. Solve 1vs1 then solve 2vs2 and 1v3 and 3vs1 using the results from 1vs1 and so on. ignore blunders and find a solution later for this
  using namespace durak;
  auto gamesToKeepPlaying = std::vector<durak::Game>{};
  auto finishedGames = std::vector<durak::Game>{};
  auto tmpResults = simulateRound (game);
  ranges::copy_if (tmpResults, ranges::back_inserter (finishedGames), [] (Game const &game) { return game.checkIfGameIsOver (); });
  ranges::copy_if (tmpResults, ranges::back_inserter (gamesToKeepPlaying), [] (Game const &game) { return not game.checkIfGameIsOver (); });
  while (not gamesToKeepPlaying.empty ())
    {
      auto gamesToAnalyze = std::move (gamesToKeepPlaying);
      gamesToKeepPlaying.clear ();
      for (auto &gameToAnalyze : gamesToAnalyze)
        {
          auto results = simulateRound (gameToAnalyze);
          ranges::copy_if (results, ranges::back_inserter (finishedGames), [] (Game const &game) { return game.checkIfGameIsOver (); });
          ranges::copy_if (results, ranges::back_inserter (gamesToKeepPlaying), [] (Game const &game) { return not game.checkIfGameIsOver (); });
        }
    }
  return finishedGames;
}

Round
moveTree (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards, durak::Type trumpType)
{
  using namespace durak;
  auto playerCards = std::vector<std::vector<Card> >{ attackCards, defendCards };
  auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = playerCards } };
  auto histories = std::vector<ResultAndHistory>{};
  //   TODO use another solver. solver should solve one round and than look up the result of the game
  ranges::transform (solve (gameToAnalyse), ranges::back_inserter (histories), [] (Game const &game) { return std::make_tuple (game.durak (), onlyFirstRound (game.getHistory ())); });
  return Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
}
