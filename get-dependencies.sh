#!/usr/bin/env bash
set -euo pipefail

# Unity: unit testing framework
# TO-DO: Consider extracting only the C source code, there's a lot of build
# config files contained in the releases...

RELEASE_TARBALL=v2.5.2.tar.gz

cd lib/
curl --remote-name --location https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/${RELEASE_TARBALL}
tar --gunzip --extract --file $RELEASE_TARBALL
# Need to remove the .git/ folder that the Unity v2.5.2 tarball leaves behind;
# - could cause problems if other external libraries are added that also
# contain a .git/ folder
rm -rf .git/ $RELEASE_TARBALL
