// C++ Libs
#include <iostream>
#include <vector>
#include <array>
#include <typeinfo>
#include <string>

// External Libs
#include <Eigen/Dense>

// External Files
#include "main.h"

using Eigen::MatrixXd;
using namespace std;

typedef Eigen::RowVectorXf RowVector;
typedef vector<RowVector*> data;

int main(int argv, char** args)
{
        bool train = false;
        string stringInput;
        const size_t M = 7, N = 10;

        //_______________Setup Array_______________
        cout << "___NN Input Array___" << endl;
        
        array<array<int, M>, N> inputArray{};

        // FIXME - Instead of creating a string of keyboard characters we can abstract the targets into images drawn by user. The keys to the dict should be of type int instead
        string keyboardString = "1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./";

        // Translator creates a dictionary with DataStream values of array size NxM
        auto dictDatastream = translator <char, DataStream <int, M, N>*, int, M, N> (keyboardString);

        //_______________Eigen - Machine Learning_______________
        // The NN code below references https://www.geeksforgeeks.org/ml-neural-network-implementation-in-c-from-scratch/
        cout << "___Neural Network___" << endl;

        // NeuralNetwork n({ 2, 3, 1 });
        // data in_dat, out_dat;
        // genData("test");
        // readCSV("test-in", in_dat);
        // readCSV("test-out", out_dat);
        // n.train(in_dat, out_dat);

        //_______________SDL2 - Canvas_______________
        cout << "___SDL___" << endl;

        char tempChar = '&';
        auto retInputGrid = SDL_Canvas <char, DataStream <int, M, N>*, int, M, N> (train, tempChar, inputArray, dictDatastream);

        int checkInputSum = compareArray <Node*, int, M, N> (retInputGrid.tRectArray, dictDatastream['1'] -> dataArray);

        cout << "number of nodes different between input arrays: " << checkInputSum << endl;

        // FIXME - Test this and the grid buttons
        int noisePercent = 70;
        noise <Node*, M, N> (noisePercent, retInputGrid.tRectArray);

        // Below is an attempt at starting the loop through dict of keyboardChar/targetArray key/value pairs

        for (unsigned int i = 0; i < keyboardString.size(); i++)
        {
                char* charArray = &keyboardString[0];
                // Return type for SDL_Canvas is Grid <Node*, M, N>
                auto retGrid = SDL_Canvas <char, DataStream <int, M, N>*, int, M, N> (train, charArray[i], inputArray, dictDatastream);
                int checkSum = compareArray <Node*, int, M, N> (retGrid.tRectArray, dictDatastream[charArray[i]] -> dataArray);
                // print array takes reference as input array of int pointers
                printArray <int*, M, N> (retGrid.tNumbersArray);
        }

        // cout << "number of nodes different between input arrays: " << checkSum << endl;

        // FIXME - make sure to destroy objects when finished with memory
        return EXIT_SUCCESS;
}

// BLUE font in debugger terminal means you can't cout << non-printable things