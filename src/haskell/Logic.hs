module Logic where

switchPlayer :: Game -> Game
switchPlayer game =
    case gamePlayer game ->
        PlayerW -> game { gamePlayer = PlayerB }
        PlayerB -> game { gamePlayer = PlayerW }

winner :: Board -> Maybe Player
winner board = asum

checkGameOver :: Game -> Game
checkGameOver game
  | Just p
