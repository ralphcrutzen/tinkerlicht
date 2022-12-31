# TinkerLicht

A Mastodon bot programmed in Node-Red to change the color of an RGB led strip via a toot with hashtag #tinkerlicht.
Directly after tooting, the bot replies with a picture of the lights in the new color.

Example: https://botsin.space/@tinkerlicht

Hardware:
- Raspberry Pi (Zero W)
- Raspberry Pi Camera
- Led strip

Software:
- Node-Red
- Extra nodes:
  - node-red-node-pi-gpio
  - node-red-contrib-mastodon
  - node-red-contrib-camerapi
  - node-red-node-pi-neopixel

To do:
- Dynamically set the Access Token properties of the Mastodon nodes. After this, I can publish the flow to this repository.
