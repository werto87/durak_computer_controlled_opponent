#include "src/cxx/database.hxx"
#include "src/cxx/solve.hxx"
#include "src/cxx/util.hxx"
#include <catch2/catch.hpp>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <durak/card.hxx>
#include <magic_enum.hpp>
#include <vector>

TEST_CASE ("database setup", "[abc]") { REQUIRE (database::gameStateAsString ({ { '1', '2', '3', 'a' }, { 'c', 'b' } }, durak::Type::hearts) == "1,2,3,a;c,b;0"); }

std::vector<uint8_t>
moveResultToBinary (std::vector<std::tuple<uint8_t, Result> > const &moveResults)
{
  auto results = std::vector<uint8_t>{};
  results.reserve (moveResults.size () * 2);
  for (auto moveResult : moveResults)
    {
      auto [move, result] = moveResult;
      results.push_back (move);
      results.push_back (magic_enum::enum_integer (result));
    }
  return results;
}

TEST_CASE ("save solve result to database", "[abc]")
{
  database::createEmptyDatabase ();
  database::createTables ();
  auto results = solveDurak (36, 1, 1, {});
  using namespace durak;
  soci::session sql (soci::sqlite3, database::databaseName);
  for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
    {
      for (auto const &resultForTrump : results.at (static_cast<size_t> (trumpType)))
        {
          auto const &[cards, combination] = resultForTrump;
          auto round = database::Round{};
          round.gameState = database::gameStateAsString (cards, trumpType);
          round.combination = moveResultToBinary (combination);
          confu_soci::insertStruct (sql, round);
        }
    }
  auto someRound = confu_soci::findStruct<database::Round> (sql, "gameState", database::gameStateAsString ({ { 0 }, { 1 } }, Type::clubs));
  REQUIRE (someRound.has_value ());
}
