#include "durak_computer_controlled_opponent/solve.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/permutation.hxx"
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

using Histories = std::vector<durak::HistoryEvent>;

TEST_CASE ("solve multiple games", "[abc]")
{
  using namespace durak;
  auto results = std::array<std::vector<Round>, 4>{};
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          results.at (static_cast<size_t> (trumpType)).push_back (moveTree (std::vector<Card> (cards.begin (), cards.begin () + attackCardCount), std::vector<Card> (cards.begin () + attackCardCount, cards.end ()), trumpType));
        }
    }
  REQUIRE (results.at (0).at (3).attackIsWinning.size () == 1);
  REQUIRE (results.at (0).at (3).defendIsWinning.size () == 0);
  REQUIRE (results.at (0).at (3).draw.size () == 1);
}

TEST_CASE ("nextActions", "[abc]")
{
//  TODO write test
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  using namespace durak;

  // auto someRound = { { 0 }, { 1 } }, Type::clubs;
  // auto moveResult = binaryToMoveResult (someRound.combination);
  // auto result = nextActions ({ 0 }, moveResult);
  // REQUIRE (result == std::vector<std::tuple<uint8_t, Result> >{ { 1, Result::Draw }, { 253, Result::AttackWon } });
}
