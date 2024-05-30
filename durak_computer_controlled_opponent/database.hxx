#ifndef D179262B_4D2C_43C0_ACA1_5464B5C2533B
#define D179262B_4D2C_43C0_ACA1_5464B5C2533B

#include "confu_soci/convenienceFunctionForSoci.hxx"
#include "solve.hxx"
#include <durak/card.hxx>
#include <filesystem>
#include <small_memory_tree/smallMemoryTree.hxx>

BOOST_FUSION_DEFINE_STRUCT ((durak_computer_controlled_opponent::database), Round, (std::string, gameState) (uint64_t, maxChildren) (std::string, hierarchy) (std::string, data))

namespace durak_computer_controlled_opponent::database
{

void deleteDatabaseAndCreateNewDatabase (std::filesystem::path const &databasePath);

void createDatabaseIfNotExist (std::filesystem::path const &databasePath);

void createTables (std::filesystem::path const &databasePath);

std::string gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &cards, durak::Type trump);

void insertGameLookUp (std::filesystem::path const &databasePath, std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> > const &gameLookup);

small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > binaryToSmallMemoryTreeData (Round const &round);

}

#endif /* D179262B_4D2C_43C0_ACA1_5464B5C2533B */