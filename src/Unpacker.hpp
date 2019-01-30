#ifndef UNPACKER_H
#define UNPACKER_H

#include <cstdint>
#include <stdexcept>
#include <vector>

/**
 * Unpacker class
 * The class accepts a template type which is the output type (signed int, int8_t, etc)
 * It unpacks complex data as sequential I,Q,I,Q
 */
template <typename T>
class Unpacker {
  public:
    /**
     * Constructor for alternate params.
     * map - the mapping from source data (considering source data bits as 
     *       an unsigned integer) to the correct integer mapping.
     */
    Unpacker(std::vector<int>& map, unsigned int numChannels,
             unsigned int numBits, bool complex);
    /**
     * Default constructor
     */
    Unpacker();

    void ResetIdicies() { m_componentIdx = m_channelIdx = 0; }

    /**
     * Returns how many samples per channel per byte 
     * in the packed data according to current settings.
     */
    double GetChanSamplesPerByte();
    
    /**
     * Returns how large an output buffer (per channel)
     * is needed for the given input size.
     * Return is "units of T"
     */
    unsigned int GetOutputBufferSize(unsigned int inputSize);

    /**
     * Unpack
     */
    void Unpack(const std::vector<uint8_t>& inputBuffer,
                std::vector<std::vector<T>>& outputBuffers);

  private:
    std::vector<int> m_map;
    unsigned int m_numChannels;
    unsigned int m_numBits;
    bool m_isComplex;
    unsigned int m_channelIdx;
    unsigned int m_componentIdx;
    unsigned int m_outputBufferSize;
};

#endif