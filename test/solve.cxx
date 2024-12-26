#include "durak_computer_controlled_opponent/solve.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/database.hxx"
#include "durak_computer_controlled_opponent/permutation.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>
#include <confu_json/to_json.hxx>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <cstdint>
#include <durak/card.hxx>
#include <durak/game.hxx>
#include <durak/gameOption.hxx>
#include <magic_enum/magic_enum.hpp>
#include <small_memory_tree/smallMemoryTree.hxx>
#include <soci/session.h>
#include <st_tree.h>
#include <tuple>

using namespace durak_computer_controlled_opponent;

TEST_CASE ("solve ", "[abc]")
{
  using namespace durak;
  auto gameToAnalyze = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 1, Type::hearts }, { 1, Type::clubs }, { 1, Type::diamonds }, { 1, Type::spades } } } };
  auto results = solve (gameToAnalyze);
  REQUIRE (results.size () == 6);
}

TEST_CASE ("solve multiple games", "[abc]")
{
  using namespace durak;
  auto results = std::array<std::vector<Round>, 4>{};
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  for (const auto &combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          results.at (static_cast<size_t> (trumpType)).push_back (moveTree (std::vector<Card> (cards.begin (), cards.begin () + attackCardCount), std::vector<Card> (cards.begin () + attackCardCount, cards.end ()), trumpType));
        }
    }
  REQUIRE (results.at (0).at (3).attackIsWinning.size () == 1);
  REQUIRE (results.at (0).at (3).defendIsWinning.empty ());
  REQUIRE (results.at (0).at (3).draw.size () == 1);
}

TEST_CASE ("Action", "[abc]")
{
  SECTION ("action()==Action::Category::PlayCard")
  {
    auto action = Action{ 42 };
    REQUIRE (action () == Action::Category::PlayCard);
  }
  SECTION ("action()==Action::Category::PassOrTakeCard")
  {
    auto action = Action{ 253 };
    REQUIRE (action () == Action::Category::PassOrTakeCard);
  }
  SECTION ("action()==Action::Category::Undefined")
  {
    auto action = Action{ 254 };
    REQUIRE (action () == Action::Category::Undefined);
  }
}
namespace durak_computer_controlled_opponent
{
st_tree::tree<std::tuple<Action, Result> > convertToNonKeyedTree (st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > const &tree);
}

TEST_CASE ("convertToNonKeyedTree", "[abc]")
{
  auto treeToConvert = st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> >{};
  treeToConvert.insert ({ Result::AttackWon, true });
  auto itr = treeToConvert.bf_begin ();
  itr->insert (Action{ 1 }, { Result::AttackWon, true });
  itr->insert (Action{ 2 }, { Result::AttackWon, true });
  ++itr;
  itr->insert (Action{ 33 }, { Result::AttackWon, true });
  itr->insert (Action{ 44 }, { Result::AttackWon, true });
  ++itr;
  itr->insert (Action{ 55 }, { Result::AttackWon, true });
  itr->insert (Action{ 66 }, { Result::AttackWon, true });
  itr->insert (Action{ 77 }, { Result::AttackWon, true });
  ++itr;
  ++itr;
  itr->insert (Action{ 111 }, { Result::AttackWon, true });
  auto result = convertToNonKeyedTree (treeToConvert);
  auto resultItr = result.bf_begin ();
  for (auto const &node : treeToConvert)
    {
      REQUIRE (node.size () == resultItr->size ());
      REQUIRE (std::get<0> (node.data ()) == std::get<1> (resultItr->data ()));
      REQUIRE (node.key () == std::get<0> (resultItr->data ()));
      ++resultItr;
    }
}

TEST_CASE ("nextActionsAndResults", "[abc]")
{
  using namespace durak;
  using LookUpTable = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4>;
  using CardLookUp = std::map<std::tuple<uint8_t, uint8_t>, LookUpTable>;
  auto gameLookup = CardLookUp{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = LookUpTable{ gameLookup.at ({ 1, 1 }) };
  SECTION ("1v1 attack won")
  {
    auto smt = small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >{ oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }) };
    auto actionsAndResults = nextActionsAndResults ({}, smt);
    REQUIRE (actionsAndResults == std::vector<std::tuple<Action, Result> >{ { Action{ 0 }, Result::AttackWon } });
  }
}

TEST_CASE ("nextActionsAndResults more than one action", "[abc]")
{
  using namespace durak;
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4>{ gameLookup.at ({ 1, 1 }) };
  SECTION ("1v1 attack draw")
  {
    auto actionsAndResults = nextActionsAndResults ({ Action{ 0 } }, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >{ oneCardVsOneCard.at (0).at ({ { 0 }, { 4 } }) });
    REQUIRE (actionsAndResults == std::vector<std::tuple<Action, Result> >{ { Action{ 4 }, Result::Draw }, { Action{ 253 }, Result::AttackWon } });
  }
}

TEST_CASE ("nextActionForRole")
{
  using namespace durak;
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4>{ gameLookup.at ({ 1, 1 }) };
  auto actionsAndResults = nextActionsAndResults ({}, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >{ oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }) });
  auto nextAction = nextActionForRole (actionsAndResults, durak::PlayerRole::attack);
  REQUIRE (nextAction == Action{ 0 });
}

TEST_CASE ("solveGameTree")
{
  auto tree = st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> >{};
  tree.insert (std::tuple<Result, bool>{ {}, false });
  tree.root ().insert (Action{ 1 }, std::tuple<Result, bool>{ {}, true });
  tree.root ()[Action{ 1 }].insert (Action{ 0 }, std::tuple<Result, bool>{ Result::DefendWon, false });
  tree.root ()[Action{ 1 }].insert (Action{ 4 }, std::tuple<Result, bool>{ Result::Undefined, false });
  tree.root ()[Action{ 1 }][Action{ 4 }].insert (Action{ 5 }, std::tuple<Result, bool>{ Result::Undefined, true });
  tree.root ()[Action{ 1 }][Action{ 4 }][Action{ 5 }].insert (Action{ 0 }, std::tuple<Result, bool>{ Result::Draw, false });
  tree.root ()[Action{ 1 }][Action{ 4 }][Action{ 5 }].insert (Action{ 253 }, std::tuple<Result, bool>{ Result::AttackWon, false });
  tree.root ()[Action{ 1 }].insert (Action{ 253 }, std::tuple<Result, bool>{ Result::AttackWon, false });
  tree.root ().insert (Action{ 5 }, std::tuple<Result, bool>{ {}, true });
  tree.root ()[Action{ 5 }].insert (Action{ 0 }, std::tuple<Result, bool>{ Result::Draw, false });
  tree.root ()[Action{ 5 }][Action{ 0 }].insert (Action{ 1 }, std::tuple<Result, bool>{ Result::Undefined, true });
  tree.root ()[Action{ 5 }][Action{ 0 }][Action{ 1 }].insert (Action{ 4 }, std::tuple<Result, bool>{ Result::Draw, false });
  tree.root ()[Action{ 5 }][Action{ 0 }][Action{ 1 }].insert (Action{ 253 }, std::tuple<Result, bool>{ Result::AttackWon, false });
  tree.root ()[Action{ 5 }].insert (Action{ 4 }, std::tuple<Result, bool>{ Result::DefendWon, false });
  solveGameTree (tree);
  auto [result, attack] = tree.root ()[Action{ 5 }].data ();
  REQUIRE (result == Result::DefendWon);
}
