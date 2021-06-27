#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "test/constant.hxx"
#include <array>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <catch2/catch.hpp>
#include <durak/game.hxx>
#include <durak/print.hxx>
#include <execution>
#include <iomanip>
#include <iostream>
#include <range/v3/range.hpp>
#include <vector>
using namespace durak;

// GameData
// filterGameDataByAccountName (durak::GameData const &gameData, std::string const &accountName)
// {
//   auto filteredGameData = gameData;
//   for (auto &player : filteredGameData.players | std::ranges::views::filter ([&accountName] (auto const &player) { return player.name != accountName; }))
//     {
//       std::transform (player.cards.begin (), player.cards.end (), player.cards.begin (), [] (boost::optional<durak::Card> const &) { return boost::optional<durak::Card>{}; });
//     }
//   return filteredGameData;
// }

// // TEST_CASE ("allowed moves assisting player", "[game]")
// // {
// //   auto gameOption = GameOption{};
// //   gameOption.customCardDeck = testCardDeck8 ();
// //   auto game = Game{ { "player1", "player2" }, gameOption };
// //   auto &attackingPlayer = game.getAttackingPlayer ().value ();
// //   auto &defendingPlayer = game.getDefendingPlayer ().value ();
// //   REQUIRE (game.getAllowedMoves (PlayerRole::attack).size () == 1);
// //   REQUIRE (game.getAllowedMoves (PlayerRole::defend).size () == 0);
// //   game.playerStartsAttack (attackingPlayer.cardsForIndex ({ 0 }));
// //   REQUIRE (game.getAllowedMoves (PlayerRole::attack).size () == 1);
// //   auto gameData = game.getGameData ();
// //   auto filteredGameData = filterGameDataByAccountName (game.getGameData (), "player1");
// // }

// struct KnowenCards
// {
//   std::vector<Card> enemy{};
//   std::vector<Card> graveYard{};
//   std::vector<Card> player{};
//   // TODO we know the last card in the deck
//   // std::vector<Card> deck{};
//   std::vector<Card> allCards{};
// };

// std::vector<Card>
// unknowenCards (KnowenCards const &knowenCards)
// {
//   auto result = std::vector<Card>{};
//   ranges::copy_if (knowenCards.allCards, std::back_inserter (result), [&knowenCards] (Card const &card) {
//     if (ranges::find (knowenCards.enemy, card) != knowenCards.enemy.end ())
//       {
//         return false;
//       }
//     else if (ranges::find (knowenCards.graveYard, card) != knowenCards.enemy.end ())
//       {
//         return false;
//       }
//     else if (ranges::find (knowenCards.player, card) != knowenCards.enemy.end ())
//       {
//         return false;
//       }
//     // else if (ranges::find (knowenCards.deck, card) != knowenCards.deck.end ())
//     //   {
//     //     return false;
//     //   }
//     return true;
//   });
//   return result;
// }

// TEST_CASE ("unknowenCards all cards unknowen", "[game]")
// {
//   auto knowenCards = KnowenCards{};
//   knowenCards.allCards = testCardDeck12 ();
//   REQUIRE (unknowenCards (knowenCards).size () == 12);
// }

// TEST_CASE ("unknowenCards all cards knowen", "[game]")
// {
//   auto knowenCards = KnowenCards{};
//   knowenCards.allCards = testCardDeck12 ();
//   knowenCards.player = testCardDeck12 ();
//   REQUIRE (unknowenCards (knowenCards).size () == 0);
// }

// Game playCardDefend (Game &game);

