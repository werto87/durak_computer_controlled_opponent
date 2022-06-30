#include "durak_computer_controlled_opponent/database.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/solve.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <catch2/catch.hpp>
#include <charconv>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstdint>
#include <date/date.h>
#include <durak/card.hxx>
#include <iomanip>
#include <magic_enum.hpp>
#include <stdexcept>
#include <string>
#include <vector>

TEST_CASE ("database setup", "[abc]") { REQUIRE (database::gameStateAsString ({ { 1, 2, 3, 6 }, { 4, 5 } }, durak::Type::hearts) == "1,2,3,6;4,5;0"); }

TEST_CASE ("save solve result to database", "[abc]")
{
  database::createEmptyDatabase ();
  database::createTables ();

  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  // gameLookup.insert ({ { 2, 2 }, solveDurak (36, 2, 2, gameLookup) });
  // gameLookup.insert ({ { 3, 1 }, solveDurak (36, 3, 1, gameLookup) });
  // gameLookup.insert ({ { 2, 4 }, solveDurak (36, 2, 4, gameLookup) });
  // gameLookup.insert ({ { 3, 3 }, solveDurak (36, 3, 3, gameLookup) });
  using namespace durak;
  using namespace date;
  soci::session sql (soci::sqlite3, database::databaseName);
  database::insertGameLookUp (gameLookup);
  auto someRound = confu_soci::findStruct<database::Round> (sql, "gameState", database::gameStateAsString ({ { 0 }, { 1 } }, Type::clubs));
  REQUIRE (someRound.has_value ());
  auto [attack, defend, trump] = attackAndDefendCardsAndTrump (someRound->gameState);
  REQUIRE (attack == idsToCards (std::vector<uint8_t>{ 0 }));
  REQUIRE (defend == idsToCards (std::vector<uint8_t>{ 1 }));
  REQUIRE (trump == Type::clubs);
}
