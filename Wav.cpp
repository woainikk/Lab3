//
// Created by Маргарита on 13.10.2017.
//

#include "Wav.h"
#include <iostream>
#include <cstring> // memset
#include <fstream>
#include <cstdio>
#include <vector>
#include <math.h>

using namespace std;

Wav::Wav(const std::string &f) {
    fileName = f;
    readFile();
}

void Wav::readFile() throw(IOException, IncorrectFormat) {
    nullHeader();

    ifstream file(fileName, ifstream::binary);
    if (!file.good()) {
        throw IOException(fileName);
    }

    file.read((char *) &header, HEADER_SIZE);
    if (file.tellg() != HEADER_SIZE) {
        throw IncorrectFormat(fileName);
    }

    file.seekg(0, ios::end);
    file.tellg();
    checkHeader(file.tellg());
    extractDataInt16(file);
    file.close();

    printInfo();

}

void Wav::nullHeader() {
    memset(&header, 0, sizeof(WavHeader));
}

void
Wav::checkHeader(long long fileSize) throw
(HeaderRiffException, HeaderFileSizeError, HeaderWaveError, HeaderFMTError, HeaderNotPCMError,
HeaderSubchunk1Error, HeaderBytesRateError, HeaderBlockAlignError, HeaderSubchunk2SizeError) {

    if (header.chunkId[0] != 'R' ||
        header.chunkId[1] != 'I' ||
        header.chunkId[2] != 'F' ||
        header.chunkId[3] != 'F') {
        throw HeaderRiffException();
    }

    if (header.chunkSize != fileSize - 8) {
        throw HeaderFileSizeError();
    }

    if (header.format[0] != 'W' ||
        header.format[1] != 'A' ||
        header.format[2] != 'V' ||
        header.format[3] != 'E') {
        throw HeaderWaveError();
    }

    if (header.subchunk1Id[0] != 'f' ||
        header.subchunk1Id[1] != 'm' ||
        header.subchunk1Id[2] != 't' ||
        header.subchunk1Id[3] != ' ') {
        throw HeaderFMTError();
    }

    if (header.audioFormat != 1) {
        throw HeaderNotPCMError();
    }

    if (header.subchunk1Size != 16) {
        throw HeaderSubchunk1Error();
    }

    if (header.byteRate != header.sampleRate * header.numChannels * header.bitsPerSample / 8) {
        throw HeaderBytesRateError();
    }

    if (header.blockAlign != header.numChannels * header.bitsPerSample / 8) {
        throw HeaderBlockAlignError();
    }

    if (header.subchunk2Id[0] != 'd' ||
        header.subchunk2Id[1] != 'a' ||
        header.subchunk2Id[2] != 't' ||
        header.subchunk2Id[3] != 'a') {
        throw HeaderFMTError();
    }

    if (header.subchunk2Size != fileSize - HEADER_SIZE) {
        throw HeaderSubchunk2SizeError();
    }

}

void Wav::printInfo() {
    cout << "--------------------" << endl;
    cout << "AudiFormat:    " << header.audioFormat << endl;
    cout << "numChannels:   " << header.numChannels << endl;
    cout << "sampleRate:    " << header.sampleRate << endl;
    cout << "bitsPerSample: " << header.bitsPerSample << endl;
    cout << "byteRate:      " << header.byteRate << endl;
    cout << "blockAlign:    " << header.blockAlign << endl;
    cout << "chunkSize:     " << header.chunkSize << endl;
    cout << "subchunk1Size: " << header.subchunk1Size << endl;
    cout << "subchunk2Size: " << header.subchunk2Size << endl;
    cout << "--------------------" << endl;

}

void Wav::prefillHeader() {
    header.chunkId[0] = 'R';
    header.chunkId[1] = 'I';
    header.chunkId[2] = 'F';
    header.chunkId[3] = 'F';

    header.format[0] = 'W';
    header.format[1] = 'A';
    header.format[2] = 'V';
    header.format[3] = 'E';

    header.subchunk1Id[0] = 'f';
    header.subchunk1Id[1] = 'm';
    header.subchunk1Id[2] = 't';
    header.subchunk1Id[3] = ' ';

    header.subchunk2Id[0] = 'd';
    header.subchunk2Id[1] = 'a';
    header.subchunk2Id[2] = 't';
    header.subchunk2Id[3] = 'a';

    header.audioFormat = 1;
    header.subchunk1Size = 16;
    header.bitsPerSample = 16;
}

void Wav::fillHeader(unsigned int chanCount, uint16_t bitsPerSample, uint32_t sampleRate, unsigned int samplesCountPerChan)
throw(UnsupportedFormatException, IncorrectParamsException) {

    if (bitsPerSample != 16) {
        throw UnsupportedFormatException();
    }

    if (chanCount < 1) {
        throw IncorrectParamsException();
    }
    prefillHeader();

    uint32_t fileSizeBytes = 44 + chanCount * (bitsPerSample / 8) * samplesCountPerChan;

    header.sampleRate = sampleRate;
    header.numChannels = chanCount;
    header.bitsPerSample = 16;

    header.chunkSize = fileSizeBytes - 8;
    header.subchunk2Size = fileSizeBytes - HEADER_SIZE;

    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = static_cast<uint16_t>(header.numChannels * header.bitsPerSample / 8);

}

