#include "solve.hxx"
#include "compressCard.hxx"
#include "permutation.hxx"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <durak/game.hxx>
#include <magic_enum/magic_enum.hpp>
#include <small_memory_tree/smallMemoryTree.hxx>
#include <small_memory_tree/stTree.hxx>
#include <tuple>
#include <utility>
#include <vector>

namespace durak_computer_controlled_opponent
{
using Histories = std::vector<durak::HistoryEvent>;
using ResultAndHistory = std::tuple<boost::optional<durak::Player>, Histories>;
Histories
onlyFirstRound (std::vector<durak::HistoryEvent> const &histories)
{
  auto results = histories;
  auto isRoundInformation = [] (durak::HistoryEvent const &historyEvent) { return std::holds_alternative<durak::RoundInformation> (historyEvent); };
  if (auto firstRoundInformation = std::ranges::find_if (results, isRoundInformation); firstRoundInformation != results.end ())
    {
      if (auto secondRoundInformation = std::ranges::find_if (std::next (firstRoundInformation), results.end (), isRoundInformation); secondRoundInformation != results.end ())
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
  for (const auto &history : histories)
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
tableValidForMoveLookUp (const std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > &table)
{
  auto findNotBeatenCard = [] (std::pair<durak::Card, boost::optional<durak::Card> > const &cardAndCardToBeat) {
    auto const &[card, cardToBeat] = cardAndCardToBeat;
    return not cardToBeat.has_value ();
  };
  if (auto notBeaten = std::ranges::find_if (table, findNotBeatenCard); notBeaten != table.end ())
    {
      if (auto notBeaten2 = std::ranges::find_if (std::next (notBeaten), table.end (), findNotBeatenCard); notBeaten2 != table.end ())
        {
          return false;
        }
    }
  return true;
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

// std::optional<durak::Card>
// cardToPlay (Round const &round, std::vector<std::pair<durak::Card, boost::optional<durak::Card> > > const &cardsOnTable, durak::PlayerRole playerRole)
// {
//   if (auto actionsDone = tableToActions (cardsOnTable))
//     {
//       auto const &winningMoves = (playerRole == durak::PlayerRole::attack) ? round.attackIsWinning : round.defendIsWinning;
//       auto const &playerCards = (playerRole == durak::PlayerRole::attack) ? round.attackingPlayerCards : round.defendingPlayerCards;
//       if (auto winningCombination = ranges::find_if (winningMoves, [&actionsDone] (auto const &combi) { return ranges::starts_with (combi, actionsDone._value ()); }); winningCombination != winningMoves.end ())
//         {
//           if (actionsDone->size () < winningCombination->size ())
//             {
//               auto cardToPlay = winningCombination->at (actionsDone->size ()).playedCard ();
//               if (ranges::find (playerCards, cardToPlay) != playerCards.end ())
//                 {
//                   return cardToPlay;
//                 }
//               else
//                 {
//                   return std::nullopt;
//                 }
//             }
//           else
//             {
//               return std::nullopt;
//             }
//         }
//       else
//         {
//           auto const &drawMoves = round.draw;
//           if (auto combination = ranges::find_if (drawMoves, [&actionsDone] (auto const &combi) { return ranges::starts_with (combi, actionsDone._value ()); }); combination != drawMoves.end ())
//             {
//               if (actionsDone->size () < combination->size ())
//                 {
//                   auto cardToPlay = combination->at (actionsDone->size ()).playedCard ();
//                   if (ranges::find (playerCards, cardToPlay) != playerCards.end ())
//                     {
//                       return cardToPlay;
//                     }
//                   else
//                     {
//                       return std::nullopt;
//                     }
//                 }
//               else
//                 {
//                   return std::nullopt;
//                 }
//             }
//           else
//             {
//               return std::nullopt;
//             }
//         }
//     }
//   else
//     {
//       return std::nullopt;
//     }
// }

std::vector<std::tuple<durak::Game, AttackDefendPass> >
startAttack (std::tuple<durak::Game, AttackDefendPass> const &gameWithPasses)
{
  auto const &[game, attackDefendPass] = gameWithPasses;
  using namespace durak;
  auto result = std::vector<std::tuple<durak::Game, AttackDefendPass> >{};
  auto allowedMoves = game.getAllowedMoves (PlayerRole::attack);
  if (std::ranges::find (allowedMoves, Move::startAttack) != allowedMoves.end ())
    {
      for (auto const &card : game.getAttackingPlayer ()->getCards ())
        {
          result.emplace_back (gameWithPasses);
          if (not std::get<0> (result.back ()).playerStartsAttack ({ card }))
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
  if (std::ranges::find (allowedMoves, Move::addCard) != allowedMoves.end ())
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
  if (std::ranges::find (allowedMovesAttack, Move::startAttack) != allowedMovesAttack.end ())
    {
      results = startAttack (gameWithPasses);
    }
  else if (std::ranges::find (allowedMovesAttack, Move::addCard) != allowedMovesAttack.end ())
    {
      results = addCard (gameWithPasses);
    }
  else if (std::ranges::find (allowedMovesAttack, Move::pass) != allowedMovesAttack.end () or attackDefendPass.defendPass)
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
        return std::get<1> (gameAndDefPass).attackPass && std::get<1> (gameAndDefPass).defendPass
               && (std::ranges::find_if (resultsWithPass,
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
      auto const notFinishedRound = [] (std::tuple<Game, AttackDefendPass> const &gameAndDefPass) { return not(std::get<1> (gameAndDefPass).attackPass && std::get<1> (gameAndDefPass).defendPass); };
      for (const auto &startedGameWithPass : startedGamesWithPass)
        {
          auto defendResults = defendAction (startedGameWithPass);
          std::ranges::copy_if (defendResults, std::back_inserter (tmpResults), notFinishedRound);
          std::ranges::copy_if (defendResults, std::back_inserter (resultsWithPass), finishedRoundAndNewResult);
        }
      auto myResults = std::vector<std::tuple<Game, AttackDefendPass> >{};
      for (const auto &tmpResult : tmpResults)
        {
          auto attackResults = attackAction (tmpResult);
          std::ranges::copy_if (attackResults, std::back_inserter (myResults), notFinishedRound);
          std::ranges::copy_if (attackResults, std::back_inserter (resultsWithPass), finishedRoundAndNewResult);
        }
      startedGamesWithPass = myResults;
    }
  for (auto &[result, pass] : resultsWithPass)
    {
      result.nextRound (result.countOfNotBeatenCardsOnTable () != 0);
    }
  auto results = std::vector<Game>{};
  std::ranges::transform (resultsWithPass, std::back_inserter (results), [] (auto const &resultWithPass) { return std::get<0> (resultWithPass); });
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
  if (std::ranges::find (allowedMovesDefend, Move::defend) != allowedMovesDefend.end ())
    {
      auto tryDefendResults = defend (gameWithPasses);
      result.insert (result.end (), tryDefendResults.begin (), tryDefendResults.end ());
    }
  if (std::ranges::find (allowedMovesDefend, Move::takeCards) != allowedMovesDefend.end ())
    {
      result.emplace_back (gameWithPasses);
      std::get<1> (result.back ()).defendPass = true;
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
  if (std::ranges::find (allowedMoves, Move::defend) != allowedMoves.end ())
    {
      for (auto const &card : game.getDefendingPlayer ()->getCards ())
        {
          result.emplace_back (gameWithPasses);
          auto table = std::get<0> (result.back ()).getTable ();
          if (auto firstCardToBeat = std::ranges::find_if (table, [] (auto cardToBeatAndCard) { return not cardToBeatAndCard.second.has_value (); }); firstCardToBeat != table.end ())
            {
              std::get<0> (result.back ()).playerDefends (firstCardToBeat->first, card);
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
  if (_value >= 253)
    {
      return std::nullopt;
    }
  else
    {
      return idToCard (_value);
    }
}

std::uint8_t
Action::value () const
{
  return _value;
}
Action::Category
Action::operator() () const
{
  if (_value > 253)
    {
      return Category::Undefined;
    }
  else if (_value == 253)
    {
      return Category::PassOrTakeCard;
    }
  else
    {
      return Category::PlayCard;
    }
}

Round::Round (std::vector<durak::Card> attackingPlayerCards_, std::vector<durak::Card> const &defendingPlayerCards_, std::vector<ResultAndHistory> const &resultsAndHistories) : attackingPlayerCards{ std::move (attackingPlayerCards_) }, defendingPlayerCards{ defendingPlayerCards_ }
{
  for (auto [result, histories] : resultsAndHistories)
    {
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
  std::ranges::copy_if (tmpResults, std::back_inserter (finishedGames), [] (Game const &game_) { return game_.checkIfGameIsOver (); });
  std::ranges::copy_if (tmpResults, std::back_inserter (gamesToKeepPlaying), [] (Game const &game_) { return not game_.checkIfGameIsOver (); });
  while (not gamesToKeepPlaying.empty ())
    {
      auto gamesToAnalyze = std::move (gamesToKeepPlaying);
      gamesToKeepPlaying.clear ();
      for (auto const &gameToAnalyze : gamesToAnalyze)
        {
          auto results = simulateRound (gameToAnalyze);
          std::ranges::copy_if (results, std::back_inserter (finishedGames), [] (Game const &game_) { return game_.checkIfGameIsOver (); });
          std::ranges::copy_if (results, std::back_inserter (gamesToKeepPlaying), [] (Game const &game_) { return not game_.checkIfGameIsOver (); });
        }
    }
  return finishedGames;
}

Round
moveTree (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards, durak::Type trumpType)
{
  using namespace durak;
  auto playerCards = std::vector<std::vector<Card> >{ attackCards, defendCards };
  auto gameToAnalyze = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = playerCards } };
  auto histories = std::vector<ResultAndHistory>{};
  //   TODO use another solver. solver should solve one round and than look up the result of the game
  std::ranges::transform (solve (gameToAnalyze), std::back_inserter (histories), [] (Game const &game) { return std::make_tuple (game.durak (), onlyFirstRound (game.getHistory ())); });
  return Round{ gameToAnalyze.getAttackingPlayer ()->getCards (), gameToAnalyze.getDefendingPlayer ()->getCards (), histories };
}

using Histories = std::vector<durak::HistoryEvent>;
using ResultAndHistory = std::tuple<boost::optional<durak::Player>, Histories>;

void
setParentResultType (bool isProAttack, Result const &childResult, Result &parentResult)
{
  if (isProAttack)
    {
      if (parentResult == Result::Undefined || (magic_enum::enum_integer (childResult) > magic_enum::enum_integer (parentResult)))
        {
          parentResult = childResult;
        }
    }
  else
    {
      if (parentResult == Result::Undefined || (childResult != Result::Undefined && magic_enum::enum_integer (childResult) < magic_enum::enum_integer (parentResult)))
        {
          parentResult = childResult;
        }
    }
}

bool
validActionSequence (std::vector<Action> actions, std::vector<durak::Card> const &attackCards)
{
  for (auto i = size_t{ 0 }; i < actions.size (); i++)
    {
      auto const shouldBeAttackTurn = i % 2 == 0;
      auto const isAttackTurn = actions.at (i).playedCard () && std::ranges::find (attackCards, actions.at (i).playedCard ().value ()) != attackCards.end ();
      if (not shouldBeAttackTurn and isAttackTurn)
        {
          return false;
        }
    }
  return true;
}

std::vector<std::vector<Action> >
insertDrawCardsAction (std::vector<durak::Card> const &attackCards, std::vector<std::vector<Action> > const &vectorsOfActions)
{
  auto results = std::vector<std::vector<Action> >{};
  for (auto actions : vectorsOfActions)
    {
      while (not validActionSequence (actions, attackCards))
        {
          for (auto i = size_t{ 0 }; i < actions.size (); i++)
            {
              auto const shouldBeAttackTurn = i % 2 == 0;
              auto const isAttackTurn = actions.at (i).playedCard () && std::ranges::find (attackCards, actions.at (i).playedCard ().value ()) != attackCards.end ();
              if (not shouldBeAttackTurn and isAttackTurn)
                {
                  actions.insert (actions.begin () + static_cast<long> (i), Action{});
                  break;
                }
            }
        }
      if (actions.size () % 2 != 0)
        {
          actions.emplace_back ();
        }
      results.emplace_back (std::move (actions));
    }
  return results;
}

void
vectorWithMovesToTree (std::vector<durak::Card> const &attackCards, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > &tree, std::vector<std::vector<Action> > const &vectorsOfActions, Result result)
{
  for (auto actions : insertDrawCardsAction (attackCards, vectorsOfActions))
    {
      auto isAttack = true;
      auto node = tree.root ().insert (actions.at (0), std::tuple<Result, bool>{ Result::Undefined, true });
      for (auto i = size_t{ 1 }; i < actions.size (); ++i)
        {
          isAttack = !isAttack;
          node = node.first->insert (actions.at (i), std::tuple<Result, bool>{ (actions.size () - i == 1) ? result : Result::Undefined, isAttack });
        }
    }
}

st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> >
createTree (Round const &round)
{
  auto result = st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> >{};
  result.insert (std::tuple<Result, bool>{ Result::Undefined, true });
  vectorWithMovesToTree (round.attackingPlayerCards, result, round.attackIsWinning, Result::AttackWon);
  vectorWithMovesToTree (round.attackingPlayerCards, result, round.defendIsWinning, Result::DefendWon);
  vectorWithMovesToTree (round.attackingPlayerCards, result, round.draw, Result::Draw);
  return result;
}

Result
searchForGameResult (std::vector<uint8_t> const &attackCardsIds, std::vector<uint8_t> const &defendCardsIds, std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint16_t> > const &gameResults)
{
  if (auto result = gameResults.find ({ attackCardsIds, defendCardsIds }); result != gameResults.end ())
    {
      return std::get<1> (result->second.getNodes ().front ().value);
    }
  else
    {
      return Result::Undefined;
    }
}
boost::optional<durak::Player>
durakInGame (Result result, durak::Game const &game)
{
  if (result == Result::AttackWon)
    {
      return game.getDefendingPlayer ().value ();
    }
  else if (result == Result::DefendWon)
    {
      return game.getAttackingPlayer ().value ();
    }
  else if (result == Result::Draw)
    {
      return {};
    }
  else
    {
      std::cout << "game should have a winner or draw" << std::endl;
      abort ();
    }
}

std::tuple<std::vector<durak::Card>, std::vector<durak::Card> >
attackAndDefendCompressed (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card> >{};
  auto tmpVector = attackCards;
  tmpVector.insert (tmpVector.end (), defendCards.begin (), defendCards.end ());
  auto cards = compress (tmpVector);
  return { { cards.begin (), cards.begin () + static_cast<long> (attackCards.size ()) }, { cards.begin () + static_cast<long> (attackCards.size ()), cards.end () } };
}

boost::optional<durak::Player>
calcGameResult (durak::Game const &game, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint16_t> >, 4> > const &gameLookup)
{
  // find game in lookup
  if (game.checkIfGameIsOver ())
    {
      return game.durak ();
    }
  else
    {
      if (game.getAttackingPlayer () and game.getDefendingPlayer ())
        {
          auto const &attackCards = game.getAttackingPlayer ()->getCards ();
          auto const &defendCards = game.getDefendingPlayer ()->getCards ();
          if (attackCards.size () < 2 and defendCards.size () > 1)
            {
              return game.getDefendingPlayer ().value ();
            }
          else
            {
              auto [attackCardsCompressed, defendCardsCompressed] = attackAndDefendCompressed (attackCards, defendCards);
              auto attackCardsIds = cardsToIds (attackCardsCompressed);
              std::ranges::sort (attackCardsIds);
              auto defendCardsIds = cardsToIds (defendCardsCompressed);
              std::ranges::sort (defendCardsIds);
              if (gameLookup.count ({ attackCardsIds.size (), defendCardsIds.size () }) != 0)
                {
                  return durakInGame (searchForGameResult (attackCardsIds, defendCardsIds, gameLookup.at ({ attackCardsIds.size (), defendCardsIds.size () }).at (static_cast<uint8_t> (game.getTrump ()))), game);
                }
              else
                {
                  std::cout << "could not find " << std::to_string (attackCardsIds.size ()) << "v" << std::to_string (defendCardsIds.size ()) << " in game lookup" << std::endl;
                  abort ();
                }
            }
        }
      else
        {
          std::cout << "game not over but no attacking or/and defending player" << std::endl;
          abort ();
        }
    }
  return {};
}

st_tree::tree<std::tuple<Action, Result> >
convertToNonKeyedTree (st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > const &tree)
{
  auto result = st_tree::tree<std::tuple<Action, Result> >{};
  result.insert (std::make_tuple (tree.root ().key (), std::get<0> (tree.root ().data ())));
  auto itr = result.bf_begin ();
  for (auto const &node : tree)
    {
      for (auto const &child : node)
        {
          itr->insert (std::make_tuple (child.key (), std::get<0> (child.data ())));
        }
      ++itr;
    }
  return result;
}

std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint16_t> >, 4>
solveDurak (size_t n, size_t attackCardCount, size_t defendCardCount, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint16_t> >, 4> > const &gameLookup)
{
  using namespace durak;
  auto _combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  using namespace small_memory_tree;
  auto compressedGames = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, SmallMemoryTree<std::tuple<Action, Result>, uint16_t> >, 4>{};
  auto skippedCombinations = uint64_t{};
  for (const auto &combi : _combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + static_cast<long> (attackCardCount));
          auto defendCards = std::vector<Card> (cards.begin () + static_cast<long> (attackCardCount), cards.end ());
          auto gameToAnalyze = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyze);
          auto histories = std::vector<ResultAndHistory>{};
          std::ranges::transform (tmpResults, std::back_inserter (histories), [&gameLookup] (Game const &game) { return std::make_tuple (calcGameResult (game, gameLookup), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyze.getAttackingPlayer ()->getCards (), gameToAnalyze.getDefendingPlayer ()->getCards (), histories };
          auto tree = createTree (round);
          solveGameTree (tree);
          try
            {
              auto smt = SmallMemoryTree<std::tuple<Action, Result>, uint16_t>{ StTreeAdapter{ convertToNonKeyedTree (tree) } };
              compressedGames.at (static_cast<size_t> (trumpType)).insert ({ { cardsToIds (attackCards), cardsToIds (defendCards) }, smt });
            }
          catch (boost::numeric::positive_overflow const &e)
            {
              compressedGames.at (static_cast<size_t> (trumpType)).insert ({ { cardsToIds (attackCards), cardsToIds (defendCards) }, {} });
              std::cout << "exception in solveDurak." << std::endl;
              std::cout << e.what ();
              std::cout << "trump: " << magic_enum::enum_name (trumpType) << std::endl;
              auto const cardsAsIdsAsString = [] (std::vector<durak::Card> const &_cards) -> std::string {
                auto result = std::stringstream{};
                std::ranges::copy (cardsToIds (_cards), std::ostream_iterator<int> (result, " "));
                return result.str ();
              };
              std::cout << "attack cards: " << cardsAsIdsAsString (attackCards) << std::endl;
              std::cout << "defend cards: " << cardsAsIdsAsString (defendCards) << std::endl;
              ++skippedCombinations;
            }
        }
    }
  if (skippedCombinations) std::cout << "skippedCombinations: " << skippedCombinations << std::endl;
  return compressedGames;
}

std::vector<std::tuple<Action, Result> >
nextActionsAndResults (std::vector<Action> const &actions, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result>, uint16_t> const &moveResults)
{
  auto const &rootElement = moveResults.getNodes ().front ().value;
  if (auto childrenOpt = small_memory_tree::calcChildrenForPath (moveResults, { rootElement }))
    {
      auto path = std::decay_t<decltype (childrenOpt.value ())>{ rootElement };
      for (auto const &action : actions)
        {
          if (auto childResultToPlay = std::ranges::find_if (childrenOpt.value (), [action] (std::tuple<Action, Result> const &moveResult) { return action == std::get<0> (moveResult); }); childResultToPlay != childrenOpt.value ().end ())
            {
              path.push_back (*childResultToPlay);
              childrenOpt = small_memory_tree::calcChildrenForPath (moveResults, path);
              if (not childrenOpt.has_value ())
                {
                  throw std::logic_error{ "could not find action for path." };
                }
            }
        }
      auto result = std::vector<std::tuple<Action, Result> >{};
      std::ranges::transform (childrenOpt.value (), std::back_inserter (result), [] (auto const &valueResult) {
        auto const &[value, _result] = valueResult;
        return std::tuple<Action, Result>{ value, _result };
      });
      return result;
    }
  else
    {
      throw std::logic_error{ "could not find action for path." };
    }
}

std::optional<durak_computer_controlled_opponent::Action>
nextActionForRole (const std::vector<std::tuple<Action, durak_computer_controlled_opponent::Result> > &nextActions, const durak::PlayerRole &playerRole)
{
  if (playerRole == durak::PlayerRole::attack || playerRole == durak::PlayerRole::defend)
    {
      if (auto winningAction = std::ranges::find_if (nextActions,
                                                     [&playerRole] (auto const &actionAsBinaryAndResult) {
                                                       auto const &[actionAsBinary, result] = actionAsBinaryAndResult;
                                                       if (playerRole == durak::PlayerRole::attack)
                                                         {
                                                           return result == durak_computer_controlled_opponent::Result::AttackWon;
                                                         }
                                                       else
                                                         {
                                                           return result == durak_computer_controlled_opponent::Result::DefendWon;
                                                         }
                                                     });
          winningAction != nextActions.end ())
        {
          return { durak_computer_controlled_opponent::Action{ std::get<0> (*winningAction) } };
        }
      else if (auto drawAction = std::ranges::find_if (nextActions,
                                                       [] (auto const &actionAsBinaryAndResult) {
                                                         auto const &[actionAsBinary, result] = actionAsBinaryAndResult;
                                                         return result == durak_computer_controlled_opponent::Result::Draw;
                                                       });
               drawAction != nextActions.end ())
        {
          return { durak_computer_controlled_opponent::Action{ std::get<0> (*drawAction) } };
        }
      else if (not nextActions.empty ())
        {
          return { durak_computer_controlled_opponent::Action{ std::get<0> (nextActions.front ()) } };
        }
      else
        {
          return std::nullopt;
        }
    }
  return std::nullopt;
}
void
solveGameTree (st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > &t)
{
  auto nodes = std::map<decltype (t.df_pre_begin ()->ply ()), std::vector<decltype (t.df_pre_begin ())> >{};
  for (auto begin = t.df_pre_begin (); begin != t.df_pre_end (); ++begin)
    {
      if (not begin->is_root ())
        {
          nodes[begin->ply ()].push_back (begin);
        }
    }
  std::ranges::for_each (nodes.rbegin (), nodes.rend (), [] (auto const &keyValue) {
    for (auto const &node : keyValue.second)
      {
        setParentResultType (std::get<1> (node->data ()), std::get<0> (node->data ()), std::get<0> (node->parent ().data ()));
      }
  });
}
}