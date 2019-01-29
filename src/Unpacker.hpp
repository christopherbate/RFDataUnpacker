#ifndef UNPACKER_H
#define UNPACKER_H

#include <cstdint>
#include <exception>
#include <vector>

/**
 * Unpacker class
 */
class Unpacker {
  public:
    /**
     * Init with parameters
     */
    Unpacker(std::vector<int>& map, unsigned int numChannels,
             unsigned int numBits, bool complex)
        : m_map(map), m_numChannels(numChannels), m_numBits(numBits),
          m_isComplex(complex) {
        ResetIdicies();
    }

    /**
     * Defaults
     */
    Unpacker() {
        m_isComplex = true;
        m_map = {1, 3, -3, -1};
        m_numChannels = 2;
        m_numBits = 2;
        ResetIdicies();
    }

    void ResetIdicies() { m_componentIdx = m_channelIdx = 0; }

    double GetChanSamplesPerByte() {
        return ((double)(8 / m_numBits) / (double)(m_isComplex ? 2 : 1) /
                (double)m_numChannels);
    }

    unsigned int GetOutputBufferSize(unsigned int inputSize) {
        double chanSamplesPerByte = GetChanSamplesPerByte();
        return (unsigned int)(inputSize)*chanSamplesPerByte *
               (m_isComplex ? 2 : 1);
    }

    /**
     * Unpack
     */
    void Unpack(std::vector<uint8_t>& inputBuffer,
                std::vector<std::vector<int8_t>>& outputBuffers) {
        unsigned int outputIdx = 0;

        // Verify output Size.
        unsigned int outputSize = GetOutputBufferSize(inputBuffer.size());
        for (auto iter = outputBuffers.begin(); iter != outputBuffers.end();
             iter++) {
            if (outputSize != (*iter).size()) {
                throw std::runtime_error("Output buffer size incorrect.");
            }
        }

        // Verify output Buffer is the correct size.
        // For each bytes.
        for (unsigned int i = 0; i < inputBuffer.size(); i++) {
            unsigned char byte = inputBuffer[i];
            for (unsigned int j = 0; j < 8 / m_numBits; j++) {
                unsigned char point = (byte >> (m_numBits * j)) & 0x3;

                int samplePoint = m_map[point];
                outputBuffers[m_channelIdx][outputIdx + m_componentIdx] =
                    (samplePoint);

                if (m_isComplex) {
                    m_componentIdx = (m_componentIdx + 1) % 2;
                    if (m_componentIdx == 0) {
                        m_channelIdx = (m_channelIdx + 1) % m_numChannels;
                        if (m_channelIdx == 0)
                            outputIdx = outputIdx + (m_isComplex ? 2 : 1);
                    }
                }
            }
        }
    }

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