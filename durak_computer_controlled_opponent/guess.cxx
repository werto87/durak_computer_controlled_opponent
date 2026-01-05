#include "durak_computer_controlled_opponent/guess.hxx"
#include "durak_computer_controlled_opponent/simulation/action.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <durak/game.hxx>
#include <ranges>

namespace durak_computer_controlled_opponent::guess
{
std::expected<MoveToPlay, NextMoveToPlayForRoleError>
nextMoveToPlayForRole (std::filesystem::path const &databasePath, durak::Game const &game, durak::PlayerRole playerRole)
{
  auto const &allowedMoves = game.getAllowedMoves (playerRole);
  switch (playerRole)
    {
    case durak::PlayerRole::attack:
    case durak::PlayerRole::assistAttacker:
      {
        if (std::ranges::contains (allowedMoves, durak::Move::startAttack))
          {
            if (auto attackingPlayer = game.getAttackingPlayer ())
              {
                auto playerCards = attackingPlayer->getCards ();
                std::ranges::sort (playerCards);
                std::ranges::stable_partition (playerCards, [trump = game.getTrump ()] (durak::Card const &card) { return card.type != trump; });
                return MoveToPlay{ Move::PlayCard, playerCards.front () };
              }
          }
        else if (std::ranges::contains (allowedMoves, durak::Move::addCard))
          {
            auto playerCards = std::vector<durak::Card>{};
            if (durak::PlayerRole::attack == playerRole)
              {
                if (auto attackingPlayer = game.getAttackingPlayer ())
                  {
                    playerCards = attackingPlayer->getCards ();
                  }
              }
            else
              {
                if (auto assistingPlayer = game.getAssistingPlayer ())
                  {
                    playerCards = assistingPlayer->getCards ();
                  }
              }
            for (auto const &card : game.getTableAsVector ())
              {
                if (auto cardItr = std::ranges::find_if (playerCards, [&card] (durak::Card const &_card) { return card.value == _card.value; }); cardItr != playerCards.end ())
                  {
                    return MoveToPlay{ Move::PlayCard, *cardItr };
                  }
              }
          }
        else if (std::ranges::contains (allowedMoves, durak::Move::pass))
          {
            auto moveToPlay = MoveToPlay{};
            moveToPlay.move = Move::PassOrTakeCard;
            return moveToPlay;
          }
      }
    case durak::PlayerRole::defend:
      {
        break;
      }
    }
  return {};
}

}