# Filament Storage Container

## Introduction

I recently began using my 3D printer again, and have been noticing decreased print quality due to moisture absorbtion in the filament.
I am doing to very large, multi-day prints, so wanted something that I could use while printing with the filament. When I began this 
project, I was contacted by RepKord and I am now using their RepBox2 as a base platform to build this box.

## Cool, how do I make one?

First get a [RepBox2](https://www.repkord.com/repbox), then get the below materials
1. Arduino Mega or similar. 
1. A [relay that can control your heater](https://www.aliexpress.com/item/32909704250.html?spm=a2g0s.9042311.0.0.4cc54c4dHngZjt) or 2 if your fan uses a separate power supply than the heater.
1. An [SPI MicroSD Card socket](https://smile.amazon.com/gp/product/B07BJ2P6X6/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1) and a micro SD card.
1. A DHT22 or [AM2302](https://www.aliexpress.com/item/32812831153.html?spm=a2g0s.9042311.0.0.27204c4dLt1bN4)
or similar Temp/Humidity sensor.  
1. You'll need 4, 1 for each filament spool, 
[5kg Load Cell Weight Sensors](https://www.aliexpress.com/item/32864971873.html?spm=a2g0s.9042311.0.0.27204c4dLt1bN4)
1. 4 HX711 boards for the Load Cells.
1. 4 rotary encoders
1. A 20x4 LCD screen
1. 120mm 12V DC fan
1. 12V 10A DC power supply
1. An AC heating element and thermal shutoff from a food dehydrator.  I used the cheapest one I could find, a 
[$30 Presto](https://www.gopresto.com/product/presto-dehydro-electric-food-dehydrator-06300) (they list as $50, but I paid $30).

## Notes

RepKord will eventually be selling these, once we finalize the design, so if you aren't in a hurry, there will be a kit later.

