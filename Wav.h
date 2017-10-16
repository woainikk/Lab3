//
// Created by Маргарита on 13.10.2017.
//
#pragma once

#include "WavExceptions.h"
#include <iostream>
#include <fstream>
#include <vector>

class Wav {
public:
    std::string fileName;

    Wav(const std::string &f);

    void makeMono();

    void makeReverb(double delaySeconds, float decay) throw(IncorrectParamsException);


    void makeWavFile(std::string newFileName) throw(IncorrectParamsException, IOException);

private:
    void readFile() throw(IOException, IncorrectFormat);

    std::vector<std::vector<short>> channelsData;

    void nullHeader();

    struct WavHeader {
        char chunkId[4];

        uint32_t chunkSize;

        char format[4];

        char subchunk1Id[4];

        uint32_t subchunk1Size;

        uint16_t audioFormat;

        uint16_t numChannels;

        uint32_t sampleRate;

        uint32_t byteRate;

        uint16_t blockAlign;

        uint16_t bitsPerSample;

        char subchunk2Id[4];

        uint32_t subchunk2Size;
    };
    WavHeader header;

    static const int HEADER_SIZE = 44;

    void checkHeader(long long fileBytesSize) throw(HeaderRiffException, HeaderFileSizeError, HeaderWaveError,
    HeaderFMTError, HeaderNotPCMError, HeaderSubchunk1Error, HeaderBytesRateError, HeaderBlockAlignError,
    HeaderSubchunk2SizeError);

    void printInfo();

    void prefillHeader();

    void fillHeader(unsigned int chanCount, uint16_t bitsPerSample, uint32_t sampleRate, uint32_t samplesCountPerChan)throw
    (UnsupportedFormatException, IncorrectParamsException);

    void extractDataInt16(std::ifstream &file) throw(UnsupportedFormatException, IOException);



    int getSampleCountPerChan() const;

};



