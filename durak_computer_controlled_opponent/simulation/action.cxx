#include "durak_computer_controlled_opponent/simulation/action.hxx"
#include "durak_computer_controlled_opponent/simulation/compressCard.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
namespace durak_computer_controlled_opponent
{
std::optional<durak::Card>
Action::playedCard () const
{
  if (_value >= 253)
    {
      return std::nullopt;
    }
  else
    {
      return idToCard (_value);
    }
}

std::uint8_t
Action::value () const
{
  return _value;
}
Action::Category
Action::operator() () const
{
  if (_value > 253)
    {
      return Category::Undefined;
    }
  else if (_value == 253)
    {
      return Category::PassOrTakeCard;
    }
  else
    {
      return Category::PlayCard;
    }
}
}