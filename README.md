# InputRedirectionClient-Qt
Input redirection client for the 3DS using QtGamepad

Supported platforms:

* Windows (via xinput, if you don't have a Xbox controller you should use x360ce)
* Linux (via evdev)
* OSX
* maybe others?

If you have multiple controllers connected at the same time, this software will combine their inputs.

## OSX Installation Instructions
Note that this process requires using the Terminal app and running commands.

1. Install Apple Compiler Toolchain ([learn more here](https://developer.apple.com/documentation/xcode/installing-the-command-line-tools/))
```bash
xcode-select --install
```

2. Install [Homebrew](https://brew.sh/)
```bash
# check if you already have Homebrew:
brew --version

# to install Homebrew:
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

3. Install Qt
```bash
brew install qt@5
```

4. Clone this repository
```bash
# choose somewhere to download the repo (e.g., ~/Downloads)
cd ~/Downloads # or wherever
git clone https://github.com/TuxSH/InputRedirectionClient-Qt.git
cd InputRedirectionClient-Qt
```

5. Compile the Code
For Apple Silicon Mac (M1/M2/M3/M4:
```bash
/opt/homebrew/opt/qt@5/bin/qmake
make
```
For Older Intel-based Mac:
```bash
/usr/local/opt/qt@5/bin/qmake
make
```
This will generate a `InputRedirectionClient-Qt.app`. Note that there may be compile warnings.

6. Run the App

Either double-click `InputRedirectionClient-Qt.app` or in terminal run:
```bash
open InputRedirectionClient-Qt.app
```

Optionally, you can add this to your Applications by dragging `InputRedirectionClient-Qt.app` into your `Applications` folder.
