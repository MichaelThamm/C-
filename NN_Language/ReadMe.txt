Requirements:

This could take 30 minutes for initial set up

Must use windows (Linux commands are similar - requires research)
You must have Visual Studio Code Installed
System Properties You Should Know:
- (Choose Windows or Linux)
- (Choose Processor Achitecture)


***********START HERE***********

Learn From?
-----------SCRATCH-----------
	To create a workspace (dev environment):

	https://www.youtube.com/watch?v=0HD0pqVtsmw&t=0s
	https://www.youtube.com/watch?v=rgCJbsCSARM&t=815s

	Setting up your own external libs:

			Getting SDL2 to work:

			https://giovanni.codes/setup-sdl2-with-visual-studio-code-and-mingw64-on-windows/

			More Linking Libs:

			https://stackoverflow.com/questions/10358745/how-to-use-libraries
			https://stackoverflow.com/questions/52910102/vscode-c-task-json-include-path-and-libraries


-----------NOT-SCRATCH-----------
	That youtube video was a warmup for learning to set up a VS Code dev environments.
	If you want to instead use my template dev environment:

	Unzip the C++_NeuralNetwork.zip file to a preferred location where your own dev environment will be located.

	Watch this video to install a compiler (mingw):

	http://mingw-w64.org/doku.php/download
	https://www.youtube.com/watch?v=0HD0pqVtsmw&t=0s

	Copy the folder path where the C++_NeuralNetwork folder was unziped as "C++_NeuralNetwork"
	Open VSCode and go to:
	1) File>Open Workspace  -  open the <Your Folder>\C++_NeuralNetwork\NN_Language\NeuralNet.code-workspace file
	2) In C++_NeuralNetwork .vscode\c_cpp_properties.json change line 18 from:
			"compilerPath": "<My Folder>/msys64/mingw64/bin/g++.exe",
			to:
			"compilerPath": "<Your Folder>/msys64/mingw64/bin/g++.exe",
	3) In C++_NeuralNetwork tasks.json change line 7 from:
			"command": "<My Folder>/msys64/mingw64/bin/g++.exe",
			to:
			"command": "<Your Folder>/msys64/mingw64/bin/g++.exe",
	4) In C++_NeuralNetwork tasks.json change from:
			"-I<My Folder>C++_NeuralNetwork/NN_Language/src/eigen-3.3.9",
			"-I<My Folder>C++_NeuralNetwork/NN_Language/src/SDL2-2.0.14/x86_64-w64-mingw32/include",
			"-L<My Folder>C++_NeuralNetwork/NN_Language/src/SDL2-2.0.14/x86_64-w64-mingw32/lib",
			to:
			"-I<Your Folder>C++_NeuralNetwork/NN_Language/src/eigen-3.3.9",
			"-I<Your Folder>C++/NN_Language/src/SDL2-2.0.14/x86_64-w64-mingw32/include",
			"-L<Your Folder>C++_NeuralNetwork/NN_Language/src/SDL2-2.0.14/x86_64-w64-mingw32/lib",
	5) In C++_NeuralNetwork launch.json change line 18 from:
			"miDebuggerPath": "<My Folder>/msys64/mingw64/bin/gdb.exe",
			to:
			"miDebuggerPath": "<Your Folder>/msys64/mingw64/bin/gdb.exe",
	6) In VSCode go to:
	Run>Start Debugging


The #include main.h statement pulls all the class and function definitions from the main.h header file.
This cleans up the main.cpp code.
For more info: https://www.youtube.com/watch?v=C2cfz7klsQc


If you are having a hard time:
https://www.youtube.com/watch?v=rgCJbsCSARM&t=815s


If it worked congrats!
-Expected result is some data dump to the terminal and a tab should open with a GUI of a red screen
-The C++_NeuralNetwork folder is large in size due to the external libs included (Eigen & SDL2)
	If you want a barebones environment or do not need these, remove them from the terminal execution and the ./src folder


TODO for a fun starter project:

// Create a neural network that displays the value you typed on your keyboard on a GUI
// The array will be a combinations of 1s and 0s or chars to draw or make letters from the data stream
// Solver values being a real number between 0 and 1 allow for colour mapping to a GUI like SDL2