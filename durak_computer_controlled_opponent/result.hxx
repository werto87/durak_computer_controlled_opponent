#pragma once
#include <cstdint>
namespace durak_computer_controlled_opponent
{
enum class Result : uint8_t
{
  Undefined,
  DefendWon,
  Draw,
  AttackWon
};
}