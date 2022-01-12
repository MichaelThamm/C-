// C++ Libs
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <string>
#include <chrono>
#include <thread>

#include <iterator>
#include <algorithm>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

// External Libs
#include <Eigen/Dense>
#include <SDL2/SDL.h>

using Eigen::MatrixXd;
using namespace std;

typedef float Scalar;
typedef Eigen::MatrixXf Matrix;
typedef Eigen::RowVectorXf RowVector;

SDL_Event event;


// function for length of an array
template <class T, size_t size>
int lengthArray(const array<T, size> &iArray)
{
    return iArray.size();
}


Scalar activationFunction(Scalar x)
{
    return tanhf(x);
}


Scalar activationFunctionDerivative(Scalar x)
{
    return 1 - tanhf(x) * tanhf(x);
}


//________________class for nodes in the grid________________
class Node
{
public:
    int state = 0;
    int tIndexX, tIndexY = 0;
    SDL_Rect rectStruct;

    Node(int &iIndexX, int &iIndexY, const int &iCanvasSizeX, const int &iCanvasSizeY, int iArraySizeX, int iArraySizeY, int &yButtonSize);
};

Node :: Node(int &iIndexX, int &iIndexY, const int &iCanvasSizeX, const int &iCanvasSizeY, int iArraySizeX, int iArraySizeY, int &yButtonSize)
{
    tIndexX = iIndexX;
    tIndexY = iIndexY;
    rectStruct.w = iCanvasSizeX / iArraySizeX;
    rectStruct.h = (iCanvasSizeY - yButtonSize) / iArraySizeY;
    rectStruct.x = tIndexX * rectStruct.w;
    rectStruct.y = tIndexY * rectStruct.h + yButtonSize;
}

// struct for a grid of array size NxM
template <class A, class B, size_t M, size_t N>
struct Grid
{
    array<array<A, M>, N> tRectArray {};
    array<array<B*, M>, N> tNumbersArray {};
};


//________________class for producing a stream of 2D arrays________________
template <class A, size_t M, size_t N>
class DataStream
{
public:
    array<array<A, M>, N> dataArray{};

    // The constructor creates an NxM array 
    DataStream(array<array<A, M>, N> &iWriteArray);

    // I am returning the array because I want copies of the original array for each key/value pair
    array<array<A, M>, N> writeToArray(array<array<A, M>, N> &iArray, array<array<A, M>, N> &iWriteArray);
};

template <class A, size_t M, size_t N>
DataStream<A, M, N> :: DataStream(array<array<A, M>, N> &iWriteArray)
{
    dataArray = writeToArray(dataArray, iWriteArray);
}

template <class A, size_t M, size_t N>
array<array<A, M>, N> DataStream<A, M, N> :: writeToArray(array<array<A, M>, N> &iArray, array<array<A, M>, N> &iWriteArray)
{
    iArray = iWriteArray;
    return iArray;
}


//________________class for neural network________________
class NeuralNetwork
{
private:
        vector<unsigned int> topology;
public:
        // constructor
        NeuralNetwork(vector<unsigned int> topology, Scalar learningRate = Scalar(0.005));

        // forward propagation
        void propagateForward(RowVector& input);
        // back propagation of errors made by neurons

        void propagateBackward(RowVector& output);
        // function to calculate errors made by neurons in each layer

        void calcErrors(RowVector& output);
        // function to update the weights of connections

        void updateWeights();

        // function to train the neural network give an array of data points
        void train(vector<RowVector*> input_data, vector<RowVector*> output_data);

        vector<RowVector*> neuronLayers; // stores the different layers of out network
        vector<RowVector*> cacheLayers; // stores the unactivated (activation fn not yet applied) values of layers
        vector<RowVector*> deltas; // stores the error contribution of each neurons
        vector<Matrix*> weights; // the connection weights itself
        Scalar learningRate;
};


