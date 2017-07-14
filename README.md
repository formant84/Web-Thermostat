# web-thermostat

This is the source for a web-enabled central heating thermostat. 

The arduino sketch is designed to run on an ESP8862 with the boiler connected via a relay.

The ESP serves up a website loadedbased on the bootstrap framework which creates a 
websocket connection to the arduino sketch. This is then used as a bridge to pass 
data between the javascript code and the arduino sketch.

The majoirty of the javascript code is to control the appearance and behaviour of the rather snazzy round sliders.

The website can actually be hosted on any device as long as has the correct IP address for the thermostat entered 
into the javascript file.
