# First, copy compiled binary to package build dir
cp build/esp-unlock package/esp-unlock-0.1/

# Go to package build dir, create package, come back
cd package/esp-unlock-0.1/
dpkg-buildpackage -rfakeroot
cd ../../
