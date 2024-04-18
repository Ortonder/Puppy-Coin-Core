Puppycoin Core
=============

Setup
---------------------
[Puppycoin Core](http://puppycoin.org/wallet) is the original Puppycoin client and it builds the backbone of the network. However, it downloads and stores the entire history of Puppycoin transactions; depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more. Thankfully you only have to do this once.

Running
---------------------
The following are some helpful notes on how to run Puppycoin Core on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/puppycoin-qt` (GUI) or
- `bin/puppycoind` (headless)

### Windows

Unpack the files into a directory, and then run puppycoin-qt.exe.

### macOS

Drag Puppycoin-Qt to your applications folder, and then run Puppycoin-Qt.

### Need Help?

* See the documentation at the [Puppycoin Wiki](https://github.com/Puppycoin-Project/Puppycoin/wiki)
for help and more information.
* Ask for help on [BitcoinTalk](https://bitcointalk.org/index.php?topic=1262920.0) or on the [Puppycoin Forum](http://forum.puppycoin.org/).
* Join our Discord server [Discord Server](https://discord.puppycoin.org)

Building
---------------------
The following are developer notes on how to build Puppycoin Core on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The Puppycoin repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Multiwallet Qt Development](multiwallet-qt.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://www.fuzzbawls.pw/puppycoin/doxygen/)
- [Translation Process](translation_process.md)
- [Unit Tests](unit-tests.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Dnsseed Policy](dnsseed-policy.md)

### Resources
* Discuss on the [BitcoinTalk](https://bitcointalk.org/index.php?topic=1262920.0) or the [Puppycoin](http://forum.puppycoin.org/) forum.
* Join the [Puppycoin Discord](https://discord.puppycoin.org).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