// Game
// playCardAttack (Game &game)
// {
//   if (game.checkIfGameIsOver ())
//     {
//       return game;
//     }
//   auto attackingPlayer = game.getAttackingPlayer ().value ();
//   auto defendingPlayer = game.getDefendingPlayer ().value ();
//   auto attackingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::attack);
//   auto defendingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::defend);
//   if (ranges::find (attackingPlayerAllowedMoves, Move::startAttack) != attackingPlayerAllowedMoves.end ())
//     {
//       std::vector<Card> winningCards{};
//       std::vector<Card> losingCards{};
//       std::vector<Card> drawingCards{};
//       for (auto const &card : attackingPlayer.getCards ())
//         {
//           auto gameCopy = game;
//           if (gameCopy.playerStartsAttack ({ card }))
//             {
//               gameCopy = playCardDefend (gameCopy);
//               if (gameCopy.checkIfGameIsOver ())
//                 {
//                   if (gameCopy.durak ())
//                     {
//                       if (gameCopy.durak ().value ().id != attackingPlayer.id)
//                         {
//                           winningCards.push_back (card);
//                           // TODO think if its ok to be gready :D
//                           break;
//                         }
//                       else
//                         {
//                           losingCards.push_back (card);
//                         }
//                     }
//                   else
//                     {
//                       drawingCards.push_back (card);
//                     }
//                 }
//             }
//         }
//       if (not winningCards.empty ())
//         {
//           game.playerStartsAttack ({ winningCards.front () });
//         }
//       else if (not drawingCards.empty ())
//         {
//           game.playerStartsAttack ({ drawingCards.front () });
//         }
//       else if (not losingCards.empty ())
//         {
//           game.playerStartsAttack ({ losingCards.front () });
//         }
//       else
//         {
//           throw "STRANGE STATE";
//         }
//       return playCardDefend (game);
//     }
//   else if (ranges::find (attackingPlayerAllowedMoves, Move::addCard) != attackingPlayerAllowedMoves.end ())
//     {
//       std::vector<Card> winningCards{};
//       std::vector<Card> losingCards{};
//       std::vector<Card> drawingCards{};
//       for (auto const &card : attackingPlayer.getCards ())
//         {
//           auto gameCopy = game;
//           if (gameCopy.playerAssists (PlayerRole::attack, { card }))
//             {
//               gameCopy = playCardDefend (gameCopy);
//               if (gameCopy.checkIfGameIsOver ())
//                 {
//                   if (gameCopy.durak ())
//                     {
//                       if (gameCopy.durak ().value ().id != attackingPlayer.id)
//                         {
//                           winningCards.push_back (card);
//                           // TODO think if its ok to be gready :D
//                           break;
//                         }
//                       else
//                         {
//                           losingCards.push_back (card);
//                         }
//                     }
//                   else
//                     {
//                       drawingCards.push_back (card);
//                     }
//                 }
//             }
//         }
//       if (not winningCards.empty ())
//         {
//           game.playerAssists (PlayerRole::attack, { winningCards.front () });
//         }
//       else if (not drawingCards.empty ())
//         {
//           game.playerAssists (PlayerRole::attack, { drawingCards.front () });
//         }
//       else if (not losingCards.empty ())
//         {
//           game.playerAssists (PlayerRole::attack, { losingCards.front () });
//         }
//       else
//         {
//           throw "STRANGE STATE";
//         }
//       return playCardDefend (game);
//     }
//   else if (ranges::find (attackingPlayerAllowedMoves, Move::pass) != attackingPlayerAllowedMoves.end ())
//     {
//       game.nextRound (false);
//       return playCardAttack (game);
//     }
//   throw "NO POSIBLE MOVE BUT GAME IS NOT OVER";
//   return game;
// }

// Game
// playCardDefend (Game &game)
// {
//   if (game.checkIfGameIsOver ())
//     {
//       return game;
//     }
//   auto defendingPlayer = game.getDefendingPlayer ().value ();
//   auto defendingPlayerAllowedMoves = game.getAllowedMoves (PlayerRole::defend);
//   if (ranges::find (defendingPlayerAllowedMoves, Move::defend) != defendingPlayerAllowedMoves.end ())
//     {
//       std::vector<Card> winningCards{};
//       std::vector<Card> losingCards{};
//       std::vector<Card> drawingCards{};
//       for (auto const &card : defendingPlayer.getCards ())
//         {
//           auto gameCopy = game;
//           if (gameCopy.playerDefends (gameCopy.cardsNotBeatenOnTableWithIndex ().at (0).second, card))
//             {
//               gameCopy = playCardAttack (gameCopy);
//               if (gameCopy.checkIfGameIsOver ())
//                 {
//                   if (gameCopy.durak ())
//                     {
//                       if (gameCopy.durak ().value ().id != defendingPlayer.id)
//                         {
//                           winningCards.push_back (card);
//                           // TODO think if its ok to be gready :D
//                           break;
//                         }
//                       else
//                         {
//                           losingCards.push_back (card);
//                         }
//                     }
//                   else
//                     {
//                       drawingCards.push_back (card);
//                     }
//                 }
//             }
//         }
//       if (not winningCards.empty ())
//         {
//           game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, winningCards.front ());
//         }
//       else if (not drawingCards.empty ())
//         {
//           game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, drawingCards.front ());
//         }
//       else if (not losingCards.empty ())
//         {
//           game.playerDefends (game.cardsNotBeatenOnTableWithIndex ().front ().second, losingCards.front ());
//         }
//       else
//         {
//           throw "STRANGE STATE";
//         }
//       return playCardAttack (game);
//     }
//   else if (ranges::find (defendingPlayerAllowedMoves, Move::takeCards) != defendingPlayerAllowedMoves.end ())
//     {
//       game.nextRound (true);
//       return playCardAttack (game);
//     }
//   throw "NO POSIBLE MOVE BUT GAME IS NOT OVER";
//   return game;
// }

