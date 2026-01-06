#include "durak_computer_controlled_opponent/guess.hxx"
#include <catch2/catch.hpp>
#include <durak/game.hxx>

using namespace durak_computer_controlled_opponent;
using namespace durak_computer_controlled_opponent::guess;

durak::Game
simpleGame ()
{
  auto gameOption = durak::GameOption{};
  gameOption.customCardDeck = std::vector<durak::Card>{};
  gameOption.trump = durak::Type::hearts;
  gameOption.cardsInHands = std::vector<std::vector<durak::Card> >{ { { 1, durak::Type::clubs } }, { { 1, durak::Type::hearts } } };
  return durak::Game{ { "a", "b" }, gameOption };
}

durak::Game
simpleGameAttackPlayedCard ()
{
  auto game = simpleGame ();
  auto &player = game.getAttackingPlayer ().value ();
  game.playerStartsAttack (player.getCards ());
  return game;
}

TEST_CASE ("nextMoveToPlayForRole attack play card", "[abc]")
{
  auto const &moveToPlay = nextMoveToPlayForRole (simpleGame (), durak::PlayerRole::attack);
  REQUIRE (moveToPlay.value ().move == Move::PlayCard);
  REQUIRE (moveToPlay.value ().card.value () == durak::Card{ 1, durak::Type::clubs });
}

TEST_CASE ("nextMoveToPlayForRole play card", "[abc]")
{
  auto const &moveToPlay = nextMoveToPlayForRole (simpleGameAttackPlayedCard (), durak::PlayerRole::attack);
  REQUIRE (moveToPlay.error () == NextMoveToPlayForRoleError::couldNotFindAMoveToPlay);
}

TEST_CASE ("nextMoveToPlayForRole defend", "[abc]")
{
  auto const &moveToPlay = nextMoveToPlayForRole (simpleGameAttackPlayedCard (), durak::PlayerRole::defend);
  REQUIRE (moveToPlay.value ().move == Move::PlayCard);
  REQUIRE (moveToPlay.value ().card.value () == durak::Card{ 1, durak::Type::hearts });
}

TEST_CASE ("nextMoveToPlayForRole defend no move to play", "[abc]")
{
  auto const &moveToPlay = nextMoveToPlayForRole (simpleGame (), durak::PlayerRole::defend);
  REQUIRE (moveToPlay.error () == NextMoveToPlayForRoleError::couldNotFindAMoveToPlay);
}

TEST_CASE ("nextMoveToPlayForRole waiting no move to play", "[abc]")
{
  auto const &moveToPlay = nextMoveToPlayForRole (simpleGame (), durak::PlayerRole::waiting);
  REQUIRE (moveToPlay.error () == NextMoveToPlayForRoleError::couldNotFindAMoveToPlay);
}