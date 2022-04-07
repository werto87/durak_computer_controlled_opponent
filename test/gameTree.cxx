#include "src/cxx/compressCard.hxx"
#include "src/cxx/permutation.hxx"
#include "src/cxx/solve.hxx"
#include <catch2/catch.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <durak/card.hxx>
#include <durak/print.hxx>
#include <functional>
#include <magic_enum.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <st_tree.h>
inline const char *
indent_padding (size_t n)
{
  static char const spaces[] = "                                                                   ";
  static const unsigned ns = sizeof (spaces) / sizeof (*spaces);
  if (n >= ns) n = ns - 1;
  return spaces + (ns - 1 - n);
}

template <typename Iterator, typename Stream>
void
serialize_indented (const Iterator &F, const Iterator &L, Stream &s, size_t indent = 2)
{
  for (Iterator j (F); j != L; ++j)
    {
      s << indent_padding (j->ply () * indent);
      using namespace durak;
      s << ((j->ply () != 0) ? std::string{ "Node: " }.c_str () : std::string{ "" }.c_str ()) << ((j->key ().playedCard ()) ? j->key ().playedCard ().value () : Card{ 255, Type::hearts }) << " " << magic_enum::enum_name (std::get<0> (j->data ())) << (std::get<1> (j->data ()) ? std::string{ " Attack" } : std::string{ " Defend" });
      s << "\n";
    }
}

template <typename Tree, typename Stream>
void
serialize_indented (const Tree &t, Stream &s, size_t indent = 2)
{
  serialize_indented (t.df_pre_begin (), t.df_pre_end (), s, indent);
}

enum class Result : int8_t
{
  Undefined = -1,
  DefendWon = 0,
  Draw = 1,
  AttackWon = 2
};

void
setParentResultType (bool isProAttack, Result const &childResult, Result &parentResult)
{
  if (isProAttack)
    {
      if (magic_enum::enum_integer (childResult) > magic_enum::enum_integer (parentResult))
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
      auto const isAttackTurn = actions.at (i).playedCard () && ranges::find (attackCards, actions.at (i).playedCard ().value ()) != attackCards.end ();
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
      auto result = std::vector<Action>{};
      while (validActionSequence (actions, attackCards) == false)
        {
          for (auto i = size_t{ 0 }; i < actions.size (); i++)
            {
              auto const shouldBeAttackTurn = i % 2 == 0;
              auto const isAttackTurn = actions.at (i).playedCard () && ranges::find (attackCards, actions.at (i).playedCard ().value ()) != attackCards.end ();
              if (not shouldBeAttackTurn and isAttackTurn)
                {
                  actions.insert (actions.begin () + i, Action{});
                  break;
                }
            }
        }
      if (actions.size () % 2 != 0)
        {
          actions.push_back (Action{});
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

TEST_CASE ("gameTree ", "[abc]")
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto results = std::vector<std::tuple<std::vector<Card>, std::vector<Card>, Type, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto round = moveTree (attackCards, defendCards, trumpType);
          auto t = createTree (round);
          solveGameTree (t);
          results.push_back ({ attackCards, defendCards, trumpType, t });
        }
    }
  // std::cout << "attack cards:" << std::endl;
  // for (auto card : std::get<0> (results.at (42)))
  //   {
  //     std::cout << card << std::endl;
  //   }
  // std::cout << "defend cards:" << std::endl;
  // for (auto card : std::get<1> (results.at (42)))
  //   {
  //     std::cout << card << std::endl;
  //   }
  // std::cout << "trump: " << magic_enum::enum_name (std::get<2> (results.at (42))) << std::endl;
  // serialize_indented (std::get<3> (results.at (42)), std::cout);
}

using Histories = std::vector<durak::HistoryEvent>;
using Ids = std::vector<uint8_t>;
using ResultAndHistory = std::tuple<boost::optional<durak::Player>, Histories>;