// TEST_CASE ("callculate winner all cards knowen beating to the end player 2 should win", "[game]")
// {
//   auto gameState = GameState{};
//   gameState.cardDeck = std::vector<Card>{}; // X
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ Card{ 1, Type::diamonds }, Card{ 2, Type::diamonds } }); // X
//   gameState.players.push_back (player2);
//   gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
//   gameState.trump = Type::hearts;
//   gameState.attackStarted = false;
//   gameState.gameOver = false;
//   gameState.round = defaultRoundToStart;
//   gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
//   auto game = Game{ gameState };
//   REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
//   REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
//   auto result = playCardAttack (game);
//   REQUIRE (result.checkIfGameIsOver ());
//   REQUIRE (result.durak ().has_value ());
//   REQUIRE (result.durak ().value ().id == "Player2");
// }

// TEST_CASE ("callculate winner all cards knowen beating to the end player 1 should win", "[game]")
// {
//   auto gameState = GameState{};
//   gameState.cardDeck = std::vector<Card>{};
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ Card{ 1, Type::diamonds }, Card{ 2, Type::diamonds } });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
//   gameState.players.push_back (player2);
//   gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
//   gameState.trump = Type::hearts;
//   gameState.attackStarted = false;
//   gameState.gameOver = false;
//   gameState.round = defaultRoundToStart;
//   gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
//   auto game = Game{ gameState };
//   REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
//   REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
//   auto result = playCardAttack (game);
//   REQUIRE (result.checkIfGameIsOver ());
//   REQUIRE (result.durak ().has_value ());
//   REQUIRE (result.durak ().value ().id == "Player1");
// }

// TEST_CASE ("callculate winner all cards knowen beating to the end draw", "[game]")
// {
//   auto gameState = GameState{};
//   gameState.cardDeck = std::vector<Card>{};
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ Card{ 1, Type::diamonds }, Card{ 1, Type::spades } });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ Card{ 1, Type::hearts }, Card{ 2, Type::hearts } });
//   gameState.players.push_back (player2);
//   gameState.table = std::vector<std::pair<Card, boost::optional<Card> > >{};
//   gameState.trump = Type::hearts;
//   gameState.attackStarted = false;
//   gameState.gameOver = false;
//   gameState.round = defaultRoundToStart;
//   gameState.numberOfCardsPlayerShouldHave = defaultNumberOfCardsPlayerShouldHave;
//   auto game = Game{ gameState };
//   REQUIRE (game.getAttackingPlayer ().value ().getCards ().size () == 2);
//   REQUIRE (game.getDefendingPlayer ().value ().getCards ().size () == 2);
//   auto result = playCardAttack (game);
//   REQUIRE (result.checkIfGameIsOver ());
//   REQUIRE (result.durak ().has_value () == false);
// }

// GameState
// cards12 ()
// {
//   auto cardDeck = testCardDeck12 ();
//   auto gameState = GameState{};
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ cardDeck.end () - 12, cardDeck.end () - 6 });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ cardDeck.end () - 6, cardDeck.end () });
//   gameState.players.push_back (player2);
//   gameState.trump = Type::hearts;
//   return gameState;
// }

// // TEST_CASE ("12 cards game", "[game]")
// // {
// //   // TODO improve performance
// //   auto game = Game{ cards12 () };
// //   auto result = playCardAttack (game);
// //   REQUIRE (result.checkIfGameIsOver ());
// //   REQUIRE_FALSE (result.durak ().has_value ());
// //   REQUIRE (result.getRound () == 3);
// //   auto history = result.getHistory ();
// //   for (auto historyElement : history)
// //     {
// //       std::cout << historyElement << std::endl;
// //     }
// // }

