# arduinoDHTLCD
  On interval read DHT sensor, write debug lines to serial, and display to LCD : 
      anthropomorphized action gliph responding to isComfortable() function, true between 73-80F temp and 30-70% humid
      current temp and humidity, 
      indicator if readings are up +, down -, or same = given the average of the running average and a saved collection of events over an interval
      rotating action gliph.

  Serial: 
  88906 | Humidity: 42.10% | Temp: 79.34°F | Index: 78.95°F | All good! avg : 79.38- avg : 42.18-

  16x2 LCD :
  ----------------
  ^_^ 75.00F + <|>
  >|> 49.70% = > <
  
  O_O 75.00F = <|>
  <|< 49.70% = > <
  ---------------
  #_# 85.00F - > <
  >|> 90.50% + <|>

  o_O 85.00F = > <
  <|< 90.50% + <|>