Result
searchForGameResult (std::vector<uint8_t> const &attackCardsIds, std::vector<uint8_t> const &defendCardsIds, std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > > const &gameResults)
{
  if (auto gameResultTwoVersusTwo = ranges::find_if (gameResults,
                                                     [&attackCardsIds, &defendCardsIds] (std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > const &attackDefendCardsAndTree) {
                                                       auto const &[attackCardsIdsToCompare, defendCardsIdsToCompare, tree] = attackDefendCardsAndTree;
                                                       return attackCardsIds == attackCardsIdsToCompare && defendCardsIds == defendCardsIdsToCompare;
                                                     });
      gameResultTwoVersusTwo != gameResults.end ())
    {
      st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > const &resultTree = std::get<2> (*gameResultTwoVersusTwo);
      return std::get<0> (resultTree.root ().data ());
    }
  return Result::Undefined;
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

std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>
results1v1 ()
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto compresedGames = std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyse);
          auto histories = std::vector<ResultAndHistory>{};
          //   TODO use another solver. solver should solve one round and than look up the result of the game
          ranges::transform (tmpResults, ranges::back_inserter (histories), [] (Game const &game) { return std::make_tuple (game.durak (), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
          auto t = createTree (round);
          solveGameTree (t);
          compresedGames.at (static_cast<size_t> (trumpType)).push_back ({ cardsToIds (attackCards), cardsToIds (defendCards), t });
        }
    }
  return compresedGames;
}

std::tuple<std::vector<durak::Card>, std::vector<durak::Card> >
attackAndDefendCompressed (std::vector<durak::Card> const &attackCards, std::vector<durak::Card> const &defendCards)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card> >{};
  auto tmpVector = attackCards;
  tmpVector.insert (tmpVector.end (), defendCards.begin (), defendCards.end ());
  auto cards = compress (tmpVector);
  return { { cards.begin (), cards.begin () + attackCards.size () }, { cards.begin () + attackCards.size (), cards.end () } };
}

boost::optional<durak::Player>
calcGameResult (durak::Game const &game, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> > gameLookup)
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
              ranges::sort (attackCardsIds);
              auto defendCardsIds = cardsToIds (defendCardsCompressed);
              ranges::sort (defendCardsIds);
              return durakInGame (searchForGameResult (attackCardsIds, defendCardsIds, gameLookup.at ({ attackCardsIds.size (), defendCardsIds.size () }).at (static_cast<uint8_t> (game.getTrump ()))), game);
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

std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>
results2v2 (std::map<std::tuple<uint8_t, uint8_t>, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> > const &gameLookup)
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 2;
  size_t const defendCardCount = 2;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto compresedGames = std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyse);
          auto histories = std::vector<ResultAndHistory>{};
          ranges::transform (tmpResults, ranges::back_inserter (histories), [&gameLookup] (Game const &game) { return std::make_tuple (calcGameResultTwoVersusTwo (game, gameLookup), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
          auto t = createTree (round);
          solveGameTree (t);
          compresedGames.at (static_cast<size_t> (trumpType)).push_back ({ cardsToIds (attackCards), cardsToIds (defendCards), t });
        }
    }
  return compresedGames;
}