// GameState
// randomCards (u_int16_t maxValue, u_int16_t typeCount)
// {
//   auto cardDeck = generateCardDeck (maxValue, typeCount);
//   auto gameState = GameState{};
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ cardDeck.begin (), cardDeck.end () - (cardDeck.size () / 2) });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ cardDeck.end () - (cardDeck.size () / 2), cardDeck.end () });
//   gameState.players.push_back (player2);
//   gameState.trump = Type::hearts;
//   return gameState;
// }

// GameState
// splitCardsBetweenPlayers (std::vector<Card> const &cardDeck)
// {
//   auto gameState = GameState{};
//   auto player1 = Player{};
//   player1.id = "Player1";
//   player1.takeCards ({ cardDeck.begin (), cardDeck.end () - (cardDeck.size () / 2) });
//   gameState.players.push_back (player1);
//   auto player2 = Player{};
//   player2.id = "Player2";
//   player2.takeCards ({ cardDeck.end () - (cardDeck.size () / 2), cardDeck.end () });
//   gameState.players.push_back (player2);
//   gameState.trump = Type::hearts;
//   return gameState;
// }

// bool
// containsNumber (std::vector<u_int16_t> const &x, std::vector<u_int16_t> const &y)
// {
//   return true;
// }

// std::vector<std::vector<Card> >
// cardDecksToPlay (size_t selectedCardsSize, size_t differentValues)
// {
//   auto result = std::vector<std::vector<Card> >{};
//   std::vector<Card> cardDeck{};
//   for (u_int16_t cardValue = 1; cardValue <= (differentValues / 4); cardValue++)
//     {
//       for (u_int16_t type = 0; type < 4; type++)
//         {
//           cardDeck.push_back (Card{ .value = cardValue, .type = static_cast<Type> (type) });
//         }
//     }
//   for (u_int16_t n = 1; n <= differentValues; n++)
//     {
//       for (u_int16_t m = 1; m <= differentValues; m++)
//         {
//           if (n < m)
//             {
//               for (u_int16_t i = 1; i <= differentValues; i++)
//                 {
//                   if (i != m and i != n)
//                     {
//                       for (u_int16_t j = 1; j <= differentValues; j++)
//                         {
//                           if (j != m and j != n and i < j)
//                             {
//                               std::cout << n << "," << m << "," << i << "," << j << std::endl;
//                               result.push_back ({ cardDeck.at (n - 1), cardDeck.at (m - 1), cardDeck.at (i - 1), cardDeck.at (j - 1) });
//                               //     result.push_back ({ cardDeck.at (i - 1), cardDeck.at (j - 1), cardDeck.at (n - 1), cardDeck.at (m - 1) });
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }
//   return result;
// }

// TEST_CASE ("4 cards from 8 play all possible games", "[abc]")
// {
//   for (auto const &cardDeck : cardDecksToPlay (4, 8))
//     {
//       auto game = Game{ splitCardsBetweenPlayers (cardDeck) };
//       auto result = playCardAttack (game);
//       REQUIRE (result.checkIfGameIsOver ());
//     }
//   // TODO tabelle  mit zwei zahlen machen von 1 bis 8 dann filtern und abkuerzen
// }

template <typename Bidi>
constexpr bool
next_permutation_n (Bidi begin, Bidi end, size_t permutationStep)
{
  auto result = true;
  for (auto n = permutationStep; n > 1; n--)
    {
      std::next_permutation (begin, end);
    }
  return std::next_permutation (begin, end);
}

template <typename Bidi, typename Functor>
constexpr void
for_each_permuted_combination (Bidi begin, Bidi middle, Bidi end, Functor func)
{
  do
    {
      func (begin, middle);
      std::reverse (middle, end);
    }
  while (std::next_permutation (begin, end));
}

constexpr boost::multiprecision::uint128_t
factorial (size_t n)
{
  unsigned i = 1;
  boost::multiprecision::uint128_t factorial = 1;
  while (i < n)
    {
      ++i;
      factorial *= i;
    }
  return factorial;
}

size_t constexpr
combintions (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize / 2)) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

size_t constexpr
combintions2 (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize)) * factorial (subsetSize / 2) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

template <size_t setOfNumbersSize, size_t subsetSize>
std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> constexpr
subset (std::array<uint8_t, setOfNumbersSize> setOfNumbers)
{
  if (subsetSize % 2 != 0) return {};
  std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets;
  for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize / 2, setOfNumbers.end (), [&subsets, elementCount = 0U] (auto begin, auto end) mutable {
    if (std::is_sorted (begin, end))
      {
        for (auto i = 0U; begin != end; i++, begin++)
          {
            subsets.at (elementCount).at (i) = *begin;
          }
        elementCount++;
      }
  });
  return subsets;
}

