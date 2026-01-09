#include "durak_computer_controlled_opponent/guess.hxx"
#include <catch2/catch.hpp>
#include <durak/game.hxx>
#include <ranges>

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

durak::Game
fullGame ()
{
  using namespace durak;
  auto gameOption = durak::GameOption{};
  gameOption.customCardDeck = std::vector<Card>{ { 2, Type::diamonds }, { 7, Type::clubs }, { 8, Type::clubs }, { 3, Type::hearts }, { 3, Type::clubs }, { 3, Type::diamonds }, { 2, Type::clubs }, { 5, Type::diamonds }, { 6, Type::diamonds }, { 7, Type::diamonds }, { 8, Type::diamonds }, { 0, Type::diamonds }, { 1, Type::spades }, { 2, Type::spades }, { 3, Type::spades }, { 1, Type::diamonds }, { 5, Type::spades }, { 6, Type::spades }, { 7, Type::spades }, { 8, Type::spades }, { 0, Type::spades }, { 1, Type::hearts }, { 2, Type::hearts }, { 0, Type::clubs }, { 1, Type::clubs }, { 5, Type::hearts }, { 6, Type::clubs }, { 7, Type::hearts }, { 8, Type::hearts }, { 0, Type::hearts }, { 4, Type::hearts }, { 4, Type::spades }, { 4, Type::clubs }, { 5, Type::clubs }, { 6, Type::hearts }, { 4, Type::diamonds } };
  return Game{ { "a", "b" }, gameOption };
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

TEST_CASE ("nextMoveToPlayForRole play whole game", "[abc]")
{
  auto game = fullGame ();
  while (not game.checkIfGameIsOver ())
    {
      auto const &attackMoveToPlay = nextMoveToPlayForRole (game, durak::PlayerRole::attack);
      if (attackMoveToPlay)
        {
          switch (attackMoveToPlay.value ().move)
            {
            case Move::PlayCard:
              {
                if (game.getAttackStarted ())
                  {
                    game.playerAssists (durak::PlayerRole::attack, { attackMoveToPlay.value ().card.value () });
                    break;
                  }
                else
                  {
                    game.playerStartsAttack ({ attackMoveToPlay.value ().card.value () });
                    break;
                  }
              }
            case Move::PassOrTakeCard:
              {
                // nothing to do here attack pass is not a move game understands
                break;
              }
            default:
              break;
            }
        }
      auto const &defendMoveToPlay = nextMoveToPlayForRole (game, durak::PlayerRole::defend);
      if (defendMoveToPlay)
        {
          switch (defendMoveToPlay.value ().move)
            {
            case Move::PlayCard:
              {
                for (auto const &[card, unused] : game.getTable () | std::views::filter ([] (auto const &cardToBeatAndCardOptional) { return not std::get<1> (cardToBeatAndCardOptional).has_value (); }))
                  {
                    if (durak::beats (card, defendMoveToPlay.value ().card.value (), game.getTrump ()))
                      {
                        game.playerDefends (card, { defendMoveToPlay.value ().card.value () });
                        break;
                      }
                  }
                break;
              }
            case Move::PassOrTakeCard:
              {
                auto allowedMoves = game.getAllowedMoves (durak::PlayerRole::defend);
                if (allowedMoves.size () == 1 and allowedMoves.front () == durak::Move::takeCards)
                  {
                    game.nextRound (std::ranges::any_of (game.getTable (), [] (auto const &cardAndOptionalCard) { return not std::get<1> (cardAndOptionalCard).has_value (); }));
                  }
                break;
              }
            default:
              break;
            }
        }
      if (not(attackMoveToPlay or defendMoveToPlay))
        {
          REQUIRE (false);
        }
    }
}

TEST_CASE ("nextMoveToPlayForRole play whole game fuzzing", "[.fuzzing]")
{
  auto game = fullGame ();
  while (not game.checkIfGameIsOver ())
    {
      auto const &attackMoveToPlay = nextMoveToPlayForRole (game, durak::PlayerRole::attack);
      if (attackMoveToPlay)
        {
          switch (attackMoveToPlay.value ().move)
            {
            case Move::PlayCard:
              {
                if (game.getAttackStarted ())
                  {
                    game.playerAssists (durak::PlayerRole::attack, { attackMoveToPlay.value ().card.value () });
                    break;
                  }
                else
                  {
                    game.playerStartsAttack ({ attackMoveToPlay.value ().card.value () });
                    break;
                  }
              }
            case Move::PassOrTakeCard:
              {
                // nothing to do here attack pass is not a move game understands
                break;
              }
            default:
              break;
            }
        }
      auto const &defendMoveToPlay = nextMoveToPlayForRole (game, durak::PlayerRole::defend);
      if (defendMoveToPlay)
        {
          switch (defendMoveToPlay.value ().move)
            {
            case Move::PlayCard:
              {
                for (auto const &[card, unused] : game.getTable () | std::views::filter ([] (auto const &cardToBeatAndCardOptional) { return not std::get<1> (cardToBeatAndCardOptional).has_value (); }))
                  {
                    if (durak::beats (card, defendMoveToPlay.value ().card.value (), game.getTrump ()))
                      {
                        game.playerDefends (card, { defendMoveToPlay.value ().card.value () });
                        break;
                      }
                  }
                break;
              }
            case Move::PassOrTakeCard:
              {
                auto allowedMoves = game.getAllowedMoves (durak::PlayerRole::defend);
                if (allowedMoves.size () == 1 and allowedMoves.front () == durak::Move::takeCards)
                  {
                    game.nextRound (std::ranges::any_of (game.getTable (), [] (auto const &cardAndOptionalCard) { return not std::get<1> (cardAndOptionalCard).has_value (); }));
                  }
                break;
              }
            default:
              break;
            }
        }
      if (not(attackMoveToPlay or defendMoveToPlay))
        {
          REQUIRE (false);
        }
    }
}