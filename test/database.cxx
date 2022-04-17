#include "src/cxx/database.hxx"
#include "src/cxx/solve.hxx"
#include "src/cxx/util.hxx"
#include <catch2/catch.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
TEST_CASE ("save solve result to database", "[abc]")
{
  database::createEmptyDatabase ();
  database::createTables ();
  soci::session sql (soci::sqlite3, database::databaseName);
  auto round = database::Round{};
  round.gameState = gameStateAsString ({ { '1', '2' }, { '3', '4' } });
  confu_soci::insertStruct (sql, round);

  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
}
