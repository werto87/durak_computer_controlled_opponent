#pragma once
#include <durak/card.hxx>

namespace durak_computer_controlled_opponent
{
enum struct Move
{
  PlayCard,
  PassOrTakeCard
};

struct MoveToPlay
{
  Move move{};
  std::optional<durak::Card> card{};
};
}