#ifndef D179262B_4D2C_43C0_ACA1_5464B5C2533B
#define D179262B_4D2C_43C0_ACA1_5464B5C2533B

#include "confu_soci/convenienceFunctionForSoci.hxx"

std::string const databaseName = "database/combination.db";

BOOST_FUSION_DEFINE_STRUCT ((database), Combination, (std::string, gameState) (std::vector<uint8_t>, combination))

namespace database
{
void createEmptyDatabase ();

void createDatabaseIfNotExist ();

void createTables ();

}

#endif /* D179262B_4D2C_43C0_ACA1_5464B5C2533B */
