# traceinvaders2

## Important usage notes:

### Dependencies:
- Dependencies for sketches are all placed in the `/lib/` folder. This is crucial, as Arduino IDE uses this folder to discover libraries for inclusion in sketches.
- External libraries (driver code from vendors, basically anything not written by us) is to be placed in `/lib/external/`
- Subsystem libraries (code we create for our various subsystems) is to be placed in `/lib/subsystems/`.
### To include a library:
- Once your subsystem or external library is correctly placed in `/lib`, include the header file using the syntax:
```
#include <external/library_name.hpp>
#include <subsystem/subsystem_name.hpp>
```
- Note: Every sketch MUST include the line `include <config/config.hpp>`, otherwise the sketch will not have access to important pin assignments and flags.

### Test suites:
- When creating a new test suite, place your `.ino` file into a folder in `/test/`. Make sure that the sketch and its containing folder have the exact same name.
