#include "compressCard.hxx"

namespace durak_computer_controlled_opponent
{



std::vector<durak::Card>
compress (std::vector<durak::Card> cards)
{
  if (cards.empty ()) return {};
  auto idsAndCards = std::vector<std::tuple<size_t, durak::Card> >{};
  std::ranges::transform (cards, std::back_inserter (idsAndCards), [id = size_t{ 0 }] (durak::Card const &card) mutable { return std::tuple<size_t, durak::Card>{ id++, card }; });
  std::ranges::sort (idsAndCards, {}, [] (std::tuple<size_t, durak::Card> const &idAndCard) { return std::get<1> (idAndCard); });
  auto setToNumber = uint16_t{ 0 };
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