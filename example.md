# Thundering Herd Example

## Prerequisites
```
git clone https://github.com/lpsmodsim/2022HPCSummer-ThunderingHerd.git
```

# Run the simulation
Use the python driver file within 2022HPCSummer-ThunderingHerd/tests/thunderingHerd.py 

This simulation doesn't currently generate thundering herd, but has the base components needed to do so, given that further work is done to tweak the parameters, and align requests in a way that would cause the thundering herd problem to occur


# Running
This is assuming the user is on a system running a Ubuntu-Based Linux Distro.

Prerequisites
```
sudo apt install singularity black mypi
git clone https://github.com/tactcomplabs/sst-containers.git
```
Follow the instructions in the git repo to build the container "sstpackage-11.1.0-ubuntu-20.04.sif".
```
cp sst-containers/singularity/sstpackage-11.1.0-ubuntu-20.04.sif /usr/local/bin/
git clone https://github.com/lpsmodsim/2022HPCSummer-SST.git
sudo ./2022HPCSummer-SST/additions.def.sh
```

Running the model
```
cd 2022HPCSummer-ThunderingHerd
make > output.txt
```

Re-run the model
```
make clean
make > output.txt
```

Simulation output is generated in 2022HPCSummer-ThunderingHerd/output.txt
