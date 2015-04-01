Service Routing Layer - SRL
===========================

Version 1.0.0  
Anthony Wertz - awertz@cmu.edu  
Carnegie Mellon University - Auton Lab

Description
-----------

This library was developed as a generic interface layer for services deployed by
CMU for the DARPA MEMEX program. Note, however, that this library does not need
to be used directly to use the services we deploy.

Building
--------

This project is mainly C++ with a Python interface module. To build everything
just run

```
mkdir build && cd build
cmake .. && make
```

There are some CMake options which are documented in the build script (just run
`cmake -LH ..`).

By default a sample C++ server and the Python SRL package will be built. Running
`make install` will install the `cpp-server` application in the search path.
(`/usr/local/bin` on \*nix machines, `C:/Program Files/srl/samples` on Windows.)

Using the C++ Library
---------------------

Running `make install` will place the compiled library and the header files in
the search path. For an example of how to use it, refer to the `cpp-server`
sample application. What's currently not present is an example (or documentation)
of how to build a C++ service. This can be either deduced by looking at the
library source, inferred by looking at the TAD service implemented in Python,
or you can wait until documentation is updated. :-)

Using the Python Library
------------------------

The CMake build process will build a `pip` installable package (assuming setup
tools are available on the platform) by calling `python setup.py sdist` in the
python interface directory (`python/SRL`). To use it, you can `pip install`
that package, either globally or in a virtual environment, e.g. on Ubuntu:

```
virtualenv env
. env/bin/activate
pip install python/SRL/dist/*.tar.gz
```

For an example using the library to create a service or a client, see the
documentation in the TAD project.

License
-------

The MIT License (MIT)

Copyright (c) 2015 Carnegie Mellon University Auton Lab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
