#include "durak_computer_controlled_opponent/guess.hxx"
#include "durak_computer_controlled_opponent/simulation/action.hxx"
#include "durak_computer_controlled_opponent/util.hxx"
#include <durak/game.hxx>
#include <ranges>

namespace durak_computer_controlled_opponent::guess
{
std::expected<MoveToPlay, NextMoveToPlayForRoleError>
nextMoveToPlayForRole (durak::Game const &game, durak::PlayerRole playerRole)
{
  auto const &allowedMoves = game.getAllowedMoves (playerRole);
  if (allowedMoves.empty ()) return std::unexpected (NextMoveToPlayForRoleError::couldNotFindAMoveToPlay);
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
            return MoveToPlay{ Move::PassOrTakeCard };
          }
        break;
      }
    case durak::PlayerRole::defend:
      {
        if (std::ranges::contains (allowedMoves, durak::Move::defend))
          {
            if (auto defendingPlayer = game.getDefendingPlayer ())
              {
                auto const &cards = defendingPlayer->getCards ();
                auto const &table = game.getTable ();
                for (auto const &[cardToBeat, unused] : table | std::views::filter ([] (auto cardAndOptionalCard) { return not std::get<1> (cardAndOptionalCard).has_value (); }))
                  {
                    for (auto const &card : cards)
                      {
                        if (durak::beats (cardToBeat, card, game.getTrump ()))
                          {
                            return MoveToPlay{ Move::PlayCard, card };
                          }
                      }
                    return MoveToPlay{ Move::PassOrTakeCard };
                  }
              }
          }
        else
          {
            return MoveToPlay{ Move::PassOrTakeCard };
          }
        break;
      }
    }
  return std::unexpected (NextMoveToPlayForRoleError::couldNotFindAMoveToPlay);
}

}