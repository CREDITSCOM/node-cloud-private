# node-cloud-private
Private credits blockchain
<h2>Build dependencies</h2>
<ul>
<li><a href="https://www.boost.org/users/history/version_1_70_0.html">Boost 1.70</a> or newest static prebuild</li>
<li>Compiler with C++17 support</li>
<li><a href="https://cmake.org/download/">Cmake 3.11</a> or newest</li>
<li> Requirements fo building <a href="https://thrift.apache.org/docs/install/">Apache Thrift</a></li>
<li>The building Berkeley DB distribution uses the Free Software Foundation's autoconf and libtool tools to build on UNIX platforms.</li>

On Windows:<br/>

It is necessary to run in the terminal, which sets the environment variables for building a Visual Studio project

>```sh
>git clone https://github.com/CREDITSCOM/node-cloud-private.git
>cd node
>git submodule update --init --recursive
>mkdir build
>cd build
>cmake -DCMAKE_BUILD_TYPE=Release -A x64 ..
>cmake --build . --target ALL_BUILD --config Release
On Linux:<br/>
>```sh
>git clone https://github.com/CREDITSCOM/node-cloud-private.git
>cd node
>git submodule update --init --recursive
>mkdir build
>cd build
>cmake -DCMAKE_BUILD_TYPE=Release ..
>make -j4

<h2>System requirements:</h2>
<h4>Minimum system requirements:</h4>
Operating system: Windows® 7 / Windows® 8 / Windows® 10 64-bit (with the last update package)/Linux Ubuntu 18.04 
Processor (CPU): with frequency of 1 GHz (or faster) with PAE, NX and SSE2 support;
Memory (RAM): 8 Gb
HDD: 1 Tb
Internet connection: 3 Mbit/s.
<h4>Recommended system requirements:</h4>

Operating system: Windows® 7 / Windows® 8 / Windows® 10 64-bit (with the last update package)/Linux Ubuntu 18.04 
Processor (CPU): Intel® Core ™ i3 or AMD Phenom ™ X3 8650
Memory (RAM): 8 Gb
HDD: 3 Tb
Internet connection: 5 Mbit/s.
