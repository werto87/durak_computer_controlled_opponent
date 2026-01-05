#include "durak_computer_controlled_opponent/util.hxx"
#include <boost/numeric/conversion/cast.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace durak_computer_controlled_opponent
{

durak::Card
idToCard (uint8_t id)
{
  return durak::Card{ boost::numeric_cast<uint16_t> (id / 4), static_cast<durak::Type> (id % 4) };
}

uint8_t
cardToId (durak::Card const &card)
{
  return boost::numeric_cast<uint8_t> (card.value * 4 + static_cast<uint8_t> (card.type));
}

std::vector<uint8_t>
cardsToIds (std::vector<durak::Card> const &cards)
{
  auto results = std::vector<uint8_t>{};
  std::ranges::transform (cards, std::back_inserter (results), [] (durak::Card const &cards_) { return cardToId (cards_); });
  return results;
}

std::vector<durak::Card>
idsToCards (std::vector<uint8_t> const &ids)
{
  auto results = std::vector<durak::Card>{};
  std::ranges::transform (ids, std::back_inserter (results), [] (uint8_t id) { return idToCard (id); });
  return results;
}

}
