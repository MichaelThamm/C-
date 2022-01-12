using namespace std;
SDL_Event event;


// function for length of an array
template <class T, size_t size>
int lengthArray(const array<T, size> &iArray)
{
    return iArray.size();
}


//________________class for nodes in the grid________________
class Node
{
public:
    int state = 0;
    int tIndexX, tIndexY = 0;
    SDL_Rect rectStruct;

    Node(int &iIndexX, int &iIndexY, const int &iCanvasSizeX, const int &iCanvasSizeY, int &yButtonSize);
};

Node :: Node(int &iIndexX, int &iIndexY, const int &iNodeSizeX, const int &iNodeSizeY, int &yButtonSize)
{
    tIndexX = iIndexX;
    tIndexY = iIndexY;
    rectStruct.w = iNodeSizeX;
    rectStruct.h = iNodeSizeY;
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
Grid <Node*, A, M, N> SDL_Canvas(bool &iTrain, char &iChar, array<array<A, M>, N> &iInputArray, map<typeKey, typeValue> &idictDatastream, bool &iRedButton)
{
    bool bIsRunning = true;
    bool bGreenButton = false;
    Grid <Node*, A, M, N> oInputGrid;

    int xWindow, yWindow;
    int xMouse, yMouse;
    int xIndexNode, yIndexNode;
    const int cHeight = 600, cWidth = 800;
    int xButtonSize = cWidth, yButtonSize = cHeight/20;
    int xNodeSize = cWidth/lengthArray(oInputGrid.tRectArray[0]), yNodeSize = (cHeight - 2*yButtonSize)/lengthArray(oInputGrid.tRectArray);

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

    // Initialize RectArray with Node objects
    for (int i = 0; i < lengthArray(oInputGrid.tRectArray); i++)
    {
        for (int j = 0; j < lengthArray(oInputGrid.tRectArray[i]); j++)
        {
            oInputGrid.tRectArray[i][j] = new Node (j, i, xNodeSize, yNodeSize, yButtonSize);
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

    string sWindowDesc = "Please draw this keyboard character: ";
    sWindowDesc.push_back(iChar);
    sWindowDesc.append("   Note: Left mouse click to draw, Right mouse click to erase");
    char * cWindowDesc = &sWindowDesc[0];
    SDL_Window *window = SDL_CreateWindow(cWindowDesc, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cWidth, cHeight, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Rect greenButton, redButton;

    SDL_Init(SDL_INIT_EVERYTHING);

    // FIXME - Use SDL_test_font & SDL_test_images

    // Text for buttons
    // TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 24);
    // SDL_Color White = {255, 255, 255};
    // SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "put your text here", White);
    // SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    // SDL_Rect Message_rect; //create a rect
    // Message_rect.x = 0; Message_rect.y = 0; Message_rect.w = 100; Message_rect.h = 100;
    // SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    while (bIsRunning && !iRedButton && !bGreenButton)
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
                        bIsRunning = false;

                    // If keypress
                    case SDL_KEYDOWN:
                        // If keypress was "Esc"
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            bIsRunning = false;
                        }
                    // If mouse click
                    case SDL_MOUSEBUTTONDOWN:
                        SDL_GetWindowPosition(window, &xWindow, &yWindow);
                        SDL_GetGlobalMouseState(&xMouse, &yMouse);

                        // Clicked in grid
                        if ((yButtonSize < (yMouse - yWindow)) && ((yMouse - yWindow) < (cHeight - yButtonSize)))
                        {
                            xIndexNode = (xMouse - xWindow) / xNodeSize;
                            yIndexNode = (yMouse - yWindow - yButtonSize) / yNodeSize;

                            if (event.button.button == SDL_BUTTON_LEFT)
                            {
                                oInputGrid.tRectArray[yIndexNode][xIndexNode] -> state = 1;
                            }

                            if (event.button.button == SDL_BUTTON_RIGHT)
                            {
                                oInputGrid.tRectArray[yIndexNode][xIndexNode] -> state = 0;
                            }
                        }

                        // Green button pressed
                        else if (yButtonSize >= (yMouse - yWindow))
                        {
                            if (event.button.button == SDL_BUTTON_LEFT)
                            {
                                bGreenButton = true;
                            }
                        }

                        // Red button pressed
                        else if ((yMouse - yWindow) >= (cHeight - yButtonSize))
                        {
                            if (event.button.button == SDL_BUTTON_LEFT)
                            {
                                iRedButton = true;
                            }
                        }

                        else
                        {
                            // Nothing
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
array<array<int, M>, N> noise(int &iPercent, array<array<A, M>, N> &iInputArray)
{
    int randNumN, randNumE, randNumS, randNumW;
    int max = 100, min = 0;
    array<array<int, M>, N> coppiedArray {};
    
    // copy input array contents into coppiedArray
    for (int i = 0; i < lengthArray(iInputArray); i++)
    {
        for (int j = 0; j < lengthArray(iInputArray[0]); j++)
        {
            coppiedArray[i][j] = *iInputArray[i][j];
        }
    }

    for (int i = 0; i < lengthArray(iInputArray); i++)
    {
        for (int j = 0; j < lengthArray(iInputArray[0]); j++)
        {
            if (*iInputArray[i][j] == 1)
            {
                // Pseudo random based on input percent
                randNumN = rand()%(max - min + 1) + min;
                randNumE = rand()%(max - min + 1) + min;
                randNumS = rand()%(max - min + 1) + min;
                randNumW = rand()%(max - min + 1) + min;
                cout << randNumN << ", " << randNumE << ", " << randNumS << ", " << randNumW << endl;
                // North node (not based on canvas coords)
                if (i > 0)
                {
                    if (randNumN <= iPercent)
                    {
                        *iInputArray[i-1][j] = 1;
                    }
                }
                // East node (not based on canvas coords)
                if (j < lengthArray(iInputArray[0]) - 1)
                {  
                    if (randNumE <= iPercent)
                    {
                        *iInputArray[i][j+1] = 1;
                    }
                }
                // South node (not based on canvas coords)
                if (i < lengthArray(iInputArray) - 1)
                {
                    if (randNumS <= iPercent)
                    {
                        *iInputArray[i+1][j] = 1;
                    }
                }
                // West node (not based on canvas coords)
                if (j > 0)
                {
                    if (randNumW <= iPercent)
                    {
                        *iInputArray[i][j-1] = 1;
                    }
                }
            }
        }
    }

    return coppiedArray;
}


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