template <size_t setOfNumbersSize, size_t subsetSize>
constexpr std::array<std::array<uint8_t, subsetSize>, combintions2 (setOfNumbersSize, subsetSize)>
permutations (std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets)
{
  std::array<std::array<uint8_t, subsetSize>, combintions2 (setOfNumbersSize, subsetSize)> results;
  auto resultCount = 0UL;
  auto itr = 0UL;
  for (auto subset : subsets)
    {
      auto const numbersNotInArray = [&subset] (auto const &array) {
        for (auto number : subset)
          if (std::binary_search (array.begin (), array.end (), number)) return false;
        return true;
      };
      auto resultItr = std::find_if (subsets.begin (), subsets.end (), numbersNotInArray);
      auto i = 0UL;
      while (resultItr != subsets.end ())
        {
          ranges::copy (subset, results.at (resultCount).begin ());
          ranges::copy (*resultItr, results.at (resultCount).begin () + subsetSize / 2);
          resultCount++;
          resultItr = std::find_if (resultItr + 1, subsets.end (), numbersNotInArray);
          i++;
          if (i >= (combintions2 (setOfNumbersSize, subsetSize) / subsets.size ())) break;
        }
      itr++;
      if (itr >= (subsets.size () / 2)) break;
    }
  std::transform (results.begin (), results.begin () + results.size () / 2, results.begin () + results.size () / 2, [] (auto element) {
    std::rotate (element.begin (), element.begin () + element.size () / 2, element.end ());
    return element;
  });
  return results;
}

// TEST_CASE ("subset", "[abc]")
// {
//   // std::cout << "subset.size(): " << subset<10, 8> ({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }).size ();
// }

TEST_CASE ("subset permutationCombinations BENCHMARK", "[abc]")
{
  // TODO try out std::execution::par_unseq again
  auto constexpr setOfNumbersSize = 20;
  auto constexpr subsetSize = 12;
  auto result = subset<setOfNumbersSize, subsetSize> ({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 });
  permutations<setOfNumbersSize, subsetSize> (result);
  // BENCHMARK ("permutations") { return permutations<setOfNumbersSize, subsetSize> (result); };
}

// TEST_CASE ("permutationCombinations", "[abc]") { permutationCombinations<8, 6> (subset<8, 6> ({ 0, 1, 2, 3, 4, 5, 6, 7 })); }

// TEST_CASE ("permutations", "[abc]") { permutations<8, 6> (subset<8, 6> ({ 0, 1, 2, 3, 4, 5, 6, 7 })); }

// TEST_CASE ("permutationCombinationsBreakWhenAll", "[abc]")
// {
//   //
//   constexpr auto result = permutationCombinationsBreakWhenAllCombintationsFoundAndSearchHalfAndMirror<8, 6> (subset<8, 6> ({ 0, 1, 2, 3, 4, 5, 6, 7 }));
//   std::array<uint8_t, result.size ()> someTest;
//   std::cout << someTest.size () << std::endl;
//   // auto test = 42;
// }

// TEST_CASE ("permutation n steps", "[abc]")
// {
//   // std::array<uint8_t, 6> setOfNumbers{ 1, 2, 3, 4, 5, 6 };
//   // next_permutation_n (setOfNumbers.begin (), setOfNumbers.end (), 2);
//   // std::cout << setOfNumbers.at (0) << " " << setOfNumbers.at (1) << " " << setOfNumbers.at (2) << " " << setOfNumbers.at (3) << " " << setOfNumbers.at (4) << " " << setOfNumbers.at (5) << std::endl;

//   //
//   // std::array<uint8_t, 4> setOfNumbers{ 1, 2, 3, 4 };
//   // auto const result = [] (auto begin, auto end) {
//   //   //
//   //   // std::cout << int{ *begin } << "," << int{ *(begin + 1) } << std::endl;
//   // };
//   // };
//   // for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + 2, setOfNumbers.end (), result);
// }

// TEST_CASE ("subset benchmark", "[abc]")
// {
//   // BENCHMARK ("subset") { return subset<10, 8> ({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }); };
// }

// TEST_CASE ("subset permutations benchmark", "[abc]")
// {
//   //  BENCHMARK ("subset permutations") { return permutations<20, 10> (subset<20, 10> ({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 })); };
// }