boost::optional<durak::Player>
calcGameResultThreeVersusOne (durak::Game const &game, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &result2v2)
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
              ranges::sort (attackCardsIds);
              auto defendCardsIds = cardsToIds (defendCardsCompressed);
              ranges::sort (defendCardsIds);
              auto const &gameResults = result2v2.at (static_cast<uint8_t> (game.getTrump ()));
              if (auto gameResult = ranges::find_if (gameResults,
                                                     [&attackCardsIds, &defendCardsIds] (std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > const &attackDefendCardsAndTree) {
                                                       auto const &[attackCardsIdsToCompare, defendCardsIdsToCompare, tree] = attackDefendCardsAndTree;
                                                       return attackCardsIds == attackCardsIdsToCompare && defendCardsIds == defendCardsIdsToCompare;
                                                     });
                  gameResult != gameResults.end ())
                {
                  st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > const &resultTree = std::get<2> (*gameResult);
                  auto result = std::get<0> (resultTree.root ().data ());
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
              else
                {
                  std::cout << "game should be in lookup but is not" << std::endl;
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

std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>
results3v1 (std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &results2v2)
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 3;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto compresedGames = std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyse);
          auto histories = std::vector<ResultAndHistory>{};
          ranges::transform (tmpResults, ranges::back_inserter (histories), [&results2v2] (Game const &game) { return std::make_tuple (calcGameResultThreeVersusOne (game, results2v2), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
          auto t = createTree (round);
          solveGameTree (t);
          compresedGames.at (static_cast<size_t> (trumpType)).push_back ({ cardsToIds (attackCards), cardsToIds (defendCards), t });
        }
    }
  return compresedGames;
}

boost::optional<durak::Player>
calcGameResultTwoVersusFour (durak::Game const &game, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &result3v1)
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
              auto const &gameResults = result3v1.at (static_cast<uint8_t> (game.getTrump ()));
              auto [attackCardsCompressed, defendCardsCompressed] = attackAndDefendCompressed (attackCards, defendCards);
              auto attackCardsIds = cardsToIds (attackCardsCompressed);
              ranges::sort (attackCardsIds);
              auto defendCardsIds = cardsToIds (defendCardsCompressed);
              ranges::sort (defendCardsIds);
              if (auto gameResult = ranges::find_if (gameResults,
                                                     [&attackCardsIds, &defendCardsIds] (std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > const &attackDefendCardsAndTree) {
                                                       auto const &[attackCardsIdsToCompare, defendCardsIdsToCompare, tree] = attackDefendCardsAndTree;
                                                       return attackCardsIds == attackCardsIdsToCompare && defendCardsIds == defendCardsIdsToCompare;
                                                     });
                  gameResult != gameResults.end ())
                {
                  st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > const &resultTree = std::get<2> (*gameResult);
                  auto result = std::get<0> (resultTree.root ().data ());
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
              else
                {
                  std::cout << "game should be in lookup but is not" << std::endl;
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

std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>
results2v4 (std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &threeVersusOne)
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 2;
  size_t const defendCardCount = 4;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto compresedGames = std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyse);
          auto histories = std::vector<ResultAndHistory>{};
          ranges::transform (tmpResults, ranges::back_inserter (histories), [&threeVersusOne] (Game const &game) { return std::make_tuple (calcGameResultTwoVersusFour (game, threeVersusOne), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
          auto t = createTree (round);
          solveGameTree (t);
          compresedGames.at (static_cast<size_t> (trumpType)).push_back ({ cardsToIds (attackCards), cardsToIds (defendCards), t });
        }
    }
  return compresedGames;
}

boost::optional<durak::Player>
calcGameResultThreeVersusThree (durak::Game const &game, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &oneVersusOne, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &twoVersusTwo, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &twoVersusFour)
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
              auto const &[attackCardsCompressed, defendCardsCompressed] = attackAndDefendCompressed (attackCards, defendCards);
              auto attackCardsIds = cardsToIds (attackCardsCompressed);
              ranges::sort (attackCardsIds);
              auto defendCardsIds = cardsToIds (defendCardsCompressed);
              ranges::sort (defendCardsIds);
              auto const &gameResultsToSearchIn = oneVersusOne.at (static_cast<uint8_t> (game.getTrump ()));
              auto const &gameResultsTwoVersusTwo = twoVersusTwo.at (static_cast<uint8_t> (game.getTrump ()));
              auto const &gameResultsTwoVersusFour = twoVersusFour.at (static_cast<uint8_t> (game.getTrump ()));
              auto result = Result::Undefined;
              if (attackCards.size () == 1 && defendCards.size () == 1)
                {
                  result = searchForGameResult (attackCardsIds, defendCardsIds, gameResultsToSearchIn);
                }
              else if (attackCards.size () == 2 && defendCards.size () == 2)
                {
                  result = searchForGameResult (attackCardsIds, defendCardsIds, gameResultsTwoVersusTwo);
                }
              else if (attackCards.size () == 2 && defendCards.size () == 4)
                {
                  result = searchForGameResult (attackCardsIds, defendCardsIds, gameResultsTwoVersusFour);
                }
              else
                {
                  std::cout << "game should be in lookup but is not" << std::endl;
                  abort ();
                }
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
        }
      else
        {
          std::cout << "game not over but no attacking or/and defending player" << std::endl;
          abort ();
        }
    }
  return {};
}

