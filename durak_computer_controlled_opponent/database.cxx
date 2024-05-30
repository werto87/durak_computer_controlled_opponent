#include "database.hxx"
#include "compressCard.hxx"
#include "solve.hxx" // for vector
#include "util.hxx"
#include <charconv>
#include <confu_soci/convenienceFunctionForSoci.hxx>
#include <cstddef>
#include <durak/card.hxx>
#include <filesystem> // for create_directory
#include <iostream>   // for endl, basic_ost...
#include <magic_enum/magic_enum.hpp>
#include <soci/error.h>                // for soci_error
#include <soci/session.h>              // for session
#include <soci/sqlite3/soci-sqlite3.h> // for sqlite3, sqlite...
#include <sqlite3.h>                   // for sqlite3_close
#include <stdio.h>                     // for fprintf, stderr
#include <stlplus/persistence/persistent_contexts.hpp>
#include <stlplus/persistence/persistent_pair.hpp>
#include <stlplus/persistence/persistent_vector.hpp>
#include <string>
#include <vector> // for vector

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

std::string
smallMemoryTreeDataHierarchyToBinary (std::vector<bool> const &hierarchy)
{
  auto output = std::stringstream (std::ios_base::out | std::ios_base::binary);
  auto dumpContext = stlplus::dump_context{ output };
  stlplus::dump_vector_bool (dumpContext, hierarchy);
  return output.str ();
}

std::vector<bool>
binaryToSmallMemoryTreeDataHierarchy (std::string hierarchyBinary)
{
  auto restoreStringStream = std::stringstream{ std::move (hierarchyBinary) };
  auto restoreContext = stlplus::restore_context{ restoreStringStream };
  auto result = std::vector<bool>{};
  restore_vector_bool (restoreContext, result);
  return result;
}

void
dumpTreeDataTupleActionResult (stlplus::dump_context &context, const std::tuple<Action, Result> &data)
{
  stlplus::dump_unsigned_char (context, std::get<0> (data).value ());
  stlplus::dump_unsigned_char (context, static_cast<uint8_t> (std::get<1> (data)));
}

void
restoreTupleActionResult (stlplus::restore_context &context, std::tuple<Action, Result> &data)
{
  auto getValue = uint8_t{};
  stlplus::restore_unsigned_char (context, getValue);
  std::get<0> (data) = Action{ getValue };
  stlplus::restore_unsigned_char (context, getValue);
  std::get<1> (data) = Result{ getValue };
}

std::vector<std::tuple<Action, Result> >
binaryToSmallMemoryTreeDataData (std::string movesAndResultAsBinary)
{
  auto restoreStringStream = std::stringstream{ std::move (movesAndResultAsBinary) };
  auto restoreContext = stlplus::restore_context{ restoreStringStream };
  auto restoredVec = std::vector<std::tuple<Action, Result> >{};
  stlplus::restore_vector (restoreContext, restoredVec, restoreTupleActionResult);
  return restoredVec;
}
std::string
smallMemoryTreeDataDataToBinary (std::vector<std::tuple<Action, Result> > const &moveResults)
{
  auto output = std::stringstream (std::ios_base::out | std::ios_base::binary);
  auto dumpContext = stlplus::dump_context{ output };
  stlplus::dump_vector (dumpContext, moveResults, dumpTreeDataTupleActionResult);
  return output.str ();
}

void
insertGameLookUp (std::filesystem::path const &databasePath, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> > const &gameLookup)
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
              round.maxChildren = combination.maxChildren;
              round.data = smallMemoryTreeDataDataToBinary (combination.data);
              round.hierarchy = smallMemoryTreeDataHierarchyToBinary (combination.hierarchy);
              confu_soci::insertStruct (sql, round);
            }
        }
    }
  tr.commit ();
}

small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> >
binaryToSmallMemoryTreeData (Round const &round)
{
  return { round.maxChildren, binaryToSmallMemoryTreeDataHierarchy (round.hierarchy), binaryToSmallMemoryTreeDataData (round.data) };
}

}