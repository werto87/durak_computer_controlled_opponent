#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>

TEST_CASE ("calcCardsAtRoundStart", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 1, Type::hearts }, { 1, Type::clubs }, { 1, Type::diamonds }, { 1, Type::spades } } } };
  auto attackCardsAtStart = game.getAttackingPlayer ()->getCards ();
  auto defendCardsAtStart = game.getDefendingPlayer ()->getCards ();
  {
    auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
  game.playerStartsAttack ({ durak::Card{ 1, durak::Type::clubs } });
  {
    auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
}

TEST_CASE ("calcCompressedCardsForAttackAndDefend", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 1, Type::hearts }, { 1, Type::clubs }, { 1, Type::diamonds }, { 1, Type::spades } } } };
  auto [attackCardsOld, defendCardsOld] = calcCompressedCardsForAttackAndDefend (game);
  game.playerStartsAttack ({ durak::Card{ 1, durak::Type::clubs } });
  auto [attackCards, defendCards] = calcCompressedCardsForAttackAndDefend (game);
  REQUIRE (attackCardsOld == attackCards);
  REQUIRE (defendCardsOld == defendCards);
}