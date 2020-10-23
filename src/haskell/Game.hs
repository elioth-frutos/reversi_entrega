module Game where

import Data.Array

data Player = PlayerW | PlayerB
    deriving (Eq, Show)

data Cell = Empty | Valid | Full Player
    deriving (Eq, Show)

data State = Running | GameOver
    deriving (Eq, Show)

type Board = Array (Int, Int) Cell

type Move = (Int, Int)

data Game = Game { gameBoard :: Board
                 , gamePlayer :: Player
                 , gameState :: State
                 } deriving (Eq, Show)

boardSize :: Int
boardSize = 8
