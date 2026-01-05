#pragma once
#include <optional>
#include <cstdint>
#include <durak/card.hxx>
namespace durak_computer_controlled_opponent
{
class Action
{
public:
  enum struct Category
  {
    Undefined,
    PlayCard,
    PassOrTakeCard
  };

  Action () = default;
  explicit Action (std::uint8_t value) : _value (value) {}
  // clang-format off
  auto operator<=> (const Action &) const = default;
  // clang-format on
  [[nodiscard]] Category operator() () const;
  [[nodiscard]] std::optional<durak::Card> playedCard () const;

  [[nodiscard]] std::uint8_t value () const;

private:
  std::uint8_t _value{ 253 };
};
}