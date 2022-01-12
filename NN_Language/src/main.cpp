// C++ Libs
#include <iostream>
#include <vector>
#include <array>
#include <typeinfo>
#include <string>
#include <unistd.h>
#include <fstream>
#include <map>

// External Libs
#include <Eigen/Dense>
#include <SDL2/SDL.h>

// External Files
#include "main.h"
#include "neuralNet.h"

using Eigen::MatrixXd;
using namespace std;

typedef Eigen::RowVectorXf RowVector;
// typedef vector<RowVector*> data;
typedef vector<RowVector*> data;

int main(int argv, char** args)
{
        bool bTrain = false;
        bool bExitStatus = false;
        string stringInput;
        const size_t M = 10, N = 10;

        //_______________Setup Array_______________
        cout << "___NN Input Array___" << endl;
        
        array<array<int, M>, N> inputArray{};

        string keyboardString = "1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./";

        // Translator creates a dictionary with DataStream values of array size NxM
        auto dictDatastream = translator <char, DataStream <int, M, N>*, int, M, N> (keyboardString);
        
        //_______________Eigen - Machine Learning_______________
        // The NN code below references https://www.geeksforgeeks.org/ml-neural-network-implementation-in-c-from-scratch/
        cout << "___Neural Network___" << endl;

        // TODO - topology input dictates number of neurons per layer
        // create neuronLayers with an extra bias neuron (except output layer)
        // cache and delta vectors are the same dimensions as neuronLayers
        // cache is just another name of the sum of weighted inputs from the previous layer
        NeuralNetwork n({ 2, 3, 1});
        data in_dat, out_dat;
        genData("train");
        // Read CSV must manipulate the vector pointers in the array data
        readCSV("train-in", in_dat);
        readCSV("train-out", out_dat);
        n.train(in_dat, out_dat);

        //_______________SDL2 - Canvas_______________
        cout << "___SDL___" << endl;

        // char tempChar = '&';
        // auto retInputGrid = SDL_Canvas <char, DataStream <int, M, N>*, int, M, N> (bTrain, tempChar, inputArray, dictDatastream, bExitStatus);
        bExitStatus = false;
        usleep(100000);//sleeps for 0.1 second
        // int checkInputSum = compareArray <Node*, int, M, N> (retInputGrid.tRectArray, dictDatastream['1'] -> dataArray);
        // cout << "number of nodes different between input arrays: " << checkInputSum << endl;

        // FIXME - Test this and the grid buttons
        int noisePercent = 10;
        // cout << "Before Noise: " << endl;
        // printArray <int*, M, N> (retInputGrid.tNumbersArray);
        // auto copyOfNumbersArray = noise <int*, M, N> (noisePercent, retInputGrid.tNumbersArray);
        // cout << "After Noise: " << endl;
        // printArray <int*, M, N> (retInputGrid.tNumbersArray);

        // Below is an attempt at starting the loop through dict of keyboardChar/targetArray key/value pairs
        for (unsigned int i = 0; i < keyboardString.size(); i++)
        {
                char* charArray = &keyboardString[0];
                auto retGrid = SDL_Canvas <char, DataStream <int, M, N>*, int, M, N> (bTrain, charArray[i], inputArray, dictDatastream, bExitStatus);

                if (bExitStatus) {break;};
                
                int checkSum = compareArray <Node*, int, M, N> (retGrid.tRectArray, dictDatastream[charArray[i]] -> dataArray);
                printArray <int*, M, N> (retGrid.tNumbersArray);
                usleep(100000);//sleeps for 3 second
        }

        // cout << "number of nodes different between input arrays: " << checkSum << endl;

        // FIXME - make sure to destroy objects when finished with memory
        return EXIT_SUCCESS;
}

// BLUE font in debugger terminal means you can't cout << non-printable things