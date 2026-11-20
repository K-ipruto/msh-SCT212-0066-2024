# Test Plan for `msh`

| Input | Expected behaviour | Actual result |
| :--- | :--- | :--- |
| `(blank line)` | New prompt, no error | Displayed `msh[0]> ` immediately with no error output. |
| `echo hello world` | Prints text; status 0 | Printed `hello world` and updated prompt to `msh[0]> `. |
| `ls /nonexistent` | Error from ls; non-zero status | Printed `ls: cannot access '/nonexistent': No such file or directory` and updated prompt to `msh[2]> `. |
| `nosuchcommand` | Error message; status 127 | Printed `nosuchcommand: No such file or directory` and updated prompt to `msh[127]> `. |
| `cd /tmp` then `pwd` | Prints `/tmp` | Changed directory to `/tmp` (`msh[0]> `) and `pwd` printed `/tmp`. |
| `sleep 3 &` | Prompt returns at once; PID shown | Printed `[bg] PID: <pid>` immediately and returned `msh[0]> `; background job reaped cleanly after completion. |
| `echo hi > f.txt` then `cat f.txt` | File contains `hi` | Created `f.txt` containing `hi`; `cat f.txt` printed `hi` to stdout (`msh[0]> `). |
| `sleep 100` then `Ctrl-C` | Sleep stops; shell survives; status 130 | Foreground `sleep` terminated immediately, `msh` remained active, and prompt updated to `msh[130]> `. |
| `Ctrl-D` at the prompt | Shell exits cleanly | Printed newline and exited cleanly with status `0` and no memory leaks. |

## Additional Custom Tests

| Input | Expected behaviour | Actual result |
| :--- | :--- | :--- |
| `   echo    tab   spaces   ` | Extra spaces and tabs ignored; prints `tab spaces` | Printed `tab spaces` cleanly and returned `msh[0]> `. |
| `cd` (no arguments) then `pwd` | Changes directory to `$HOME` | Changed working directory to `/home/user` and `pwd` confirmed `$HOME`. |
| `echo first > out.txt` then `echo second > out.txt` then `cat out.txt` | Truncates existing file and writes `second` | Overwrote `out.txt` with `second`; `cat out.txt` printed `second` (`msh[0]> `). |
| `exit` | Built-in command terminates shell cleanly | Exited `msh` immediately with the previous command's exit status. |