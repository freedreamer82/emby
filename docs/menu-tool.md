# Emby Configurator (emby_config_menu.sh)

Path: `emby/emby/misc/tools/emby_config_menu.sh`

Description
----------
`emby_config_menu.sh` is a small interactive shell script that provides a text-based menu (based on `dialog`) to configure build-time macros for the Emby tree. The script collects values for various settings (for example: string pool sizes, console buffer sizes, default login password, etc.) and exports the results to a C header file named `emby_config.h` (by default in the directory where the script is executed).

What it does
------------
- Presents organized menus by category: Strings, System, Console, Containers.
- Allows editing values via input popups.
- Allows toggling options on/off with checklists.
- When choosing "Save Config" the script generates (or overwrites) `emby_config.h` containing `#define` directives for the configured entries.
- Some entries use a special value `deadbeaf` to represent a disabled option; the script handles these cases consistently when writing the header.

Dependencies
------------
- bash (the script is a shell script)
- dialog (provides the text UI for the menus)
- write permission in the working directory to create `emby_config.h`

Ubuntu / Debian (install)
-------------------------
On Ubuntu or Debian-based systems you can install the required package (`dialog`) with apt. `bash` is typically preinstalled.

Run the following commands in a terminal:

```bash
sudo apt update
sudo apt install -y dialog
```

Notes:
- If you plan to run the script in a minimal container or in CI, ensure `dialog` is available; otherwise you can adapt the script to use a non-interactive mode.
- The package `dialog` pulls the required ncurses libraries as dependencies.

Example output
--------------
The script generates an `emby_config.h` file with lines like:

#define EMBY_CFG_STRING_LONG_POOL_SIZE 10
#define EMBY_CFG_CONSOLE_LINE_CURSOR ">"

(String values are quoted in the generated file where appropriate.)

Usage
-----
From the repository root (or the directory where you want `emby_config.h` to be created):

```bash
# make executable (if necessary) and run
chmod +x emby/emby/misc/tools/emby_config_menu.sh
emby/emby/misc/tools/emby_config_menu.sh
```

Notes
-----
- If `dialog` is missing, install it using your distribution package manager (e.g. `sudo apt install dialog`).
- The script uses temporary files in `/tmp` and attempts to clean them up on exit; if the script crashes you may find leftover files with the `emby_config_menu.sh.` prefix.
- Entries marked with the special value `deadbeaf` are treated as "disabled" and are written accordingly in the generated header.

Extending and maintenance
------------------------
Configuration entries are stored in an associative array (`embyconfig`) inside the script. To add a new option:
- add the key and default value to the `embyconfig` array,
- add an item to the appropriate `show_*` menu function,
- handle editing with the `change_value_popup` helper or add a new UI control if needed.

Contact
-------
For questions or feature requests open an issue or PR and include an example of the expected `emby_config.h` output.