NeuralNetwork :: NeuralNetwork(vector<unsigned int> topology, Scalar learningRate)
{
        string temp = "";
        
        this->topology = topology;
        this->learningRate = learningRate;
        for (unsigned int i = 0; i < topology.size(); i++) {
                // initialize neuron layers
                if (i == topology.size() - 1)
                neuronLayers.push_back(new RowVector(topology[i]));
                else
                neuronLayers.push_back(new RowVector(topology[i] + 1));
        
                // initialize cache and delta vectors
                cacheLayers.push_back(new RowVector(neuronLayers.size()));
                deltas.push_back(new RowVector(neuronLayers.size()));
        
                // vector.back() gives the handle to recently added element
                // coeffRef gives the reference of value at that place
                // (using this as we are using pointers here)
                if (i != topology.size() - 1) {
                neuronLayers.back()->coeffRef(topology[i]) = 1.0;
                cacheLayers.back()->coeffRef(topology[i]) = 1.0;
                }
        
                // initialize weights matrix
                if (i > 0) {
                if (i != topology.size() - 1) {
                        weights.push_back(new Matrix(topology[i - 1] + 1, topology[i] + 1));
                        weights.back()->setRandom();
                        weights.back()->col(topology[i]).setZero();
                        weights.back()->coeffRef(topology[i - 1], topology[i]) = 1.0;
                }
                else {
                        weights.push_back(new Matrix(topology[i - 1] + 1, topology[i]));
                        weights.back()->setRandom();
                }
                }
        }
};


void NeuralNetwork :: propagateForward(RowVector& input)
{
    // set the input to input layer
    // block returns a part of the given vector or matrix
    // block takes 4 arguments : startRow, startCol, blockRows, blockCols
    neuronLayers.front()->block(0, 0, 1, neuronLayers.front()->size() - 1) = input;
 
    // propagate the data forawrd
    for (unsigned int i = 1; i < topology.size(); i++) {
        // already explained above
        (*neuronLayers[i]) = (*neuronLayers[i - 1]) * (*weights[i - 1]);
    }
 
    // apply the activation function to your network
    // unaryExpr applies the given function to all elements of CURRENT_LAYER
    for (unsigned int i = 1; i < topology.size() - 1; i++) {
        neuronLayers[i]->block(0, 0, 1, topology[i]).unaryExpr(ptr_fun(activationFunction));
    }
}


void NeuralNetwork :: calcErrors(RowVector& output)
{
    // calculate the errors made by neurons of last layer
    (*deltas.back()) = output - (*neuronLayers.back());
 
    // error calculation of hidden layers is different
    // we will begin by the last hidden layer
    // and we will continue till the first hidden layer
    for (unsigned int i = topology.size() - 2; i > 0; i--) {
        (*deltas[i]) = (*deltas[i + 1]) * (weights[i]->transpose());
    }
}


void NeuralNetwork :: updateWeights()
{
    // topology.size()-1 = weights.size()
    for (unsigned int i = 0; i < topology.size() - 1; i++) {
        // in this loop we are iterating over the different layers (from first hidden to output layer)
        // if this layer is the output layer, there is no bias neuron there, number of neurons specified = number of cols
        // if this layer not the output layer, there is a bias neuron and number of neurons specified = number of cols -1
        if (i != topology.size() - 2) {
            for (unsigned int c = 0; c < weights[i]->cols() - 1; c++) {
                for (unsigned int r = 0; r < weights[i]->rows(); r++) {
                    weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
                }
            }
        }
        else {
            for (unsigned int c = 0; c < weights[i]->cols(); c++) {
                for (unsigned int r = 0; r < weights[i]->rows(); r++) {
                    weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
                }
            }
        }
    }
}


void NeuralNetwork :: propagateBackward(RowVector& output)
{
    calcErrors(output);
    updateWeights();
}


void NeuralNetwork :: train(vector<RowVector*> input_data, vector<RowVector*> output_data)
{
    for (unsigned int i = 0; i < input_data.size(); i++) {
        cout << "Input to neural network is : " << *input_data[i] << endl;
        propagateForward(*input_data[i]);
        cout << "Expected output is : " << *output_data[i] << endl;
        cout << "Output produced is : " << *neuronLayers.back() << endl;
        propagateBackward(*output_data[i]);
        cout << "MSE : " << sqrt((*deltas.back()).dot((*deltas.back())) / deltas.back()->size()) << endl;
    }
}


