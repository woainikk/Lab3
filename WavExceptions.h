//
// Created by Маргарита on 13.10.2017.
//
#pragma once

#include <iostream>

class IOException : public std::runtime_error {
public:
    explicit IOException(const std::string &wavFileName) : std::runtime_error("File " + wavFileName + " does not found") {}
};


class IncorrectFormat : public std::runtime_error {
public:
    explicit IncorrectFormat(const std::string &wavFileName) : std::runtime_error("File " + wavFileName + "is incorrect") {}
};


class HeaderRiffException : public std::runtime_error {
public:
    explicit HeaderRiffException() : std::runtime_error("Header riff error") {}
};

class HeaderFileSizeError : public std::runtime_error {
public:
    explicit HeaderFileSizeError() : std::runtime_error("Header file size error") {}
};

class HeaderWaveError : public std::runtime_error {
public:
    explicit HeaderWaveError() : std::runtime_error("Header wave error") {}
};

class HeaderFMTError : public std::runtime_error {
public:
    explicit HeaderFMTError() : std::runtime_error("Header FMT error") {}
};

class HeaderNotPCMError : public std::runtime_error {
public:
    explicit HeaderNotPCMError() : std::runtime_error("Header not PCM error") {}
};


class HeaderSubchunk1Error : public std::runtime_error {
public:
    explicit HeaderSubchunk1Error() : std::runtime_error("Header subchank1 error") {}
};

class HeaderBytesRateError : public std::runtime_error {
public:
    explicit HeaderBytesRateError() : std::runtime_error("Header bytes rate error") {}
};

class HeaderBlockAlignError : public std::runtime_error {
public:
    explicit HeaderBlockAlignError() : std::runtime_error("Header block align error") {}
};

class HeaderSubchunk2SizeError : public std::runtime_error {
public:
    explicit HeaderSubchunk2SizeError() : std::runtime_error("Header subchank2 size error") {}
};

class UnsupportedFormatException : public std::runtime_error {
public:
    explicit UnsupportedFormatException() : std::runtime_error("Unsupported Format error") {}
};

class IncorrectParamsException : public std::runtime_error {
public:
    explicit IncorrectParamsException() : std::runtime_error("Incorrect params error") {}
};


