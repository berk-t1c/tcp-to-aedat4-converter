# TCP to AEDAT4 Converter

Convert raw binary event camera frames received over TCP into AEDAT4 format for display in DV software.

## Overview

This tool acts as a bridge between a custom event camera (sending binary frames over TCP) and the standard DV ecosystem (iniVation's event camera software).
```
Camera (TCP) ──→ [Converter] ──→ DV Viewer (AEDAT4)
```

## Features

- Receives binary event frames over TCP
- Converts bit-packed data to AEDAT4 event format
- Serves events via TCP for DV software
- Fully configurable (resolution, ports, bit ordering)
- Includes test simulator for development

## Requirements

- Linux (Ubuntu 20.04+ recommended)
- CMake 3.14+
- C++20 compiler (GCC 10+)
- dv-processing library
- OpenCV (installed with dv-processing)
- Python 3 (for test simulator)

## Installation

### 1. Install Dependencies
```bash
# Add iniVation PPA
sudo add-apt-repository ppa:inivation-ppa/inivation
sudo apt update

# Install dv-processing and GUI
sudo apt install -y dv-processing dv-gui

# Install build tools
sudo apt install -y build-essential cmake
```

### 2. Clone and Build
```bash
git clone https://github.com/berkyilmaz01/tcp-to-aedat4-converter.git
cd tcp-to-aedat4-converter

mkdir build && cd build
cmake ..
make
```

## Usage

### Quick Start (Testing)

Open 3 terminals:

**Terminal 1 - Fake Camera:**
```bash
python3 test/fake_camera.py
```

**Terminal 2 - Converter:**
```bash
./build/converter
```

**Terminal 3 - DV Viewer:**
```bash
dv-gui
# Then: Add Module → Input → Network TCP Client
# Set IP: 127.0.0.1, Port: 7777
```

### With Real Camera

1. Edit `include/config.hpp` with your camera settings
2. Rebuild: `cd build && make`
3. Run converter: `./converter`
4. Connect DV viewer to port 7777

## Configuration

Edit `include/config.hpp` to change settings:
```cpp
struct Config {
    // Frame dimensions
    int width = 1280;
    int height = 780;
    
    // Network
    std::string camera_ip = "127.0.0.1";
    int camera_port = 5000;      // Camera sends here
    int aedat_port = 7777;       // DV viewer connects here
    
    // Bit unpacking (flip if image looks wrong)
    bool msb_first = false;
    bool positive_first = true;
    bool row_major = true;
    
    // Frame header
    bool has_header = true;
    int header_size = 4;
};
```

## Troubleshooting

| Problem | Solution |
|---------|----------|
| Image flipped horizontally | Set `row_major = false` |
| Image flipped vertically | Set `positive_first = false` |
| Polarity inverted | Set `positive_first = false` |
| Random noise | Set `msb_first = true` |
| Incomplete frames | Check `has_header` matches camera |

## Project Structure
```
tcp-to-aedat4-converter/
├── ARCHITECTURE.md       # Detailed design document
├── README.md             # This file
├── CMakeLists.txt        # Build configuration
├── include/
│   ├── config.hpp        # Configuration options
│   ├── tcp_receiver.hpp  # TCP receiver class
│   └── frame_unpacker.hpp # Bit unpacking class
├── src/
│   ├── main.cpp          # Entry point
│   ├── tcp_receiver.cpp  # TCP implementation
│   └── frame_unpacker.cpp # Unpacker implementation
└── test/
    └── fake_camera.py    # Test camera simulator
```

## Technical Details

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed design documentation.

## License

MIT License

## Author

Berk Yilmaz
