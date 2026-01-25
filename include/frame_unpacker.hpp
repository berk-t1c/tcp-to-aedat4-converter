#pragma once

#include "config.hpp"
#include <dv-processing/core/event.hpp>
#include <vector>
#include <cstdint>

namespace converter {

/**
 * Frame Unpacker class
 * 
 * Converts binary bit-packed frames into dv::EventStore.
 * 
 * Input format:
 *   - 2 channels (positive and negative events)
 *   - Each channel: width × height bits (1 bit per pixel)
 *   - Total: 2 × width × height / 8 bytes
 * 
 * Output format:
 *   - dv::EventStore containing events with (timestamp, x, y, polarity)
 */
class FrameUnpacker {
public:
    /**
     * Constructor
     * @param cfg Configuration reference
     */
    explicit FrameUnpacker(const Config& cfg);
    
    /**
     * Unpack a binary frame into events
     * 
     * @param frame_data Raw binary frame data
     * @param frame_number Frame sequence number (for timestamp generation)
     * @param events Output event store (will be cleared first)
     * @return Number of events unpacked
     */
    size_t unpack(
        const std::vector<uint8_t>& frame_data,
        uint64_t frame_number,
        dv::EventStore& events
    );
    
    /**
     * Unpack a binary frame into events (pointer version)
     * 
     * @param frame_data Raw binary frame data pointer
     * @param data_size Size of frame data in bytes
     * @param frame_number Frame sequence number (for timestamp generation)
     * @param events Output event store (will be cleared first)
     * @return Number of events unpacked
     */
    size_t unpack(
        const uint8_t* frame_data,
        size_t data_size,
        uint64_t frame_number,
        dv::EventStore& events
    );
    
    /**
     * Get expected frame size in bytes
     * @return Frame size
     */
    int getExpectedFrameSize() const;
    
    /**
     * Get resolution
     * @return Resolution as cv::Size
     */
    cv::Size getResolution() const;

private:
    /**
     * Check if a specific bit is set in the frame data
     * 
     * @param data Pointer to channel data
     * @param x Pixel x coordinate
     * @param y Pixel y coordinate
     * @return true if bit is set
     */
    bool getBit(const uint8_t* data, int x, int y) const;
    
    /**
     * Calculate bit index based on pixel coordinates
     * 
     * @param x Pixel x coordinate
     * @param y Pixel y coordinate
     * @return Bit index in the data array
     */
    int getBitIndex(int x, int y) const;
    
    const Config& config_;
};

} // namespace converter
