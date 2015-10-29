# TinkerLicht

Change the colors of and RGB led strip or RGBDigit via Twitter or Facebook.

You can read the project description (in Dutch) here: http://tinkerlicht.crutzen.eu.

A more technical, but outdated, description of "Twitmaslights" (the predecessor of TinkerLicht) can be found here: http://blog.crutzen.eu/2013/12/13/twitmaslights-use-an-arduino-yun-to-control-colored-led-lights-with-twitter/

## Arduino sketches
#### tinkerlicht.ino
The Arduino sketch to use TinkerLicht with an RGB led strip.

#### tinkerlicht_rgbdigit.ino
The Arduino sketch to use TinkerLicht with an RGBDigit shield.

## Python programs
#### tinkerlicht-parse-fb.py
The Python program used by the sketch to search Twitter and Facebook and to return the color code(s)/name(s).

#### tinkerlicht-tweetphoto.py
The Python program used by the sketch to take a picture and post the result on Facebook and Twitter.

#### screams.py
The list of quotes at the beginning of a tweet/Facebookmessage.

#### colors.py
The dictionary of color names and corresponding color codes. Source: https://en.wikipedia.org/wiki/List_of_colors_%28compact%29 and http://www.w3schools.com/html/html_colornames.asp

#### tokens.py
The tokens/secrets to connect with Facebook and Twitter are kept in this seperate file. *Note*: you have to add your own tokens/secrets to this file.
