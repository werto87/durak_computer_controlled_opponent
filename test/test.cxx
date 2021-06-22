#include "test/constant.hxx"
#include <catch2/catch.hpp>
#include <durak/game.hxx>
#include <durak/print.hxx>
#include <iostream>
#include <range/v3/range.hpp>
#include <vector>
using namespace durak;

GameData
filterGameDataByAccountName (durak::GameData const &gameData, std::string const &accountName)
{
  auto filteredGameData = gameData;
  for (auto &player : filteredGameData.players | std::ranges::views::filter ([&accountName] (auto const &player) { return player.name != accountName; }))
    {
      std::transform (player.cards.begin (), player.cards.end (), player.cards.begin (), [] (boost::optional<durak::Card> const &) { return boost::optional<durak::Card>{}; });
    }
  return filteredGameData;
}

// TEST_CASE ("allowed moves assisting player", "[game]")
// {
//   auto gameOption = GameOption{};
//   gameOption.customCardDeck = testCardDeck8 ();
//   auto game = Game{ { "player1", "player2" }, gameOption };
//   auto &attackingPlayer = game.getAttackingPlayer ().value ();
//   auto &defendingPlayer = game.getDefendingPlayer ().value ();
//   REQUIRE (game.getAllowedMoves (PlayerRole::attack).size () == 1);
//   REQUIRE (game.getAllowedMoves (PlayerRole::defend).size () == 0);
//   game.playerStartsAttack (attackingPlayer.cardsForIndex ({ 0 }));
//   REQUIRE (game.getAllowedMoves (PlayerRole::attack).size () == 1);
//   auto gameData = game.getGameData ();
//   auto filteredGameData = filterGameDataByAccountName (game.getGameData (), "player1");
// }

struct KnowenCards
{
  std::vector<Card> enemy{};
  std::vector<Card> graveYard{};
  std::vector<Card> player{};
  // TODO we know the last card in the deck
  // std::vector<Card> deck{};
  std::vector<Card> allCards{};
};

std::vector<Card>
unknowenCards (KnowenCards const &knowenCards)
{
  auto result = std::vector<Card>{};
  ranges::copy_if (knowenCards.allCards, std::back_inserter (result), [&knowenCards] (Card const &card) {
    if (ranges::find (knowenCards.enemy, card) != knowenCards.enemy.end ())
      {
        return false;
      }
    else if (ranges::find (knowenCards.graveYard, card) != knowenCards.enemy.end ())
      {
        return false;
      }
    else if (ranges::find (knowenCards.player, card) != knowenCards.enemy.end ())
      {
        return false;
      }
    // else if (ranges::find (knowenCards.deck, card) != knowenCards.deck.end ())
    //   {
    //     return false;
    //   }
    return true;
  });
  return result;
}

TEST_CASE ("unknowenCards all cards unknowen", "[game]")
{
  auto knowenCards = KnowenCards{};
  knowenCards.allCards = testCardDeck12 ();
  REQUIRE (unknowenCards (knowenCards).size () == 12);
}

TEST_CASE ("unknowenCards all cards knowen", "[game]")
{
  auto knowenCards = KnowenCards{};
  knowenCards.allCards = testCardDeck12 ();
  knowenCards.player = testCardDeck12 ();
  REQUIRE (unknowenCards (knowenCards).size () == 0);
}

Game playCardDefend (Game &game);