//________________User functions________________
void readCSV(string filename, vector<RowVector*>& data)
{
    data.clear();
    ifstream file(filename);
    string line, word;
    // determine number of columns in file
    getline(file, line, '\n');
    stringstream ss(line);
    vector<Scalar> parsed_vec;
    while (getline(ss, word, ','))
    {
        parsed_vec.push_back(Scalar(stof(&word[0])));
    }
    unsigned int cols = parsed_vec.size();
    data.push_back(new RowVector(cols));
    for (unsigned int i = 0; i < cols; i++) {
        data.back()->coeffRef(1, i) = parsed_vec[i];
    }
 
    // read the file
    if (file.is_open()) {
        while (getline(file, line, '\n')) {
            stringstream ss(line);
            data.push_back(new RowVector(1, cols));
            unsigned int i = 0;
            while (getline(ss, word, ','))
            {
                data.back()->coeffRef(i) = Scalar(stof(&word[0]));
                i++;
            }
        }
    }
}


void genData(string filename)
{
    ofstream file1(filename + "-in");
    ofstream file2(filename + "-out");
    for (unsigned int r = 0; r < 1000; r++) {
        Scalar x = rand() / Scalar(RAND_MAX);
        Scalar y = rand() / Scalar(RAND_MAX);
        file1 << x << ", " << y << endl;
        file2 << 2 * x + 10 + y << endl;
    }
    file1.close();
    file2.close();
}


// Produce dictionary of Datastream objects
template <class typeKey, class typeValue, class A, size_t M, size_t N>
map<typeKey, typeValue> translator(string &iString)
{
    map<typeKey, typeValue> oDictKeyboard = {};

    cout << "Please enter a valid keyboard character: " << endl;

    array<array<A, M>, N> tArray{};
    
    for (unsigned int i = 0; i < iString.size(); i++)
    {
        char* tCharArray = &iString[0];
        oDictKeyboard.insert({tCharArray[i], new DataStream <A, M, N> (tArray)});
    }

    for (auto i = oDictKeyboard.begin(); i != oDictKeyboard.end(); ++i) {
        // i -> second -> printArray(i -> second -> dataArray);
    }

    return oDictKeyboard;
}


