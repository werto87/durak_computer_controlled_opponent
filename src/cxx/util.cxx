#include "util.hxx"

std::string
gameStateAsString (std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > const &gameState)
{
  return vectorToString (std::get<0> (gameState)) + " " + vectorToString (std::get<1> (gameState));
}