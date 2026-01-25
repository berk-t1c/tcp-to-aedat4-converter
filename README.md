# TCP to AEDAT4 Converter

Convert raw binary event camera frames received over TCP into AEDAT4 format for display in DV software.

## What This Does

```
Your FPGA/Camera  ──TCP──►  [This Converter]  ──TCP──►  DV Viewer (visualization)
   (port 5000)                                            (port 7777)
```

Your hardware sends binary bit-packed frames over TCP. This software:
1. Receives those frames
2. Converts them to AEDAT4 event format
3. Streams to DV viewer for real-time visualization

---

## Step-by-Step Setup Guide

### Step 1: Install Ubuntu Dependencies

Open a terminal and run these commands one by one:

```bash
# Update package list
sudo apt update

# Add iniVation repository (for DV software)
sudo add-apt-repository ppa:inivation-ppa/inivation
sudo apt update

# Install everything needed
sudo apt install -y build-essential cmake git python3
sudo apt install -y dv-processing dv-gui
```

**What this installs:**
- `build-essential` - C++ compiler
- `cmake` - Build system
- `dv-processing` - Library to create AEDAT4 format
- `dv-gui` - Viewer to see the events

### Step 2: Download and Build the Converter

```bash
# Go to your home directory
cd ~

# Download the code
git clone https://github.com/berkyilmaz01/tcp-to-aedat4-converter.git

# Enter the folder
cd tcp-to-aedat4-converter

# Create build folder
mkdir build
cd build

# Configure and compile
cmake ..
make
```

If successful, you'll see a file called `converter` in the build folder.

### Step 3: Configure for Your Hardware

**IMPORTANT:** Before running, you must configure the settings to match your FPGA output.

Edit the configuration file:
```bash
# Open config file in text editor
nano ~/tcp-to-aedat4-converter/include/config.hpp
```

**Settings you MUST check:**

| Setting | Description | Your Value |
|---------|-------------|------------|
| `width` | Frame width in pixels | 1280 |
| `height` | Frame height in pixels | 780 |
| `camera_ip` | IP address of your FPGA | Change to your FPGA's IP |
| `camera_port` | TCP port your FPGA sends on | 5000 (or your port) |
| `has_header` | Does FPGA send 4-byte size before each frame? | `true` or `false` |

**Settings to try if image looks wrong:**

| Setting | Try if... |
|---------|-----------|
| `msb_first = true` | Image looks like random noise |
| `positive_first = false` | Colors are inverted |
| `row_major = false` | Image is rotated 90 degrees |

After editing, save (Ctrl+O, Enter, Ctrl+X in nano) and rebuild:
```bash
cd ~/tcp-to-aedat4-converter/build
make
```

---

## How to Test (Without Real Hardware)

Before connecting your real hardware, test with the fake camera simulator.

### Test Setup (3 Terminals)

**Open Terminal 1 - Start Fake Camera:**
```bash
cd ~/tcp-to-aedat4-converter
python3 test/fake_camera.py
```
You should see:
```
Fake camera listening on port 5000
Frame size: 249600 bytes (1280x780, 2 channels)
Target FPS: 500
Waiting for connection...
```

**Open Terminal 2 - Start Converter:**
```bash
cd ~/tcp-to-aedat4-converter/build
./converter
```
You should see:
```
TCP to AEDAT4 Converter
Configuration:
  Frame size: 1280 x 780
  ...
Connecting to camera...
Connected successfully!
```

**Open Terminal 3 - Start DV Viewer:**
```bash
dv-gui
```

### Connect DV Viewer to See Events

1. In DV GUI, click **"Add Module"** (top left)
2. Select **"Input"** → **"Network TCP Client"**
3. In the module settings (right panel):
   - Set **IP Address**: `127.0.0.1`
   - Set **Port**: `7777`
4. Click the **Play button** (triangle) to start
5. You should see two moving circles (one for each polarity)

### Screenshot of Expected Result
```
+----------------------------------+
|  DV Viewer                       |
|  ┌────────────────────────────┐  |
|  │     ●                      │  |  ← Positive events (moving horizontally)
|  │           ○                │  |  ← Negative events (moving vertically)
|  │                            │  |
|  └────────────────────────────┘  |
|  FPS: 500  Events: 15000/frame   |
+----------------------------------+
```

---

## How to Connect Your Real Hardware

### Network Setup

1. **Connect your FPGA/camera to the same network as your PC**
   - Direct Ethernet cable: Set static IPs (e.g., FPGA: 192.168.1.100, PC: 192.168.1.1)
   - Through switch/router: Both get IPs from DHCP or set static

