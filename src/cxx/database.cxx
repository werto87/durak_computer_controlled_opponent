#include "database.hxx"
#include "src/cxx/util.hxx"
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

namespace database
{
void
createEmptyDatabase ()
{
  std::filesystem::remove (databaseName.c_str ());
  if (not std::filesystem::exists ("database"))
    {
      std::filesystem::create_directory ("database");
    }
  using namespace sqlite_api;
  sqlite3 *db{};
  int rc{};
  rc = sqlite3_open (databaseName.c_str (), &db);
  if (rc)
    {
      fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
      return;
    }
  sqlite3_close (db);
}

void
createDatabaseIfNotExist ()
{
  using namespace sqlite_api;
  if (not std::filesystem::exists ("database"))
    {
      std::filesystem::create_directory ("database");
    }
  sqlite3 *db{};
  int rc{};
  rc = sqlite3_open (databaseName.c_str (), &db);
  if (rc)
    {
      fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
      return;
    }
  sqlite3_close (db);
}

void
createTables ()
{
  soci::session sql (soci::sqlite3, databaseName);
  try
    {
      confu_soci::createTableForStruct<Round> (sql);
    }
  catch (soci::soci_error const &error)
    {
      std::cout << error.get_error_message () << std::endl;
    }
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
}