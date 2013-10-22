Create a debian package:

Go to build directory and call:
cpack -G DEB

Install package:
sudo dpkg --install Repetier-Server-0.50.1-Linux.deb

Remove package:
sudo dpkg --remove repetier-server
