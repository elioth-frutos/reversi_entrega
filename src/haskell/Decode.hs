module Decode
    ( decode
    ) where

import Data.Array
import Game

decode :: String -> Game
decode str =
    Game { gameBoard = constructGameBoard str
         , gamePlayer = getPlayer str
         , gameState = Running
         }

constructGameBoard :: String -> Board
constructGameBoard =
    (listArray bounds) . stringToListOfCells
        where bounds = ((0, 0), (boardSize-1, boardSize-1))

stringToListOfCells :: String -> [Cell]
stringToListOfCells (c:[]) = []
stringToListOfCells (c:cs) =
    convertCharToCell c : stringToListOfCells cs

convertCharToCell :: Char -> Cell
convertCharToCell char =
    case char of
      'B' -> Full PlayerB
      'W' -> Full PlayerW
      'V' -> Valid
      'E' -> Empty

getPlayer :: String -> Player
getPlayer = convertCharToPlayer . last

convertCharToPlayer :: Char -> Player
convertCharToPlayer char =
    case char of
      'B' -> PlayerB
      'W' -> PlayerW
