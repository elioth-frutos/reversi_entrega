import System.Environment
import Decode

main :: IO ()
main =
    getArgs >>=
        (\_ -> putStrLn "D3")
