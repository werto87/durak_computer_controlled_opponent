#ifndef D179262B_4D2C_43C0_ACA1_5464B5C2533B
#define D179262B_4D2C_43C0_ACA1_5464B5C2533B

#include "confu_soci/convenienceFunctionForSoci.hxx"
#include <durak/card.hxx>

BOOST_FUSION_DEFINE_STRUCT ((database), Round, (std::string, gameState) (std::vector<uint8_t>, combination))

namespace database
{
std::string const databaseName = "database/combination.db";

void createEmptyDatabase ();

void createDatabaseIfNotExist ();

void createTables ();

std::string gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &cards, durak::Type trump);

}

#endif /* D179262B_4D2C_43C0_ACA1_5464B5C2533B */
