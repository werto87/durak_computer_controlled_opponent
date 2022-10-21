#include "database.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <charconv>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <durak/card.hxx>
#include <filesystem> // for create_directory
#include <iostream>   // for endl, basic_ost...
#include <magic_enum.hpp>
#include <soci/error.h>                // for soci_error
#include <soci/session.h>              // for session
#include <soci/sqlite3/soci-sqlite3.h> // for sqlite3, sqlite...
#include <sqlite3.h>                   // for sqlite3_close
#include <stdio.h>                     // for fprintf, stderr
#include <string>
#include <vector> // for vector



namespace durak_computer_controlled_opponent::database
{
void
createEmptyDatabase (std::filesystem::path const& databasePath)
{
//  TODO rethink this user should be able to set a path for the database
//  std::filesystem::remove (databasePath);
//  if (not std::filesystem::exists ("database"))
//    {
//      std::filesystem::create_directory ("database");
//    }
//  using namespace sqlite_api;
//  sqlite3 *db{};
//  int rc{};
//  rc = sqlite3_open (databaseName.c_str (), &db);
//  if (rc)
//    {
//      fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
//      return;
//    }
//  sqlite3_close (db);
  std::cout<<"IMPLEMENT createEmptyDatabase"<<std::endl;
  std::abort();
}

void
createDatabaseIfNotExist (std::filesystem::path const& databasePath)
{
  //  TODO rethink this user should be able to set a path for the database
//  using namespace sqlite_api;
//  if (not std::filesystem::exists ("database"))
//    {
//      std::filesystem::create_directory ("database");
//    }
//  sqlite3 *db{};
//  int rc{};
//  rc = sqlite3_open (databaseName.c_str (), &db);
//  if (rc)
//    {
//      fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
//      return;
//    }
//  sqlite3_close (db);
  std::cout<<"IMPLEMENT createDatabaseIfNotExist"<<std::endl;
  std::abort();
}

void
createTables (std::filesystem::path const& databasePath)
{
  //  TODO rethink this user should be able to set a path for the database
//  soci::session sql (soci::sqlite3, databaseName);
//  try
//    {
//      confu_soci::createTableForStruct<Round> (sql);
//    }
//  catch (soci::soci_error const &error)
//    {
//      std::cout << error.get_error_message () << std::endl;
//    }
  std::cout<<"IMPLEMENT createTables"<<std::endl;
  std::abort();
}

std::string
gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &cards, durak::Type trump)
{
  return vectorToString (std::get<0> (cards)) + ";" + vectorToString (std::get<1> (cards)) + ";" + std::to_string (magic_enum::enum_integer (trump));
}


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

void
insertGameLookUp (std::filesystem::path const& databasePath,std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> > const &gameLookup)
{
//  soci::session sql (soci::sqlite3, database::databaseName);
//  soci::transaction tr (sql);
//  for (auto const &gameTypeAndGame : gameLookup)
//    {
//      using namespace durak;
//      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
//        {
//          for (auto const &resultForTrump : std::get<1> (gameTypeAndGame).at (static_cast<size_t> (trumpType)))
//            {
//              auto const &[cards, combination] = resultForTrump;
//              auto round = database::Round{};
//              round.gameState = database::gameStateAsString (cards, trumpType);
//              round.combination = database::moveResultToBinary (combination);
//              confu_soci::insertStruct (sql, round);
//            }
//        }
//    }
//  tr.commit ();
std::cout<<"IMPLEMENT insertGameLookUp"<<std::endl;
std::abort();
}

}