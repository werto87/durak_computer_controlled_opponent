#pragma once

#include "durak_computer_controlled_opponent/moveToPlay.hxx"
#include "durak_computer_controlled_opponent/result.hxx"
#include <expected>
#include <filesystem>
#include <vector>

namespace durak
{
class Game;
enum struct PlayerRole;
}

namespace durak_computer_controlled_opponent::simulation_lookup
{
enum struct NextMoveToPlayForRoleError
{
  databaseDoesNotExist,
  databaseMissingTable,
  gameNotInLookupTable,
  noMoveToPlay
};

std::expected<MoveToPlay, NextMoveToPlayForRoleError> nextMoveForRole (std::filesystem::path const &databasePath, durak::Game const &game, durak::PlayerRole playerRole);
}
