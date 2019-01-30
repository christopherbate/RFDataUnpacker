#include "Unpacker.hpp"

template class Unpacker<int8_t>;
template class Unpacker<int32_t>;

template <typename T>
Unpacker<T>::Unpacker(std::vector<int>& map, unsigned int numChannels,
                   unsigned int numBits, bool complex)
    : m_map(map), m_numChannels(numChannels), m_numBits(numBits),
      m_isComplex(complex) {
    ResetIdicies();
}

template <typename T>
Unpacker<T>::Unpacker() {
    m_isComplex = true;
    m_map = {1, 3, -3, -1};
    m_numChannels = 2;
    m_numBits = 2;
    ResetIdicies();
}

template <typename T>
void Unpacker<T>::Unpack(const std::vector<uint8_t>& inputBuffer,
                      std::vector<std::vector<T>>& outputBuffers) {
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
        // 
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

template <typename T>
double Unpacker<T>::GetChanSamplesPerByte() {
    return ((double)(8 / m_numBits) / (double)(m_isComplex ? 2 : 1) /
            (double)m_numChannels);
}

template <typename T>
unsigned int Unpacker<T>::GetOutputBufferSize(unsigned int inputSize) {
    double chanSamplesPerByte = GetChanSamplesPerByte();
    return (unsigned int)(inputSize)*chanSamplesPerByte * (m_isComplex ? 2 : 1);
}