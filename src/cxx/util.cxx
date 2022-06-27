#include "util.hxx"
#include "src/cxx/compressCard.hxx"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <charconv>
#include <magic_enum.hpp>

std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>
attackAndDefendCardsAndTrump (std::string const &gameState)
{
  auto result = std::tuple<std::vector<durak::Card>, std::vector<durak::Card>, durak::Type>{};
  auto &[attack, defend, trump] = result;
  std::vector<std::string> splitMessage{};
  boost::algorithm::split (splitMessage, gameState, boost::is_any_of (";"));
  if (splitMessage.size () == 3)
    {
      attack = stringToCards (splitMessage.at (0));
      defend = stringToCards (splitMessage.at (1));
      trump = magic_enum::enum_cast<durak::Type> (std::stoi (splitMessage.at (2))).value ();
    }
  else
    {
      throw std::logic_error{ "gameState wrong format should only have 2 times ; gameStateAsString: " + gameState };
    }
  return result;
}
std::vector<durak::Card>
stringToCards (std::string const &cardsAsString)
{
  auto result = std::vector<durak::Card>{};
  std::vector<std::string> splitMessage{};
  boost::algorithm::split (splitMessage, cardsAsString, boost::is_any_of (","));
  for (auto const &value : splitMessage)
    {
      uint8_t tmp{};
      auto [ptr, ec]{ std::from_chars (value.data (), value.data () + value.size (), tmp) };
      if (ec == std::errc ())
        {
          result.push_back (idToCard (tmp));
        }
      else if (ec == std::errc::invalid_argument)
        {
          throw std::logic_error{ "That isn't a number" };
        }
      else if (ec == std::errc::result_out_of_range)
        {
          throw std::logic_error{ "This number is larger than an uint8_t" };
        }
    }
  return result;
}