#include "durak_computer_controlled_opponent/solve.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/permutation.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>
#include <confu_json/to_json.hxx>
#include <cstddef>
#include <cstdint>
#include <durak/card.hxx>
#include <durak/game.hxx>
#include <durak/gameOption.hxx>
#include <magic_enum.hpp>
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
  for (const auto& combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          results.at (static_cast<size_t> (trumpType)).push_back (moveTree (std::vector<Card> (cards.begin (), cards.begin () + attackCardCount), std::vector<Card> (cards.begin () + attackCardCount, cards.end ()), trumpType));
        }
    }
  REQUIRE (results.at (0).at (3).attackIsWinning.size () == 1);
  REQUIRE (results.at (0).at (3).defendIsWinning.empty());
  REQUIRE (results.at (0).at (3).draw.size () == 1);
}

TEST_CASE ("Action", "[abc]")
{
  SECTION ("action()==Action::Category::PlayCard")
  {
    auto action=Action{42};
    REQUIRE (action()==Action::Category::PlayCard);
  }
  SECTION ("action()==Action::Category::PassOrTakeCard")
  {
    auto action=Action{253};
    REQUIRE (action()==Action::Category::PassOrTakeCard);
  }
  SECTION ("action()==Action::Category::Undefined")
  {
    auto action=Action{254};
    REQUIRE (action()==Action::Category::Undefined);
  }
}


TEST_CASE ("nextActionsAndResults", "[abc]")
{
  using namespace durak;
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard= std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> {gameLookup.at ({1,1})};
  SECTION ("1v1 attack won")
  {
    auto actionsAndResults = nextActionsAndResults ({ }, oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }));
    REQUIRE (actionsAndResults == std::vector<std::tuple<Action, Result> >{ { Action{0}, Result::AttackWon } });
  }
}

TEST_CASE ("nextActionForRole")
{
  using namespace durak;
  auto   gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard= std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> {gameLookup.at ({1,1})};
  auto actionsAndResults = nextActionsAndResults ({}, oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }));
  auto nextAction = nextActionForRole (actionsAndResults, durak::PlayerRole::attack);
  REQUIRE (nextAction == Action{ 0 });
}

TEST_CASE ("solveGameTree")
{
  auto tree=st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> >{};
  tree.insert(std::tuple<Result, bool>{{},false});
  tree.root().insert(Action{1}, std::tuple<Result, bool>{{},true});
  tree.root()[Action{1}].insert(Action{0}, std::tuple<Result, bool>{Result::DefendWon,false});
  tree.root()[Action{1}].insert(Action{4}, std::tuple<Result, bool>{Result::Undefined,false});
  tree.root()[Action{1}][Action{4}].insert(Action{5}, std::tuple<Result, bool>{Result::Undefined,true});
  tree.root()[Action{1}][Action{4}][Action{5}].insert(Action{0}, std::tuple<Result, bool>{Result::Draw,false});
  tree.root()[Action{1}][Action{4}][Action{5}].insert(Action{253}, std::tuple<Result, bool>{Result::AttackWon,false});
  tree.root()[Action{1}].insert(Action{253}, std::tuple<Result, bool>{Result::AttackWon,false});
  tree.root().insert(Action{5}, std::tuple<Result, bool>{{},true});
  tree.root()[Action{5}].insert(Action{0}, std::tuple<Result, bool>{Result::Draw,false});
  tree.root()[Action{5}][Action{0}].insert(Action{1}, std::tuple<Result, bool>{Result::Undefined,true});
  tree.root()[Action{5}][Action{0}][Action{1}].insert(Action{4}, std::tuple<Result, bool>{Result::Draw,false});
  tree.root()[Action{5}][Action{0}][Action{1}].insert(Action{253}, std::tuple<Result, bool>{Result::AttackWon,false});
  tree.root()[Action{5}].insert(Action{4}, std::tuple<Result, bool>{Result::DefendWon,false});
  solveGameTree (tree);
  auto [result,attack]=tree.root()[Action{5}].data();
  REQUIRE(result==Result::DefendWon);
}
