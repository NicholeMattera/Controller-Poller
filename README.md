# Controller Poller for OJDS-NX

This is a simple program that will poll OJDS-NX for controller inputs and output them to the standard output stream in a CSV format. The CSV has the following columns:

* Left Joystick - X (-1.0 - 1.0)
* Left Joystick - Y (-1.0 - 1.0)
* Right Joystick - X (-1.0 - 1.0)
* Right Joystick - Y (-1.0 - 1.0)
* A Button (0 or 1)
* B Button (0 or 1)
* X Button (0 or 1)
* Y Button (0 or 1)
* L3 Button (0 or 1)
* R3 Button (0 or 1)
* L Button (0 or 1)
* R Button (0 or 1)
* L2 Button (0 or 1)
* R2 Button (0 or 1)
* Plus Button (0 or 1)
* Minus Button (0 or 1)
* D-Pad Left Button (0 or 1)
* D-Pad Up Button (0 or 1)
* D-Pad Right Button (0 or 1)
* D-Pad Down Button (0 or 1)

## Usage

__Poll the controller with no rate limiting:__

```./ctrl_poller 10.0.5.2 > inputs.csv```

__Poll the controller every 17ms (around 60 times per seconds):__

```./ctrl_poller 10.0.5.2 17 > inputs.csv```

To stop just press `Ctrl + C` to close the program.