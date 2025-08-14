#include "durak_computer_controlled_opponent/database.hxx"

void
insertVector (auto &sql, auto const &vectorToInsert, auto const &combinationAndGameTrees)
{
  for (auto const &combinationAndTreeToInsert : vectorToInsert)
    {
      if (auto combinationAndTreeToInsertItr = combinationAndGameTrees.find (combinationAndTreeToInsert); combinationAndTreeToInsertItr != combinationAndGameTrees.end ())
        {
          durak_computer_controlled_opponent::database::insertCombination (sql, *combinationAndTreeToInsertItr);
        }
    }
}

void
insertGameLookupBackwardsAndPartitionedByWinnersDiamondIsTrump (auto &sql, auto const &gameLookup)
{
  using namespace durak_computer_controlled_opponent;
  for (auto const &[unused, combinationAndTreesToInsertAndTrump] : gameLookup | std::views::reverse)
    {
      auto const &combinationAndGameTrees = combinationAndTreesToInsertAndTrump.at (static_cast<uint8_t> (durak::Type::diamonds));
      auto winningCombinations = std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > >{};
      auto otherCombinations = std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > >{};
      for (auto const &[combination, gameTree] : combinationAndGameTrees)
        {
          auto const &getSecond = [] (const auto &t) { return std::get<1> (t); };
          if (std::ranges::contains (nextActionsAndResults ({}, gameTree) | std::views::transform (getSecond), Result::AttackWon))
            {
              winningCombinations.push_back (combination);
            }
          else
            {
              otherCombinations.push_back (combination);
            }
        }
      std::mt19937 rng{ 123456u };                     // always shuffle the same way so it is deterministic
      std::ranges::shuffle (winningCombinations, rng); // shuffle winning combinations so combinations look more different while solving one after the other
      insertVector (sql, winningCombinations, combinationAndGameTrees);
      insertVector (sql, otherCombinations, combinationAndGameTrees);
    }
}

int
main ()
{
  using namespace durak_computer_controlled_opponent;
  auto const databasePath = CURRENT_BINARY_DIR / std::filesystem::path{ "diamond_combinations.db" };
  std::filesystem::remove (databasePath);
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  std::cout << "create new game lookup table" << std::endl;
  std::cout << "solveDurak (36, 1, 1, gameLookup)" << std::endl;
  gameLookup.insert ({ { uint8_t{ 1 }, uint8_t{ 1 } }, solveDurak (36, 1, 1, gameLookup, { durak::Type::diamonds }) });
  std::cout << "solveDurak (36, 2, 2, gameLookup)" << std::endl;
  gameLookup.insert ({ { uint8_t{ 2 }, uint8_t{ 2 } }, solveDurak (36, 2, 2, gameLookup, { durak::Type::diamonds }) });
  std::cout << "solveDurak (36, 3, 1, gameLookup)" << std::endl;
  gameLookup.insert ({ { uint8_t{ 3 }, uint8_t{ 1 } }, solveDurak (36, 3, 1, gameLookup, { durak::Type::diamonds }) });
  std::cout << "solveDurak (36, 2, 4, gameLookup)" << std::endl;
  gameLookup.insert ({ { uint8_t{ 2 }, uint8_t{ 4 } }, solveDurak (36, 2, 4, gameLookup, { durak::Type::diamonds }) });
  std::cout << "solveDurak (36, 3, 3, gameLookup)" << std::endl;
  gameLookup.insert ({ { uint8_t{ 3 }, uint8_t{ 3 } }, solveDurak (36, 3, 3, gameLookup, { durak::Type::diamonds }) });
  std::cout << "insert lookup table into database" << std::endl;
  database::createDatabaseIfNotExist (databasePath);
  database::createTables (databasePath);
  soci::session sql (soci::sqlite3, databasePath.string ());
  soci::transaction tr (sql);
  insertGameLookupBackwardsAndPartitionedByWinnersDiamondIsTrump (sql, gameLookup);
  tr.commit ();
  std::cout << "finished creating game lookup table " << std::endl;
  return 0;
}