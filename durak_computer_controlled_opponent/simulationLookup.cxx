#include "durak_computer_controlled_opponent/simulationLookup.hxx"
#include "durak_computer_controlled_opponent/simulation/database.hxx"
#include "durak_computer_controlled_opponent/simulation/solve.hxx"
#include "durak_computer_controlled_opponent/simulation/util.hxx"
#include <confu_soci/convenienceFunctionForSoci.hxx>

namespace durak_computer_controlled_opponent::simulation_lookup
{
std::expected<MoveToPlay, NextMoveToPlayForRoleError>
nextMoveToPlayForRole (std::filesystem::path const &databasePath, durak::Game const &game, durak::PlayerRole playerRole)
{
  auto const [compressedCardsForAttack, compressedCardsForDefend, compressedCardsForAssist] = calcIdAndCompressedCardsForAttackAndDefend (game);
  auto attackCardsCompressed = std::vector<uint8_t>{};
  std::ranges::transform (compressedCardsForAttack, std::back_inserter (attackCardsCompressed), [] (auto const &idAndCard) { return std::get<0> (idAndCard); });
  auto defendCardsCompressed = std::vector<uint8_t>{};
  std::ranges::transform (compressedCardsForDefend, std::back_inserter (defendCardsCompressed), [] (auto const &idAndCard) { return std::get<0> (idAndCard); });
  if (not std::filesystem::exists (databasePath)) return std::unexpected (NextMoveToPlayForRoleError::databaseDoesNotExist);
  soci::session sql (soci::sqlite3, databasePath.string ());
  if (not confu_soci::doesTableExist<database::Round> (sql)) return std::unexpected (NextMoveToPlayForRoleError::databaseMissingTable);
  auto round = database::Round{};
  if (auto result = confu_soci::findStruct<database::Round> (sql, "gameState", database::gameStateAsString ({ attackCardsCompressed, defendCardsCompressed })))
    {
      round = result.value ();
    }
  else if (auto result = confu_soci::findStruct<database::Round> (sql, "gameState", database::gameStateAsString ({ attackCardsCompressed, defendCardsCompressed }, game.getTrump ())))
    {
      round = result.value ();
    }
  else
    {
      return std::unexpected (NextMoveToPlayForRoleError::gameNotInLookupTable);
    }
  auto const &actions = historyEventsToActionsCompressedCards (game.getHistory (), calcCardsAndCompressedCardsForAttackAndDefend (game));
  auto const &result = nextActionsAndResults (actions, small_memory_tree::SmallMemoryTree{ durak_computer_controlled_opponent::database::binaryToSmallMemoryTree (round.nodes) });
  if (auto const &actionOptional = durak_computer_controlled_opponent::nextActionForRole (result, playerRole))
    {
      auto action = actionOptional.value ();
      auto moveToPlay = MoveToPlay{};
      auto const &category = action ();
      switch (category)
        {
        case Action::Category::PlayCard:
          {
            moveToPlay.move = Move::PlayCard;
            moveToPlay.card = action.playedCard ().value ();
            break;
          }
        case Action::Category::PassOrTakeCard:
          {
            moveToPlay.move = Move::PassOrTakeCard;
            break;
          }
        }
      return moveToPlay;
    }
  else
    {
      if (auto const &moves = game.getAllowedMoves (playerRole); moves.size () == 1 and (std::ranges::contains (moves, durak::Move::pass) or std::ranges::contains (moves, durak::Move::takeCards)))
        {
          return MoveToPlay{ Move::PassOrTakeCard };
        }
      else
        {
          return std::unexpected (NextMoveToPlayForRoleError::noMoveToPlay);
        }
    }
}

}