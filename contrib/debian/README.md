
Debian
====================
This directory contains files used to package puppycoind/puppycoin-qt
for Debian-based Linux systems. If you compile puppycoind/puppycoin-qt yourself, there are some useful files here.

## puppycoin: URI support ##


puppycoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install puppycoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your puppycoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/puppycoin128.png` to `/usr/share/pixmaps`

puppycoin-qt.protocol (KDE)

