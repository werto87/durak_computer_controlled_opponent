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

namespace durak_computer_controlled_opponent::guess
{
enum struct NextMoveToPlayForRoleError
{
  couldNotFindAMoveToPlay
};

std::expected<MoveToPlay, NextMoveToPlayForRoleError> nextMoveToPlayForRole (durak::Game const &game, durak::PlayerRole playerRole);

}
