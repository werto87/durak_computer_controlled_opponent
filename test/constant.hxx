#ifndef FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7
#define FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7
#include "durak/game.hxx"
#include <string>

using namespace durak;
inline std::vector<Card>
testCardDeck ()
{
  return std::vector<Card>{ { 7, Type::clubs }, { 8, Type::clubs }, { 3, Type::hearts }, { 3, Type::clubs }, { 2, Type::diamonds }, { 3, Type::diamonds }, { 2, Type::clubs }, { 5, Type::diamonds }, { 6, Type::diamonds }, { 7, Type::diamonds }, { 8, Type::diamonds }, { 9, Type::diamonds }, { 1, Type::spades }, { 2, Type::spades }, { 3, Type::spades }, { 1, Type::diamonds }, { 5, Type::spades }, { 6, Type::spades }, { 7, Type::spades }, { 8, Type::spades }, { 9, Type::spades }, { 1, Type::hearts }, { 2, Type::hearts }, { 9, Type::clubs }, { 1, Type::clubs }, { 5, Type::hearts }, { 6, Type::clubs }, { 7, Type::hearts }, { 8, Type::hearts }, { 9, Type::hearts }, { 4, Type::hearts }, { 4, Type::diamonds }, { 4, Type::spades }, { 4, Type::clubs }, { 5, Type::clubs }, { 6, Type::hearts } };
}

inline std::vector<Card>
testCardDeck8 ()
{
  return std::vector<Card>{ { 8, Type::hearts }, { 9, Type::hearts }, { 4, Type::hearts }, { 4, Type::diamonds }, { 4, Type::spades }, { 4, Type::clubs }, { 5, Type::clubs }, { 6, Type::hearts } };
}

inline std::vector<Card>
testCardDeck16 ()
{
  return std::vector<Card>{ { 3, Type::hearts }, { 6, Type::diamonds }, { 7, Type::clubs }, { 8, Type::clubs }, { 3, Type::clubs }, { 2, Type::diamonds }, { 3, Type::diamonds }, { 2, Type::clubs }, { 8, Type::hearts }, { 9, Type::hearts }, { 4, Type::hearts }, { 4, Type::diamonds }, { 4, Type::spades }, { 4, Type::clubs }, { 5, Type::clubs }, { 6, Type::hearts } };
}

inline std::vector<Card>
testCardDeck12 ()
{
  return std::vector<Card>{ { 3, Type::clubs }, { 2, Type::diamonds }, { 3, Type::diamonds }, { 2, Type::clubs }, { 8, Type::hearts }, { 9, Type::hearts }, { 4, Type::hearts }, { 4, Type::diamonds }, { 4, Type::spades }, { 4, Type::clubs }, { 5, Type::clubs }, { 6, Type::hearts } };
}

inline std::vector<Card>
testCardDeck4 ()
{
  return std::vector<Card>{ { 3, Type::diamonds }, { 2, Type::diamonds }, { 3, Type::clubs }, { 2, Type::clubs } };
}

#endif /* FE334B5B_FA67_454D_A6F5_A1CBF7D02BB7 */
