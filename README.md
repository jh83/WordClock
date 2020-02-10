# WordClock

Some time ago i decided to build my own "WordClock" and I must say that im quite happy with the results:

![Image of WordClock](images/WordClock_1_lowres.png?raw=true)

The hardware consists of:

- ESP8266, as the "brain". Wifi and NTP connected and hosts a web page. Th webpage is used to configure time zone, NTP sync frequency, led strength among other things.
- ~~WS2812 LED strip cut to size to form a 14x10 matrix.~~ Custom made WS2813 LED panel (replaced the previous ws2812 strip due to dead led).
- Radar module (RCWL-0516) to detect movements. If movement isn't detected in a configurable amount of time (thru the web interface) then the LEDs are turned off. 
  - Radar modules can "see thru" objects which makes it possible to mount behind the plywood front and have *working* results. Compared to PIR sensors that needs an open view.
- Lasercut 3mm plywood. A 40w laser was used in my case
- Mahogny frame. Cut from a bigger piece of wood and made into a frame.
- A white plastic bag used as a *diffuser* between the LED and the plywood front. You want this layer as thin as possible so that there isn't any light traveling to other characters thru the diffuser itself.
- A custom/modified stencil font family was created by me. A stencil font is required since parts of the letters otherwise will be lost after lasercutting

The size of the led matrix is around 10x10cm which makes each individual letter very small - around 6 mm tall and 5 mm wide.

You *might* be able to cut the plywood with a CNC router with a really tiny end mill, but the forces from the end mill might damage the tiny parts of the letters.

Tools/knowledge needed:

- A lasercutter (or ordering a custom laser cut part from internet).
- Basic woodworking tools and some wood glue.
- Basic knowledge about flashing Arduino code onto a ESP8266.
- Basic soldering skills.

Cost:

- Lasercut plywood.
  - Cheap if you own a laser cutter.
  - Price unknown if you need someone to lasercut it for you.
- LED Board
  - I had mine fabricated thru JLCPCB which also does assembly. The PCB with 140 WS2813, 140 capacitors and 140 resistors soldered onto it was about 17 USD plus shipping for a finished product.
- ESP8266.
  - I used a Wemos Mini pro since that was what I had in stock. Costs around 10 USD
- Radar sensor.
  - Around 2 USD
- Mahogny.
  - Expensive, atleast here in sweden.
