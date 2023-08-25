# ESP-Unlock Host Application

This GTK-3-based application queries and displays OTPs from an ESP-Unlock hardware token and runs the [ESP-Unlock firmware](https://github.com/0xjakob/esp-unlock).

## Status

This app is pretty much a proof-of-concept. It should work if you do not do something unusual. But it has a lot of loose screws and many errors are not checked. There is no refined design here, either, because the author is not really familiar with building GUI applications. Please keep this in mind when using this application and checking the code. This, however, should not discourage you from trying this application. Any feedback, comments, suggestions are welcome. Feel free to open a GitHub issue or pull request in that case.

## Requirements

* Currently, only Linux is supported.
* A recent C++ compiler with C++20 support.
* libgtk-3 and related libraries (see CMakeLists.txt for more details).
* libbaseencode (development version for building)

## Build

Note: You have two possibilities to get access to the *ESP-Unlock*:
1. Add yourself to the group `dialout` and change DEVICE in `gtk_app.cpp` to `/dev/ttyACM<n>`.
2. Install the [udev rules file](package/esp-unlock-0.1/99-esp-unlock.rules), which automatically creates a link `/dev/esp-unlock` with non-root access rights. This is the default setting in `gtk_app.cpp`.

Afterwards, just build like any other CMake project:
```
mkdir build
cd build
cmake ../
make -j <logical_cpu_cores_plus_2>
```

## Run

No special arguments are required to run the application. Please make sure that the application can access the ACM device file (e.g., `/dev/ttyACM0`, compare Build section) or it will not work.

## Debian Package

This repository has a Debian package build script: [`create_package.sh`](create_package.sh). This is pretty ad hoc, though, and currently tries to sign the package with the builder's GPG key.
