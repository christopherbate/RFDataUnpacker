#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    signed char map[4] = {1, 3, -3, -1};
    bool complex = true;
    unsigned int channels = 2;
    unsigned int bits = 2;
    string file = "";
    vector<string> channelFilenames;
    vector<ofstream *> outFiles;
    vector<vector<signed char> *> outputBuffers;
    //const unsigned char bitmask = 0x03;
    const unsigned int bufferSize = 1024000;

    try
    {

        // Process the settings file.
        std::ifstream infile;
        infile.open("settings.txt", ifstream::in);
        while (!infile.eof())
        {
            string key;
            string value;
            string value2;
            infile >> key;
            if (key == "bits")
            {
                infile >> value;
                bits = stoul(value);
            }
            if (key == "channels")
            {
                infile >> value;
                channels = stoul(value);
            }
            if (key == "map")
            {
                infile >> value;
                infile >> value2;
            }
            if (key == "file")
            {
                infile >> file;
                cout<<"Input: "<<file<<std::endl;
            }
            if (key == "out")
            {
                infile >> value;
                channelFilenames.push_back(value);
                std::cout << "Added "<<value<<" for output "<< std::endl;
                ofstream *infile1 = new ofstream;
                infile1->open(value, ofstream::out | ofstream::binary);
                if(!infile1->is_open()){
                    throw std::runtime_error("Could not open output file.");
                }
                outFiles.push_back(infile1); // Should be copied in
                vector<signed char> *outBuffer = new vector<signed char>;
                outputBuffers.push_back(outBuffer);
            }
        }
        infile.close();

        // Debug: Display results;
        std::cout << "Channels " << channels << endl
                  << "Bits " << bits << endl
                  << "Complex " << complex << endl;
        std::cout<<"Map:"<<endl;
        for(auto i = 0; i < 4; i++){
            std::cout<<i<<": "<<(signed)map[i]<<std::endl;
        }

        if (channelFilenames.size() != channels)
        {
            throw runtime_error("Must specify the same number of channels as output filenames");
        }

        infile.open(file, ifstream::in | ifstream::binary);
        unsigned char buffer[bufferSize];

        unsigned int channelIdx = 0;
        unsigned int componentIdx = 0;

        while (!infile.eof())
        {
            // read in the bytes;
            infile.read((char *)buffer, bufferSize);
            auto len = infile.gcount();

            // For each bytes.
            for (auto i = 0; i < len; i++)
            {
                unsigned char byte = buffer[i];
                for (unsigned int j = 0; j < 8 / bits; j++)
                {
                    unsigned char point = (byte >> (bits * j)) & 0x3;
                    signed char samplePoint = map[point];
                    outputBuffers[channelIdx]->push_back(samplePoint);
                    if (complex)
                    {
                        componentIdx = (componentIdx + 1) % 2;
                        if(componentIdx == 0)
                        {
                            channelIdx = (channelIdx+1)%channels;
                        }
                    }
                }
            }

            // Write the data to files.
            auto index = 0;
            for (auto it = outFiles.begin(); it != outFiles.end(); it++)
            {
                (*it)->write((char*)outputBuffers[index]->data(), outputBuffers[index]->size());

                outputBuffers[index]->clear();
                index++;
            }
        }

        infile.close();

        while (outFiles.size() > 0)
        {
            ofstream *item = outFiles.back();
            outFiles.pop_back();
            item->close();
            delete item;
        }

        while (outputBuffers.size() > 0)
        {
            vector<signed char> *item = outputBuffers.back();
            outputBuffers.pop_back();
            delete item;
        }
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << endl;
        return -1;
    }
}