2. **Find your FPGA's IP address** and update config:
   ```bash
   nano ~/tcp-to-aedat4-converter/include/config.hpp
   ```
   Change:
   ```cpp
   std::string camera_ip = "192.168.1.100";  // Your FPGA's IP
   int camera_port = 5000;                    // Your FPGA's port
   ```

3. **Rebuild:**
   ```bash
   cd ~/tcp-to-aedat4-converter/build
   make
   ```

### Running with Real Hardware

**Terminal 1 - Start Converter:**
```bash
cd ~/tcp-to-aedat4-converter/build
./converter
```

**Terminal 2 - Start DV Viewer:**
```bash
dv-gui
# Then connect to 127.0.0.1:7777 as described above
```

**On your FPGA:** Start sending frames to the PC's IP on port 5000.

---

## Frame Format Specification

Your FPGA must send data in this format:

### With Header (has_header = true)
```
┌──────────────────┬─────────────────────────────────────────┐
│ 4 bytes (uint32) │              Frame Data                 │
│   Frame Size     │         (249,600 bytes)                 │
│  Little-endian   │                                         │
└──────────────────┴─────────────────────────────────────────┘
```

### Frame Data Structure
```
┌─────────────────────────────┬─────────────────────────────┐
│     Positive Channel        │     Negative Channel        │
│     (124,800 bytes)         │     (124,800 bytes)         │
│   1 bit per pixel           │   1 bit per pixel           │
└─────────────────────────────┴─────────────────────────────┘
```

### Bit Packing (Default: LSB first, Row-major)
```
Byte 0: [pixel7][pixel6][pixel5][pixel4][pixel3][pixel2][pixel1][pixel0]
        (if msb_first=false, pixel0 is bit 0)

Pixels are arranged row by row:
  Row 0: pixels 0-1279
  Row 1: pixels 1280-2559
  ...
```

### Example: Sending from FPGA (Pseudocode)
```verilog
// Send header (4 bytes, little-endian)
send_byte(frame_size[7:0]);
send_byte(frame_size[15:8]);
send_byte(frame_size[23:16]);
send_byte(frame_size[31:24]);

// Send positive channel (124,800 bytes)
for each byte in positive_channel:
    send_byte(byte);

// Send negative channel (124,800 bytes)
for each byte in negative_channel:
    send_byte(byte);
```

---

## Troubleshooting

### Connection Issues

| Problem | Solution |
|---------|----------|
| "Failed to connect to camera" | Check FPGA IP address and port in config. Ping FPGA from PC. |
| "Connection closed by server" | FPGA stopped sending. Check FPGA is running. |
| No events in DV viewer | Make sure DV is connected to port 7777, not 5000. |

### Image Issues

| Problem | Solution |
|---------|----------|
| Image looks like random noise | Try `msb_first = true` in config |
| Colors/polarity inverted | Try `positive_first = false` in config |
| Image rotated 90 degrees | Try `row_major = false` in config |
| Only half the image shows | Check `width` and `height` match your sensor |
| Frames are incomplete | Check `has_header` setting matches FPGA |

### Performance Issues

| Problem | Solution |
|---------|----------|
| Low FPS | Check network connection. Use wired Ethernet, not WiFi. |
| Frames dropping | Increase `recv_buffer_size` in config (default: 50MB) |
| High latency | This is normal for TCP. Use direct Ethernet connection. |

---

## Quick Reference

### Commands Cheat Sheet

```bash
# Build the converter
cd ~/tcp-to-aedat4-converter/build && make

# Run the converter
./converter

# Run fake camera for testing
python3 ~/tcp-to-aedat4-converter/test/fake_camera.py

# Run fake camera with different settings
python3 test/fake_camera.py --port 5000 --fps 100

# Open DV viewer
dv-gui
```

### Port Numbers

| Port | Used By | Direction |
|------|---------|-----------|
| 5000 | Camera/FPGA | Camera → Converter |
| 7777 | DV Viewer | Converter → Viewer |

### File Locations

| File | Purpose |
|------|---------|
| `include/config.hpp` | All settings (edit this!) |
| `build/converter` | The main program |
| `test/fake_camera.py` | Test simulator |

---

## Technical Specifications

- **Resolution:** 1280 x 780 pixels
- **Frame size:** 249,600 bytes (2 channels x 124,800 bytes)
- **Target FPS:** 500-1000 frames per second
- **Throughput:** Up to 250 MB/s (works with 1GbE and 10GbE)
- **Protocol:** TCP/IP
- **Output format:** AEDAT4 (compatible with DV software)

---

## Need Help?

1. Check the troubleshooting section above
2. Look at [ARCHITECTURE.md](ARCHITECTURE.md) for technical details
3. Contact: Berk Yilmaz

## License

MIT License