std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>
results3v3 (std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &oneVersusOne, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &twoVersusTwo, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> const &twoVersusFour)
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 3;
  size_t const defendCardCount = 3;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  std::cout << "combinations.size(): " << combinations.size () << std::endl;
  auto compresedGames = std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4>{};
  auto i = size_t{};
  for (auto combi : combinations)
    {
      if (i % 1000 == 0)
        {
          std::cout << "i: " << i << std::endl;
        }
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .trump = trumpType, .customCardDeck = std::vector<Card>{}, .cardsInHands = std::vector<std::vector<Card> >{ attackCards, defendCards } } };
          auto tmpResults = simulateRound (gameToAnalyse);
          auto histories = std::vector<ResultAndHistory>{};
          ranges::transform (tmpResults, ranges::back_inserter (histories), [&oneVersusOne, &twoVersusTwo, &twoVersusFour] (Game const &game) { return std::make_tuple (calcGameResultThreeVersusThree (game, oneVersusOne, twoVersusTwo, twoVersusFour), onlyFirstRound (game.getHistory ())); });
          auto round = Round{ gameToAnalyse.getAttackingPlayer ()->getCards (), gameToAnalyse.getDefendingPlayer ()->getCards (), histories };
          auto t = createTree (round);
          solveGameTree (t);
          compresedGames.at (static_cast<size_t> (trumpType)).push_back ({ cardsToIds (attackCards), cardsToIds (defendCards), t });
        }
      i++;
    }
  return compresedGames;
}

TEST_CASE ("simulate round ", "[abc]")
{
  typedef std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >, 4> GameResults;
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, GameResults>{};
  std::cout << "1v1" << std::endl;
  gameLookup.insert ({ { 1, 1 }, results1v1 () });
  // auto oneVersusOne = results1v1 ();
  std::cout << "2v2" << std::endl;
  auto twoVersusTwo = results2v2 (gameLookup);
  // std::cout << "3v1" << std::endl;
  // auto threeVersusOne = results3v1 (twoVersusTwo);
  // std::cout << "2v4" << std::endl;
  // auto twoVersusFour = results2v4 (threeVersusOne);
  // std::cout << "3v3" << std::endl;
  // auto threeVersusThree = results3v3 (oneVersusOne, twoVersusTwo, twoVersusFour);
  // auto const &results = threeVersusThree.at (magic_enum::enum_integer (durak::Type::hearts));
  // std::cout << "attack cards:" << std::endl;
  // auto const &result = results.at (42);
  // for (auto id : std::get<0> (result))
  //   {
  //     std::cout << idToCard (id) << std::endl;
  //   }
  // std::cout << "defend cards:" << std::endl;
  // for (auto id : std::get<1> (result))
  //   {
  //     std::cout << idToCard (id) << std::endl;
  //   }
  // std::cout << "trump: " << magic_enum::enum_name (durak::Type::hearts) << std::endl;
  // serialize_indented (std::get<2> (result), std::cout);
}

TEST_CASE ("insertDrawCardsAction", "[abc]")
{

  std::vector<durak::Card> attackCards{ { 0, durak::Type::hearts }, { 0, durak::Type::clubs }, { 0, durak::Type::diamonds } };
  std::vector<std::vector<Action> > vectorsOfActions{ { { 0 }, { 1 }, { 2 } } };
  REQUIRE (insertDrawCardsAction (attackCards, vectorsOfActions).size () == 6);
}
