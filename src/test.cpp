#include "QuickTestCPP.h"
#include "Unpacker.hpp"

int main() {
    TestRunner::GetRunner()->AddTest("Unpacker", "Can instatiate unpacker",
                                     []() {
                                         Unpacker unpacker;
                                         return 1;
                                     });

    TestRunner::GetRunner()->AddTest(
        "Unpacker", "Can unpack two channels packed", []() {
            Unpacker unpacker;

            std::vector<std::vector<int8_t>> outputs;
            outputs.resize(2);
            outputs[0].resize(4);
            outputs[1].resize(4);

            std::vector<int8_t> truth1 = {3, -3, 1, 3};

            std::vector<int8_t> truth2 = {-1, 1, -3, -1};

            std::vector<uint8_t> input = {
                (0x1) | (0x2) << 2 | (0x3) << 4 | (0x0),
                (0x0) | (0x1) << 2 | (0x2) << 4 | (0x3) << 6};

            unpacker.Unpack(input, outputs);

            for (unsigned i = 0; i < 4; i++) {
                if (truth1[i] != outputs[0][i]) {
                    std::cout << "0 " << i << " : " << (signed)truth1[i] << " "
                              << (signed)outputs[0][i] << std::endl;
                    return 0;
                }
            }

            for (unsigned i = 0; i < 4; i++) {
                if (truth2[i] != outputs[1][i]) {
                    std::cout << "1 " << i << " : " << (signed)truth2[i] << " "
                              << (signed)outputs[1][i] << std::endl;
                    return 0;
                }
            }

            return 1;
        });

    TestRunner::GetRunner()->AddTest(
        "Unpacker", "Correct output buffer size for 2chn 2bit IQ", []() {
            Unpacker unpacker;

            if (unpacker.GetChanSamplesPerByte() != 1.0) {
                std::cerr << "Incorrect samples per byte"
                          << unpacker.GetChanSamplesPerByte() << std::endl;
                return 0;
            }

            if (unpacker.GetOutputBufferSize(100) != 200) {
                return 0;
            }

            return 1;
        });

    TestRunner::GetRunner()->AddTest(
        "Unpacker", "Correct output buffer size for 4chn 2bit IQ", []() {
            std::vector<int> map = {1, 3, -3, -1};
            Unpacker unpacker(map, 4, 2, true);

            if (unpacker.GetChanSamplesPerByte() != 0.5) {
                return 0;
            }

            if (unpacker.GetOutputBufferSize(100) != 100) {
                return 0;
            }

            return 1;
        });

    TestRunner::GetRunner()->AddTest(
        "Unpacker", "Correct output buffer size for 1chn 2bit IQ", []() {
            std::vector<int> map = {1, 3, -3, -1};
            Unpacker unpacker(map, 1, 2, true);

            if (unpacker.GetChanSamplesPerByte() != 2) {
                return 0;
            }

            if (unpacker.GetOutputBufferSize(100) != 400) {
                return 0;
            }

            return 1;
        });

    TestRunner::GetRunner()->Run();

    TestRunner::GetRunner()->PrintSummary();

    return TestRunner::GetRunner()->GetRetCode();
}