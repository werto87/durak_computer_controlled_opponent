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

void
dumpNodeWithActionResult (stlplus::dump_context &context, small_memory_tree::Node<std::tuple<Action, Result> > const &node)
{
  stlplus::dump_unsigned_char (context, std::get<0> (node.value).value ());
  stlplus::dump_unsigned_char (context, static_cast<uint8_t> (std::get<1> (node.value)));
  stlplus::dump_unsigned_long (context, static_cast<unsigned long> (node.childrenOffsetEnd));
}

void
restoreNode (stlplus::restore_context &context, small_memory_tree::Node<std::tuple<Action, Result> > &node)
{
  auto getValue = uint8_t{};
  stlplus::restore_unsigned_char (context, getValue);
  std::get<0> (node.value) = Action{ getValue };
  stlplus::restore_unsigned_char (context, getValue);
  std::get<1> (node.value) = Result{ getValue };
  stlplus::restore_unsigned_long (context, node.childrenOffsetEnd);
}

small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> >
binaryToSmallMemoryTree (std::string movesAndResultAsBinary)
{
  auto restoreStringStream = std::stringstream{ std::move (movesAndResultAsBinary) };
  auto restoreContext = stlplus::restore_context{ restoreStringStream };
  auto restoredVec = std::vector<small_memory_tree::Node<std::tuple<Action, Result> > >{};
  stlplus::restore_vector (restoreContext, restoredVec, restoreNode);
  return { restoredVec };
}
std::string
smallMemoryTreeToBinary (small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > const &smallMemoryTree)
{
  auto output = std::stringstream (std::ios_base::out | std::ios_base::binary);
  auto dumpContext = stlplus::dump_context{ output };
  stlplus::dump_vector (dumpContext, smallMemoryTree.getNodes (), dumpNodeWithActionResult);
  return output.str ();
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
              round.nodes = smallMemoryTreeToBinary (combination.getNodes ());
              confu_soci::insertStruct (sql, round);
            }
        }
    }
  tr.commit ();
}

}