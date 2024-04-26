#include "compressCard.hxx"
#include <boost/numeric/conversion/cast.hpp>

namespace durak_computer_controlled_opponent
{

durak::Card
idToCard (uint8_t id)
{
  return durak::Card{ boost::numeric_cast<u_int16_t> (id / 4), static_cast<durak::Type> (id % 4) };
}

uint8_t
cardToId (durak::Card const &card)
{
  return boost::numeric_cast<uint8_t> (card.value * 4 + static_cast<uint8_t> (card.type));
}

std::vector<uint8_t>
cardsToIds (std::vector<durak::Card> const& cards)
{
  auto results = std::vector<uint8_t>{};
  std::ranges::transform (cards, std::back_inserter (results), [] (durak::Card const &cards_) { return cardToId (cards_); });
  return results;
}

std::vector<durak::Card>
idsToCards (std::vector<uint8_t> const& ids)
{
  auto results = std::vector<durak::Card>{};
  std::ranges::transform (ids, std::back_inserter (results), [] (uint8_t id) { return idToCard (id); });
  return results;
}

std::vector<durak::Card>
compress (std::vector<durak::Card> cards)
{
  auto idsAndCards = std::vector<std::tuple<size_t, durak::Card> >{};
  std::ranges::transform (cards, std::back_inserter (idsAndCards), [id = size_t{ 0 }] (durak::Card const &card) mutable { return std::tuple<size_t, durak::Card>{ id++, card }; });
  std::ranges::sort (idsAndCards, {}, [] (std::tuple<size_t, durak::Card> const &idAndCard) { return std::get<1> (idAndCard); });
  auto setToNumber = u_int16_t{ 0 };
  auto numberToChange = std::get<1> (idsAndCards.front ()).value;
  for (auto &[id, card] : idsAndCards)
    {
      if (card.value != numberToChange)
        {
          setToNumber++;
          numberToChange = card.value;
        }
      card.value = setToNumber;
    }
  std::ranges::sort (idsAndCards);
  std::ranges::transform (idsAndCards, cards.begin (), [] (std::tuple<size_t, durak::Card> const &idAndCard) { return std::get<1> (idAndCard); });
  return cards;
}
}