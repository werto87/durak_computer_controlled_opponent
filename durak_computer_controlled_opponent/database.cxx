#include "database.hxx"
#include "compressCard.hxx"
#include "serialisation.hxx"
#include "solve.hxx"
#include "util.hxx"
#include <charconv>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <durak/card.hxx>
#include <filesystem>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <soci/error.h>
#include <soci/session.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <sqlite3.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace durak_computer_controlled_opponent::database
{
void
deleteDatabaseAndCreateNewDatabase (std::filesystem::path const &databasePath)
{
  std::filesystem::remove (databasePath);
  std::filesystem::create_directories (databasePath.parent_path ());
  using namespace sqlite_api;
  sqlite3 *db{};
  int rc{};
  rc = sqlite3_open (databasePath.c_str (), &db);
  if (rc)
    {
      fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
      return;
    }
  sqlite3_close (db);
}

void
createDatabaseIfNotExist (std::filesystem::path const &databasePath)
{
  using namespace sqlite_api;
  if (not std::filesystem::exists (databasePath))
    {
      std::filesystem::create_directories (databasePath.parent_path ());
      sqlite3 *db{};
      int rc{};
      rc = sqlite3_open (databasePath.c_str (), &db);
      if (rc)
        {
          fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
          return;
        }
      sqlite3_close (db);
    }
}

void
createTables (std::filesystem::path const &databasePath)
{
  soci::session sql (soci::sqlite3, databasePath);
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

small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >
binaryToSmallMemoryTree (std::string movesAndResultAsBinary)
{
  auto ss = std::stringstream{ movesAndResultAsBinary };
  auto archive = cereal::BinaryInputArchiveWithContainingVectorSize{ ss };
  auto smallMemoryTree = small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >{};
  archive (smallMemoryTree);
  return smallMemoryTree;
}
std::string
smallMemoryTreeToBinary (small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > const &smallMemoryTree)
{
  auto ss = std::stringstream{};
  auto archive = cereal::BinaryOutputArchiveWithContainingVectorSize{ ss };
  archive (smallMemoryTree);
  return ss.str ();
}

void
insertGameLookUp (std::filesystem::path const &databasePath, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> > const &gameLookup)
{
  soci::session sql (soci::sqlite3, databasePath.c_str ());
  soci::transaction tr (sql);
  for (auto const &gameTypeAndGame : gameLookup)
    {
      using namespace durak;
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          for (auto const &resultForTrump : std::get<1> (gameTypeAndGame).at (static_cast<size_t> (trumpType)))
            {
              auto const &[cards, combination] = resultForTrump;
              auto round = database::Round{};
              round.gameState = database::gameStateAsString (cards, trumpType);
              round.nodes = smallMemoryTreeToBinary (combination);
              confu_soci::insertStruct (sql, round);
            }
        }
    }
  tr.commit ();
}

}