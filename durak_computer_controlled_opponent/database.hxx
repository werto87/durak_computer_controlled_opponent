#ifndef D179262B_4D2C_43C0_ACA1_5464B5C2533B
#define D179262B_4D2C_43C0_ACA1_5464B5C2533B

#include "confu_soci/convenienceFunctionForSoci.hxx"
#include "solve.hxx"
#include <durak/card.hxx>
#include <filesystem>


namespace durak_computer_controlled_opponent::database
{

void createEmptyDatabase (std::filesystem::path const& databasePath);

void createDatabaseIfNotExist (std::filesystem::path const& databasePath);

void createTables (std::filesystem::path const& databasePath);

std::string gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &cards, durak::Type trump);

std::vector<uint8_t> moveResultToBinary (std::vector<std::tuple<uint8_t, durak_computer_controlled_opponent::Result> > const &moveResults);

void insertGameLookUp (std::filesystem::path const& databasePath,std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, durak_computer_controlled_opponent::Result> > >, 4> > const &gameLookup);

}

#endif /* D179262B_4D2C_43C0_ACA1_5464B5C2533B */