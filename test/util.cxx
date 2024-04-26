#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>
TEST_CASE ("calcCardsAtRoundStart", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  auto attackingPlayer = game.getAttackingPlayer ();
  auto defendingPlayer = game.getDefendingPlayer ();
  REQUIRE (attackingPlayer.has_value ());
  REQUIRE (defendingPlayer.has_value ());
  auto attackCardsAtStart = attackingPlayer->getCards ();
  auto defendCardsAtStart = defendingPlayer->getCards ();
  {
    auto const &[attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  {
    auto [attackCards, defendCards, assistCards] = calcCardsAtRoundStart (game);
    std::ranges::sort (attackCards);
    std::ranges::sort (defendCards);
    std::ranges::sort (attackCardsAtStart);
    std::ranges::sort (defendCardsAtStart);
    REQUIRE (attackCardsAtStart == attackCards);
    REQUIRE (defendCardsAtStart == defendCards);
  }
}

TEST_CASE ("calcIdAndCompressedCardsForAttackAndDefend", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  auto [attackCardsOld, defendCardsOld, assistCardsOld] = calcIdAndCompressedCardsForAttackAndDefend (game);
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  auto [attackCards, defendCards, assistCards] = calcIdAndCompressedCardsForAttackAndDefend (game);
  REQUIRE (attackCardsOld == attackCards);
  REQUIRE (defendCardsOld == defendCards);
}

TEST_CASE ("calcCardsAndCompressedCardsForAttackAndDefend", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  auto [attackCardsOld, defendCardsOld, assistCardsOld] = calcCardsAndCompressedCardsForAttackAndDefend (game);
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  auto [attackCards, defendCards, assistCards] = calcCardsAndCompressedCardsForAttackAndDefend (game);
  REQUIRE (attackCardsOld == attackCards);
  REQUIRE (defendCardsOld == defendCards);
}

TEST_CASE ("historyEventsToActionsCompressedCards", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  auto actions = historyEventsToActionsCompressedCards (game.getHistory (), calcCardsAndCompressedCardsForAttackAndDefend (game));
  REQUIRE (actions == std::vector<Action>{ Action{ 1 } });
}

TEST_CASE ("historyEventsToActionsCompressedCards second round", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  game.playerDefends ({ 3, durak::Type::clubs }, { 7, durak::Type::clubs });
  game.nextRound (false);
  auto actions = historyEventsToActionsCompressedCards (game.getHistory (), calcCardsAndCompressedCardsForAttackAndDefend (game));
  REQUIRE (actions.empty ());
}

TEST_CASE ("historyEventsToActionsCompressedCards second round play card", "[abc]")
{
  using namespace durak;
  using namespace durak_computer_controlled_opponent;
  auto game = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  game.playerStartsAttack ({ durak::Card{ 3, durak::Type::clubs } });
  game.playerDefends ({ 3, durak::Type::clubs }, { 7, durak::Type::clubs });
  game.nextRound (false);
  REQUIRE (game.playerStartsAttack ({ durak::Card{ 8, durak::Type::clubs } }));
  auto actions = historyEventsToActionsCompressedCards (game.getHistory (), calcCardsAndCompressedCardsForAttackAndDefend (game));
  REQUIRE (actions == std::vector<Action>{ Action{ 5 } });
}