Game
playCardAttack (Game &game)
{
  if (game.checkIfGameIsOver ())
    {
      return game;
    }
  auto attackingPlayer = game.getAttackingPlayer ().value ();
  auto defendingPlayer = game.getDefendingPlayer ().value ();
  auto attackingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::attack);
  auto defendingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::defend);
  if (ranges::find (attackingPlayerAllowedMoves, Move::startAttack) != attackingPlayerAllowedMoves.end ())
    {
      std::vector<Card> winningCards{};
      std::vector<Card> losingCards{};
      std::vector<Card> drawingCards{};
      for (auto const &card : attackingPlayer.getCards ())
        {
          auto gameCopy = game;
          if (gameCopy.playerStartsAttack ({ card }))
            {
              gameCopy = playCardDefend (gameCopy);
              if (gameCopy.checkIfGameIsOver ())
                {
                  if (gameCopy.durak ())
                    {
                      if (gameCopy.durak ().value ().id != attackingPlayer.id)
                        {
                          winningCards.push_back (card);
                          // TODO think if its ok to be gready :D
                          break;
                        }
                      else
                        {
                          losingCards.push_back (card);
                        }
                    }
                  else
                    {
                      drawingCards.push_back (card);
                    }
                }
            }
        }
      if (not winningCards.empty ())
        {
          game.playerStartsAttack ({ winningCards.front () });
        }
      else if (not drawingCards.empty ())
        {
          game.playerStartsAttack ({ drawingCards.front () });
        }
      else if (not losingCards.empty ())
        {
          game.playerStartsAttack ({ losingCards.front () });
        }
      else
        {
          throw "STRANGE STATE";
        }
      return playCardDefend (game);
    }
  else if (ranges::find (attackingPlayerAllowedMoves, Move::addCard) != attackingPlayerAllowedMoves.end ())
    {
      std::vector<Card> winningCards{};
      std::vector<Card> losingCards{};
      std::vector<Card> drawingCards{};
      for (auto const &card : attackingPlayer.getCards ())
        {
          auto gameCopy = game;
          if (gameCopy.playerAssists (PlayerRole::attack, { card }))
            {
              gameCopy = playCardDefend (gameCopy);
              if (gameCopy.checkIfGameIsOver ())
                {
                  if (gameCopy.durak ())
                    {
                      if (gameCopy.durak ().value ().id != attackingPlayer.id)
                        {
                          winningCards.push_back (card);
                          // TODO think if its ok to be gready :D
                          break;
                        }
                      else
                        {
                          losingCards.push_back (card);
                        }
                    }
                  else
                    {
                      drawingCards.push_back (card);
                    }
                }
            }
        }
      if (not winningCards.empty ())
        {
          game.playerAssists (PlayerRole::attack, { winningCards.front () });
        }
      else if (not drawingCards.empty ())
        {
          game.playerAssists (PlayerRole::attack, { drawingCards.front () });
        }
      else if (not losingCards.empty ())
        {
          game.playerAssists (PlayerRole::attack, { losingCards.front () });
        }
      else
        {
          throw "STRANGE STATE";
        }
      return playCardDefend (game);
    }
  else if (ranges::find (attackingPlayerAllowedMoves, Move::pass) != attackingPlayerAllowedMoves.end ())
    {
      game.nextRound (false);
      return playCardAttack (game);
    }
  throw "NO POSIBLE MOVE BUT GAME IS NOT OVER";
  return game;
}

Game
playCardDefend (Game &game)
{
  if (game.checkIfGameIsOver ())
    {
      return game;
    }
  auto defendingPlayer = game.getDefendingPlayer ().value ();
  auto defendingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::defend);
  if (ranges::find (defendingPlayerAllowedMoves, Move::defend) != defendingPlayerAllowedMoves.end ())
    {
      std::vector<Card> winningCards{};
      std::vector<Card> losingCards{};
      std::vector<Card> drawingCards{};
      for (auto const &card : defendingPlayer.getCards ())
        {
          auto gameCopy = game;
          if (gameCopy.playerDefends (gameCopy.cardsNotBeatenOnTableWithIndex ().at (0).second, card))
            {
              gameCopy = playCardAttack (gameCopy);
              if (gameCopy.checkIfGameIsOver ())
                {
                  if (gameCopy.durak ())
                    {
                      if (gameCopy.durak ().value ().id != defendingPlayer.id)
                        {
                          winningCards.push_back (card);
                          // TODO think if its ok to be gready :D
                          break;
                        }
                      else
                        {
                          losingCards.push_back (card);
                        }
                    }
                  else
                    {
                      drawingCards.push_back (card);
                    }
                }
            }
        }
      if (not winningCards.empty ())
        {
          game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, winningCards.front ());
        }
      else if (not drawingCards.empty ())
        {
          game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, drawingCards.front ());
        }
      else if (not losingCards.empty ())
        {
          game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, losingCards.front ());
        }
      else
        {
          throw "STRANGE STATE";
        }
      return playCardAttack (game);
    }
  else if (ranges::find (defendingPlayerAllowedMoves, Move::takeCards) != defendingPlayerAllowedMoves.end ())
    {
      game.nextRound (true);
      return playCardAttack (game);
    }
  throw "NO POSIBLE MOVE BUT GAME IS NOT OVER";
  return game;
}

