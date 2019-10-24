# Filament Storage Container

## Introduction

I recently began using my 3D printer again, and have been noticing decreased print quality due to moisture absorbtion in the filament.
I am doing to very large, multi-day prints, so wanted something that I could use while printing with the filament. I saw eSun has a 
filament box that dries and weighs 1 spool. They run about $90-100.  I have a Palette 2 so that I can print from up to 4 materials 
at once. So I would need 4 of these boxes.  Instead of spending nearly $400, I decided to make my own. Since I use OctoPrint to control
and monitor my prints, I wanted to be able to integrate this into OctoPrint as well, which will be a lot easier if I DIY instead of buy.

## Cool, how do I make one?

First get an Arduino Uno or similar. Also, a [relay that can contrl AC](https://www.aliexpress.com/item/32909704250.html?spm=a2g0s.9042311.0.0.4cc54c4dHngZjt).
Then you need a DHT22 or [AM2302](https://www.aliexpress.com/item/32812831153.html?spm=a2g0s.9042311.0.0.27204c4dLt1bN4)
or similar Temp/Humidity sensor.  Then you'll need 4, 1 for each filament spool, 
[5kg Load Cell Weight Sensors](https://www.aliexpress.com/item/32864971873.html?spm=a2g0s.9042311.0.0.27204c4dLt1bN4)
and finally a [20x4 LCD screen](https://www.aliexpress.com/item/32834911170.html?spm=a2g0s.9042311.0.0.27204c4dLt1bN4).
You'll also need a food dehydrator.  I used the cheapest one I could find, a 
[$30 Presto](https://www.gopresto.com/product/presto-dehydro-electric-food-dehydrator-06300) (they list as $50, but I paid $30).
For the main container, you need a large plastic container to house the filament spools. It must have a lid.  I used a large Cambro 
food storage container from a restaurant supply company.  I haven't finished the 3D models to put on top of the scales, but will be 
sharing that as soon as the parts arrive from China and I can verify the measurements. Until then feel free to make your own if you already
have the parts for this. If you want to share them, submit a pull request.

## What about OctoPrint and what's the Mock*.ino for?

The Mock*.ino is what I used to test the [OctoPrint plugin](https://github.com/waltmoorhouse/OctoPrint-Filamentstorage).

## Notes

You need to use the SparkFun Calibration library to get the calibration values for the ino file. More information
can be found at [https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all#installing-the-hx711-arduino-library-and-examples](https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all#installing-the-hx711-arduino-library-and-examples).
