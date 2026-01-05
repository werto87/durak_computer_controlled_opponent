#pragma once

#include "durak_computer_controlled_opponent/result.hxx"
#include <expected>
#include <filesystem>
#include <vector>

namespace durak
{
class Game;
enum struct PlayerRole;
}
namespace durak_computer_controlled_opponent
{
class Action;

namespace guess
{
enum struct NextActionForRoleError
{
  databaseDoesNotExist,
  databaseMissingTable,
  gameNotInLookupTable
};

std::expected<Action, NextActionForRoleError> nextActionForRole (std::filesystem::path const &databasePath, durak::Game const &game, durak::PlayerRole playerRole);

}
}