template <class typeKey, class typeValue, class A, size_t M, size_t N>
Grid <Node*, A, M, N> SDL_Canvas(bool &iTrain, char &iChar, array<array<A, M>, N> &iInputArray, map<typeKey, typeValue> &idictDatastream)
{
    bool isRunning = true;
    Grid <Node*, A, M, N> oInputGrid;

    int xWindow, yWindow;
    int xMouse, yMouse;
    int xIndexNode, yIndexNode;
    const int cHeight = 600, cWidth = 800;
    int xButtonSize = cWidth, yButtonSize = cHeight/20;
    int xNodeWidth = cWidth/lengthArray(oInputGrid.tRectArray[0]), yNodeWidth = (cHeight - yButtonSize)/lengthArray(oInputGrid.tRectArray);

    // Code to define the expected results for each data entry
    if (iTrain)
    {
        // TODO - Here is the solution which was arrived at from the below discussions:

        // We will create a dict with keyboard character as keys and array of expected values as values. Then the expected arrays will be subject to noise only near state = 1 nodes.
        // This will ensure that the letter "L" subject to noise does not deviate much from the look of a letter L.
        // All these arrays will still be classified as "L" but be 1000s of combinations of L looking images
        // I will need to do this for all the characters on the keyboard

        // TODO - The objective is to change some inputs to return a lower checksum... Inputs would be the weights to the nodes that draw the letter/image in question
        // FIXME - I need to think about how I want to train the NN (requires expected result and training data):
        // Using canvas input from user drawing and I can create the training dataset using a custom noise function
        // Create thousands of arrays that look similar enough to the letter "L"
        // FIXME - I can also try to take the user interactive approach and say that every iteration of the NN requires feedback from the user.
        // The NN only prompts the user if the certainty of the result is below 80% for example
        // This would mean that there is a lot of time at the beginning until the NN is trained
        // FIXME - I can also try to take the user interactive approach and say that every iteration of the NN requires feedback from the user.
        // Another alternative is to produce random (targeted randomness based on the letter) images on canvas and have the user classify a few
        // This is similar to how google asks what is a crosswalk, bus, car, streetlight from human input

        // FIXME - I need to think about what the objective function looks like:
        // Take the sum of the nodes that aren't equal to the target

        // FIXME - This project could begin to get really big if I don't define some constraints on what the user can draw
        // This is based on how well I have trained the NN. Ex) user inputs a smiley so the NN might predict its the letter "L" because its the closest to what it was trained with
    }

    for (int i = 0; i < lengthArray(oInputGrid.tRectArray); i++)
    {
        for (int j = 0; j < lengthArray(oInputGrid.tRectArray[i]); j++)
        {
            oInputGrid.tRectArray[i][j] = new Node (j, i, cWidth, (cHeight - yButtonSize), lengthArray(oInputGrid.tRectArray[i]), lengthArray(oInputGrid.tRectArray), yButtonSize);
        }
    }

    // pointer at each entry in tRectArray
    for (int i = 0; i < lengthArray(oInputGrid.tRectArray); i++)
    {
        for (int j = 0; j < lengthArray(oInputGrid.tRectArray[i]); j++)
        {
            oInputGrid.tNumbersArray[i][j] = &oInputGrid.tRectArray[i][j] -> state;
        }
    }

    // cout << "____________________" << endl;
    // cout << &oInputGrid.tRectArray[0][0] -> state << endl;
    // cout << &oInputGrid.tNumbersArray[0][0] << endl;
    // cout << oInputGrid.tRectArray[0][0] -> state << endl;
    // cout << oInputGrid.tNumbersArray[0][0] << endl;
    // cout << "********************" << endl;
    // oInputGrid.tRectArray[0][0] -> state = 1;
    // cout << &oInputGrid.tRectArray[0][0] -> state << endl;
    // cout << &oInputGrid.tNumbersArray[0][0] << endl;
    // cout << oInputGrid.tRectArray[0][0] -> state << endl;
    // cout << *oInputGrid.tNumbersArray[0][0] << endl;

    // FIXME - I need to printArray the input array (Node) and the keyboard (DataStream) arrays

    string sWindowDesc = "Please draw this keyboard character: ";
    sWindowDesc.push_back(iChar);
    sWindowDesc.append("   Note: Left mouse click to draw, Right mouse click to erase");
    char * cWindowDesc = &sWindowDesc[0];
    SDL_Window *window = SDL_CreateWindow(cWindowDesc, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cWidth, (cHeight - yButtonSize), SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Rect greenButton, redButton;

    SDL_Init(SDL_INIT_EVERYTHING);

    // printArray() function to return the array values after having drawn to them
    // consider defaulting the arrays in case you dont want to draw them all the time

    // FIXME - this requires a new external library
    // Text for buttons
    // TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
    // SDL_Color White = {255, 255, 255};
    // SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White);
    // SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    // SDL_Rect Message_rect; //create a rect
    // Message_rect.x = 0; Message_rect.y = 0; Message_rect.w = 100; Message_rect.h = 100;
    // SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    while (isRunning)
    {
            // Set canvas background colour
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderClear(renderer);
            
            // Create buttons
            greenButton.x = 0; greenButton.y = 0; greenButton.w = cWidth; greenButton.h = yButtonSize;
            redButton.x = 0; redButton.y = cHeight - yButtonSize; redButton.w = cWidth; redButton.h = yButtonSize;
            SDL_SetRenderDrawColor(renderer, 34,139,34, 255);
            SDL_RenderFillRect(renderer, &greenButton);
            SDL_SetRenderDrawColor(renderer, 255, 40, 0, 255);
            SDL_RenderFillRect(renderer, &redButton);

            // Create the grid on canvas
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            for (int i = 0; i < lengthArray(oInputGrid.tRectArray); i++)
            {
                for (int j = 0; j < lengthArray(oInputGrid.tRectArray[i]); j++)
                {
                    if (oInputGrid.tRectArray[i][j] -> state == 0)
                    {
                        SDL_RenderDrawRect(renderer, &oInputGrid.tRectArray[i][j] -> rectStruct);
                    }
                    else
                    {
                        SDL_RenderFillRect(renderer, &oInputGrid.tRectArray[i][j] -> rectStruct);
                    }
                }
            }

            while (SDL_PollEvent(&event))
            {
                    switch (event.type)
                    {
                    // If close window
                    case SDL_QUIT:
                        isRunning = false;
                        break;

                    // If keypress
                    case SDL_KEYDOWN:
                        // If keypress was "Esc"
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                                isRunning = false;
                        }
                    // If mouse click
                    case SDL_MOUSEBUTTONDOWN:
                        SDL_GetWindowPosition(window, &xWindow, &yWindow);
                        SDL_GetGlobalMouseState(&xMouse, &yMouse);
                        
                        // FIXME - update this for the buttons
                        xIndexNode = (xMouse - xWindow) / xNodeWidth;
                        yIndexNode = (yMouse - yWindow) / yNodeWidth;

                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            if ((yMouse - yWindow) < yButtonSize)
                            {
                                cout << "Peeppeeee" << endl;
                                // Top button pressed
                            }
                            else if ((yMouse - yWindow) >= (cHeight - yButtonSize))
                            {
                                cout << "Pooppoooo" << endl;
                                // Bottom button pressed
                            }
                            else
                            {
                                // program in an exit case when red yButtonSize
                                bool redButton = false, greenButton;
                                oInputGrid.tRectArray[yIndexNode][xIndexNode] -> state = 1;
                            }
                        }

                        if (event.button.button == SDL_BUTTON_RIGHT)
                        {
                            oInputGrid.tRectArray[yIndexNode][xIndexNode] -> state = 0;
                        }
                    }
            }

            // Draw renderer values to canvas
            SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return oInputGrid;
}


