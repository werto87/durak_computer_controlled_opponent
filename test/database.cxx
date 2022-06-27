#include "src/cxx/database.hxx"
#include "src/cxx/compressCard.hxx"
#include "src/cxx/solve.hxx"
#include "src/cxx/util.hxx"
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

std::vector<durak::Card>
stringToCards (std::string const &cardsAsString)
{
  auto result = std::vector<durak::Card>{};
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, cardsAsString, boost::is_any_of (","));
  for (auto const &value : splitMesssage)
    {
      uint8_t tmp{};
      auto [ptr, ec]{ std::from_chars (value.data (), value.data () + value.size (), tmp) };
      if (ec == std::errc ())
        {
          result.push_back (idToCard (tmp));
        }
      else if (ec == std::errc::invalid_argument)
        {
          throw std::logic_error{ "That isn't a number" };
        }
      else if (ec == std::errc::result_out_of_range)
        {
          throw std::logic_error{ "This number is larger than an uint8_t" };
        }
    }
  return result;
}

std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>
attackAndDefendCardsAndTrump (std::string const &gameState)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>{};
  auto &[attack, defend, trump] = result;
  std::vector<std::string> splitMesssage{};
  boost::algorithm::split (splitMesssage, gameState, boost::is_any_of (";"));
  if (splitMesssage.size () == 3)
    {
      attack = stringToCards (splitMesssage.at (0));
      defend = stringToCards (splitMesssage.at (1));
      trump = magic_enum::enum_cast<durak::Type> (std::stoi (splitMesssage.at (2))).value ();
    }
  else
    {
      throw std::logic_error{ "gameState wrong format should only have 2 times ; gameStateAsString: " + gameState };
    }
  return result;
}

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