TEST_CASE ("callculate winner all cards knowen beating to the end player 2 should win", "[game]")
{
  auto gameState = GameState{};
  gameState.cardDeck = std::vector<Card>{}; // X
  auto player1 = Player{};
  player1.id = "Player1";
  player1.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
  gameState.players.push_back (player1);
  auto player2 = Player{};
  player2.id = "Player2";
  player2.takeCards ({ Card{ 1, Type::diamonds }, Card{ 2, Type::diamonds } }); // X
  gameState.players.push_back (player2);
  gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
  gameState.trump = Type::hearts;
  gameState.attackStarted = false;
  gameState.gameOver = false;
  gameState.round = defaultRoundToStart;
  gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
  auto game = Game{ gameState };
  REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
  REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
  auto result = playCardAttack (game);
  REQUIRE (result.checkIfGameIsOver ());
  REQUIRE (result.durak ().has_value ());
  REQUIRE (result.durak ().value ().id == "Player2");
}

TEST_CASE ("callculate winner all cards knowen beating to the end player 1 should win", "[game]")
{
  auto gameState = GameState{};
  gameState.cardDeck = std::vector<Card>{};
  auto player1 = Player{};
  player1.id = "Player1";
  player1.takeCards ({ Card{ 1, Type::diamonds }, Card{ 2, Type::diamonds } });
  gameState.players.push_back (player1);
  auto player2 = Player{};
  player2.id = "Player2";
  player2.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
  gameState.players.push_back (player2);
  gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
  gameState.trump = Type::hearts;
  gameState.attackStarted = false;
  gameState.gameOver = false;
  gameState.round = defaultRoundToStart;
  gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
  auto game = Game{ gameState };
  REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
  REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
  auto result = playCardAttack (game);
  REQUIRE (result.checkIfGameIsOver ());
  REQUIRE (result.durak ().has_value ());
  REQUIRE (result.durak ().value ().id == "Player1");
}

TEST_CASE ("callculate winner all cards knowen beating to the end draw", "[game]")
{
  auto gameState = GameState{};
  gameState.cardDeck = std::vector<Card>{};
  auto player1 = Player{};
  player1.id = "Player1";
  player1.takeCards ({ Card{ 1, Type::diamonds }, Card{ 1, Type::spades } });
  gameState.players.push_back (player1);
  auto player2 = Player{};
  player2.id = "Player2";
  player2.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
  gameState.players.push_back (player2);
  gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
  gameState.trump = Type::hearts;
  gameState.attackStarted = false;
  gameState.gameOver = false;
  gameState.round = defaultRoundToStart;
  gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
  auto game = Game{ gameState };
  REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
  REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
  auto result = playCardAttack (game);
  REQUIRE (result.checkIfGameIsOver ());
  REQUIRE (result.durak ().has_value () == false);
}

GameState
cards12 ()
{
  auto cardDeck = testCardDeck12 ();
  auto gameState = GameState{};
  auto player1 = Player{};
  player1.id = "Player1";
  player1.takeCards ({ cardDeck.end () - 12, cardDeck.end () - 6 });
  gameState.players.push_back (player1);
  auto player2 = Player{};
  player2.id = "Player2";
  player2.takeCards ({ cardDeck.end () - 6, cardDeck.end () });
  gameState.players.push_back (player2);
  gameState.trump = Type::hearts;
  return gameState;
}

TEST_CASE ("12 cards game", "[game]")
{
  // TODO improve performance
  auto game = Game{ cards12 () };
  auto result = playCardAttack (game);
  REQUIRE (result.checkIfGameIsOver ());
  REQUIRE_FALSE (result.durak ().has_value ());
  REQUIRE (result.getRound () == 3);
  auto history = result.getHistory ();
  for (auto historyElement : history)
    {
      std::cout << historyElement << std::endl;
    }
}