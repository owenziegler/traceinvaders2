# traceinvaders2

## Important usage notes:

### Setup:
- Clone this repository to a folder named `traceinvaders2` somewhere on your computer.
- In Arduino IDE, go to the top toolbar and select File > Preference or press Ctrl+, to open Settings. The first setting in the list will be your sketchbook location. Change this from the default so it points to the `traceinvaders2` folder you just created.
- When you select the Sketchbook in the side bar (the first icon, depicting a folder), it should now show the main sketch, as well as the the test sketches in the test folder.

### Libraries:
- All external dependencies (code we download from the Internet) and internal dependencies (code we write ourselves for the various subsystems) are located in the `traceinvaders2/libraries` folder. It is crucial that these files live in the `libraries` folder, otherwise Arduino IDE will not recognize them when we include them in the project. As long as the `.hpp` and `.cpp` library are together somewhere in the libraries folder, including them in your sketches is as simple as writing the one line `#include <dependency_name.hpp>`.
- The `config.hpp` file contains all of the pin assignments and config flags that are required for our project to run. Every subsystem module, the main sketch, and all test sketches **must** `#include` this file.

### Test suites:
- When creating a new test suite, place your `.ino` file into a folder in `/test/`. Make sure that the sketch and its containing folder have the exact same name, otherwise Arduino IDE will not recognize it.
