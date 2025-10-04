# traceinvaders2

## Important usage notes:

### Dependencies:
- Dependencies for sketches are all placed in the `/lib/` folder. This is crucial, as Arduino IDE uses this folder to discover libraries for inclusion in sketches. **Dependencies for subsystem modules are handled different, so their include statements will be different (detailed in the sections below).**
- External libraries (driver code from vendors, basically anything not written by us) is to be placed in `/lib/external/`
- Subsystem libraries (code we create for our various subsystems) is to be placed in `/lib/modules/`.
- **Note: Every sketch and subsystem library must include `config.hpp`, otherwise critical pin assignments and settings flags will not be included!** 

### To include a library in a SKETCH:
- Once your subsystem or external library is correctly placed in `/lib`, include the header file using the syntax:
```
//including config.hpp:
#include <config/config.hpp>
//including an external library: 
#include <external/library_name.hpp>
//including a subsystem library:
#include <modules/module_name.hpp>
```

### To include a library in ANOTHER LIBRARY:
- Arduino IDE won't automatically resolve dependencies for these, so we need to use relative paths. There are three types of includes that you might need to use in a subsystem module: the config file, another subsystem, or an external library. Examples of each are listed below.
```
//including config.hpp:
#include "../config/config.hpp"
//including an external library: 
#include "../external/library_name.hpp"
//including a subsystem library:
#include "module_name.hpp"
```

### Test suites:
- When creating a new test suite, place your `.ino` file into a folder in `/test/`. Make sure that the sketch and its containing folder have the exact same name.
