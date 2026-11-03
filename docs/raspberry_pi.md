# Running from the Raspberry Pi 4B

The Pi is the host: it runs the Python scripts and logs data. The Mega does the
real-time work.

```bash
sudo apt install python3-venv git
git clone <your repo url> && cd tendon-tactile-hand/software
python3 -m venv .venv && . .venv/bin/activate
pip install -e .            # add [plot] if you want matplotlib on the Pi
sudo usermod -aG dialout $USER   # log out/in afterwards
```

Find the port with `ls /dev/ttyACM*` (usually `/dev/ttyACM0` for a genuine
Mega). Clones with a CH340 show up as `/dev/ttyUSB0`.

Opening the serial port resets the Mega, so every script waits about two
seconds before talking. That is expected.

For a stable device name, add a udev rule matching your board's vendor/product
id (`udevadm info -a -n /dev/ttyACM0`) and point the scripts at that path.

Headless logging:

```bash
python examples/logger.py /dev/ttyACM0 -t 60 -o ../logs/run1.csv
```

Live plots are easiest from a desktop session or over VNC/X forwarding.