template <class A, class B, size_t M, size_t N>
int compareArray(array<array<A, M>, N> &iInputArray1, array<array<B, M>, N> &iInputArray2)
{
    int oCheckSum = 0;

    for (int i = 0; i < lengthArray(iInputArray1); i++)
    {
        for (int j = 0; j < lengthArray(iInputArray1[0]); j++)
        {
            if (iInputArray1[i][j] -> state != iInputArray2[i][j])
            {
                oCheckSum ++;
            }
        }
    }

    return oCheckSum;
}


template <class A, size_t M, size_t N>
void noise(int &iPercent, array<array<A, M>, N> &iInputArray)
{
    int randNumN, randNumE, randNumS, randNumW;
    int max = 100, min = 0;

    for (int i = 0; i < lengthArray(iInputArray); i++)
    {
        for (int j = 0; j < lengthArray(iInputArray[0]); j++)
        {
            if (iInputArray[i][j] -> state == 1)
            {
                // Pseudo random based on input percent
                randNumN = rand()%((max - iPercent)-min + 1) + min;
                randNumE = rand()%((max - iPercent)-min + 1) + min;
                randNumS = rand()%((max - iPercent)-min + 1) + min;
                randNumW = rand()%((max - iPercent)-min + 1) + min;

                // North node (not based on canvas coords)
                if (i > 0)
                {
                    if (randNumN == 0)
                    {
                        iInputArray[i-1][j] -> state == 1;
                    }
                }
                // East node (not based on canvas coords)
                if (j < lengthArray(iInputArray[0]) - 1)
                {  
                    if (randNumE == 0)
                    {
                        iInputArray[i][j+1] -> state == 1;
                    }
                }
                // South node (not based on canvas coords)
                if (i < lengthArray(iInputArray) - 1)
                {
                    if (randNumS == 0)
                    {
                        iInputArray[i+1][j] -> state == 1;
                    }
                }
                // West node (not based on canvas coords)
                if (j > 0)
                {
                    if (randNumW == 0)
                    {
                        iInputArray[i][j-1] -> state == 1;
                    }
                }
            }
        }
    }
}


// void draw
//             SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//             for (int i = 0; i < lengthArray(oInputGrid.tRectArray); i++)
//             {
//                 for (int j = 0; j < lengthArray(oInputGrid.tRectArray[i]); j++)
//                 {
//                     if (oInputGrid.tRectArray[i][j] -> state == 0)
//                     {
//                         SDL_RenderDrawRect(renderer, &oInputGrid.tRectArray[i][j] -> rectStruct);
//                     }
//                     else
//                     {
//                         SDL_RenderFillRect(renderer, &oInputGrid.tRectArray[i][j] -> rectStruct);
//                     }
//                 }
//             }

template <class A, size_t M, size_t N>
void printArray(array<array<A, M>, N> &iArray)
{
    for (int i = 0; i < lengthArray(iArray); i++)
    {
        for (int j = 0; j < lengthArray(iArray[0]); j++)
        {
            cout << *iArray[i][j] << ",";
        }
        cout << endl;
    }
    cout << "\n";
}
