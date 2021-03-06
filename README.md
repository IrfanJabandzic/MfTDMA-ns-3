### Mf-TDMA module for ns-3

This repository hosts an implementation of the Multi-frequency Time Division Multiple Access (Mf-TDMA) for the NS-3 network simulator. It comes pre-packaged with NS-3 version 3.27. This module was implemented and tested on Ubuntu 16.04.

This module inclused support for both static and dynamic slot allocation. Dynamic slot allocation is based on decentralized approach.

### Installation and usage instructions
* Clone the project from git. For Ubuntu run:
```
git clone https://github.com/IrfanJabandzic/MfTDMA-ns-3.git
```
* Follow the instructions on https://www.nsnam.org/wiki/Installation to prepare all dependencies. For Ubuntu, basic dependecies are:
```
sudo apt-get update
sudo apt-get install gcc g++ python python-dev
sudo apt-get install mercurial python-setuptools valgrind
```
* Change into MfTDMA-ns-3 directory.
* Configure waf by running ns3_update script:
```
./ns3_update
```

## Running the simulation

1. Open ns3_run script and modify parameters at the start of the script according to the user requirements.
2. Run ./ns3_run n, where n is the number of parallel simulations to be executed
3. After simulations are finished, run ./ns3_analyze n, where n is the number of simulations to be analyzed.
4. Run ./ns3_plot n (OPTIONAL), where n is the number of simulations.
5. Run ./analyze_slots n (OPTIONAL), where n is the simulation number.  

All results are generated in 'results' folder.
