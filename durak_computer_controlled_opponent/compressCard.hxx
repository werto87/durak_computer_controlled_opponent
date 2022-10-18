#ifndef DAEB4B19_9360_42CE_B19B_707A7CA4258C
#define DAEB4B19_9360_42CE_B19B_707A7CA4258C

#include <durak/card.hxx>

namespace durak_computer_controlled_opponent
{

durak::Card idToCard (uint8_t id);

uint8_t cardToId (durak::Card const &card);

std::vector<uint8_t> cardsToIds (const std::vector<durak::Card>& cards);

std::vector<durak::Card> idsToCards (const std::vector<uint8_t>& ids);

std::vector<durak::Card> compress (std::vector<durak::Card> cards);

}
#endif /* DAEB4B19_9360_42CE_B19B_707A7CA4258C */
