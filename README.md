# t9RGB_control
RGB led control for T9 mini PC

This is small Linux app for control RGB LEDs of Mini PC T9, t9 Plus etc.
The app based on Python script of Niccolo Rigacci <https://rigacci.org/wiki/doku.php/doc/appunti/hardware/t9plus_mini_pc_rgb_led_control>.


## Compile
make

## Install
sudo make install

## Usage
t9rgb_control [device] <mode> [brightness speed]
	where mode is one from: "Auto" "Off" "Cycle" "Breathing" "Rainbow".
	brightness and speed value from 1 to 5 (optional, default=1).

## Copyright
__author__ = "Sergey Fedotov"
__copyright__ = "Copyright 2024 Sergey Fedotov <servifed2@gmail.com>"
__license__ = "GPLv3-or-later"
__email__ = "servifed2@gmail.com"
__version__ = "0.1.0"