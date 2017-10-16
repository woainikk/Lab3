#include <iostream>
#include "Wav.h"
#include "WavExceptions.h"
using namespace std;
int main() {
    string fileName;
    cin>>fileName;
    Wav myWav(fileName);
    //myWav.makeReverb(0.1, 0.9);
    myWav.makeMono();
    myWav.makeWavFile("test.wav");

}