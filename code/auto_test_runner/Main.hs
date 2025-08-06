--
-- > Notice: Floating Trees Inc. @ 2025
-- > Create Time: 2025-08-07 00:32:06
--

import System.Process (readProcessWithExitCode)
import System.Exit (ExitCode(..), exitFailure, exitSuccess)
import Control.Monad (mapM)

data Test = Test { name :: String, cmd :: String }

tests :: [Test]
tests =
  [ Test "Kaleidoscope Shader" "./KDShaderTests.exe"
  , Test "Kaleidoscope Core" "./KernelCoreTests.exe"
  , Test "Kaleidoscope GPU" "./KernelGPUTests.exe"
  , Test "Kaleidoscope OS" "./KernelOSTests.exe"
  ]

-- ANSI escape codes
green, red, reset :: String
green = "\x1b[32m"
red   = "\x1b[31m"
reset = "\x1b[0m"

runTest :: Test -> IO Bool
runTest (Test name cmd) = do
  (code, out, err) <- readProcessWithExitCode cmd [] ""
  case code of
    ExitSuccess -> do
      putStrLn $ green ++ "[PASS] " ++ name ++ reset
      return True
    ExitFailure code -> do
      putStrLn $ red ++ "[FAIL] " ++ name ++ " (code " ++ show code ++ ")" ++ reset
      putStrLn out
      putStrLn err
      return False

main :: IO ()
main = do
  putStrLn $ green ++ "---- KALEIDOSCOPE TEST SUITE START ----" ++ reset
  results <- mapM runTest tests
  if and results
    then do
      putStrLn $ green ++ "All tests passed!" ++ reset
      putStrLn $ green ++ "---- KALEIDOSCOPE TEST SUITE END ----" ++ reset
      exitSuccess
    else do
      putStrLn $ red ++ "Some tests failed." ++ reset
      putStrLn $ green ++ "---- KALEIDOSCOPE TEST SUITE END ----" ++ reset
      exitFailure
