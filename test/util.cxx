#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>
#include <range/v3/algorithm.hpp>
TEST_CASE ("calcCardsAtRoundStart", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  auto attackCardsAtStart = game.getAttackingPlayer ()->getCards ();
  auto defendCardsAtStart = game.getDefendingPlayer ()->getCards ();
  {
    auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  {
    auto [attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    ranges::sort (attackCards);
    ranges::sort (defendCards);
    ranges::sort (attackCardsAtStart);
    ranges::sort (defendCardsAtStart);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
}

TEST_CASE ("calcCompressedCardsForAttackAndDefend", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  auto [attackCardsOld, defendCardsOld] = calcCompressedCardsForAttackAndDefend (game);
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  auto [attackCards, defendCards] = calcCompressedCardsForAttackAndDefend (game);
  REQUIRE (attackCardsOld == attackCards);
  REQUIRE (defendCardsOld == defendCards);
}