void Wav::extractDataInt16(ifstream &file) throw(UnsupportedFormatException, IOException) {

    if (header.bitsPerSample != 16) {
        throw UnsupportedFormatException();
    }

    file.seekg(HEADER_SIZE, ios::beg);

    uint16_t chanCount = header.numChannels;
    unsigned int samplesPerChan = (header.subchunk2Size / sizeof(short)) / chanCount;

    // 1. Reading all PCM data from file to a single vector.
    vector<short> allChannels;
    allChannels.resize(chanCount * samplesPerChan);
    file.read(reinterpret_cast<char *>(allChannels.data()), header.subchunk2Size);
    if (file.tellg() != header.subchunk2Size + HEADER_SIZE) {
        throw IOException(fileName);
    }
    file.close();


    // 2. Put all channels to its own vector.
    channelsData.resize(chanCount);
    for (size_t ch = 0; ch < channelsData.size(); ch++) {
        channelsData[ch].resize(samplesPerChan);
    }

    for (int ch = 0; ch < chanCount; ch++) {
        vector<short> &chdata = channelsData[ch];
        for (size_t i = 0; i < samplesPerChan; i++) {
            chdata[i] = allChannels[chanCount * i + ch];
        }
    }
}

void Wav::makeWavFile(string newFileName) throw(IncorrectParamsException, IOException) {
    unsigned int chanCount = channelsData.size();

    if (chanCount < 1) {
        throw IncorrectParamsException();
    }

    unsigned int samplesCountPerChan = channelsData[0].size();

    // Verify that all channels have the same number of samples.
    for (size_t ch = 0; ch < chanCount; ch++) {
        if (channelsData[ch].size() != (size_t) samplesCountPerChan) {
            throw IncorrectParamsException();
        }
    }

    uint16_t bitsPerSample = 16;
    fillHeader(chanCount, bitsPerSample, header.sampleRate, samplesCountPerChan);


    vector<short> allChannels;
    allChannels.resize(chanCount * samplesCountPerChan);

    for (int ch = 0; ch < chanCount; ch++) {
        const vector<short> &chdata = channelsData[ch];
        for (size_t i = 0; i < samplesCountPerChan; i++) {
            allChannels[chanCount * i + ch] = chdata[i];
        }
    }

    ofstream fileOut(newFileName, ios::binary);

    fileOut.write((char *) &header, sizeof(header));
    fileOut.write((char *) allChannels.data(), sizeof(short) * allChannels.size());
    if (!fileOut.good()) {
        throw IOException(newFileName);
    }

    fileOut.close();

}

void Wav::makeMono() {
    int chanCount = (int) channelsData.size();

    if (chanCount != 2) {
        throw IncorrectParamsException();
    }

    unsigned int samplesCountPerChan = getSampleCountPerChan();

    // Verify that all channels have the same number of samples.
    for (size_t ch = 0; ch < chanCount; ch++) {
        if (channelsData[ch].size() != (size_t) samplesCountPerChan) {
            throw IncorrectParamsException();
        }
    }


    std::vector<short> mono;
    mono.resize(samplesCountPerChan);
    // Mono channel is an arithmetic mean of all (two) channels.
    for (size_t i = 0; i < samplesCountPerChan; i++) {
        mono[i] = static_cast<short>((channelsData[0][i] + channelsData[1][i]) / 2);
    }
    channelsData.pop_back();

}


int Wav::getSampleCountPerChan() const {
    return (int) channelsData[0].size();
}
void Wav::makeReverb(double delaySeconds, float decay) throw(IncorrectParamsException) {
    int chanCount = (int) channelsData.size();

    if (chanCount < 1) {
        throw IncorrectParamsException();
    }

    int samplesCountPerChan = (int) channelsData[0].size();

    // Verify that all channels have the same number of samples.
    for (size_t ch = 0; ch < chanCount; ch++) {
        if (channelsData[ch].size() != (size_t) samplesCountPerChan) {
            throw IncorrectParamsException();
        }
    }

    int delaySamples = (int) (delaySeconds * header.sampleRate);


    for (size_t ch = 0; ch < channelsData.size(); ch++) {
        std::vector<float> tmp;
        tmp.resize(channelsData[ch].size());

        // Convert signal from short to float
        for (size_t i = 0; i < samplesCountPerChan; i++) {
            tmp[i] = channelsData[ch][i];
        }

        // Add a reverb
        for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
            tmp[i + delaySamples] += decay * tmp[i];
        }

        // Find maximum signal's magnitude
        float maxMagnitude = 0.0f;
        for (size_t i = 0; i < samplesCountPerChan - delaySamples; i++) {
            if (abs(tmp[i]) > maxMagnitude) {
                maxMagnitude = abs(tmp[i]);
            }
        }

        // Signed short can keep values from -32768 to +32767,
        // After reverb, usually there are values large 32000.
        // So we must scale all values back to [ -32768 ... 32768 ]
        float normCoef = 30000.0f / maxMagnitude;


        // Scale back and transform floats to shorts.
        for (size_t i = 0; i < samplesCountPerChan; i++) {
            channelsData[ch][i] = (short) (normCoef * tmp[i]);
        }
    }
}
