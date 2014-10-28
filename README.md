DPA Calc
========

Table of Contents
-----------------

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Compiling DPA Calc](#compiling-dpa-calc)
- [Executing DPA Calc](#executing-dpa-calc)
  - [Input File Structure](#input-file-structure)
  - [Interpreting Output](#interpreting-output)
    - [Initialization](#initialization)
    - [Execution](#execution)
    - [Results](#results)

Introduction
------------

DPA Calc implements the Differential Power Analysis (DPA). DPA allows discovering the secret key stored into secure embedded systems by exploiting the correlation between the power consumed by a device and the data being processed. Currently, DPA Calc is specifically developed for analysing AES, as it is the standard block cipher. However, it can be easily extended to support any block cipher algorithm. 

This repository has two branches:
  - **Eigen-version** - compute the analysis using the computation resources of the available *CPUs*
  - **OpenCL-GPU-version** - compute the analysis using the computation resources of the available *GPUs*

Version
-------

1.0

Dependencies
------------

The compilation of DPA Calc requires the installation of cmake 2.6 or higher. Furthermore, if using the *OpenCL-GPU-version* branch, OpenCL installation, with the C++ wrapper (*cl.hpp*), is also required.    

The rest of the dependencies can be found in the folder dependencies, namely *Eigen, PAPI and Tclap*, and they are not required to be installed.

Compiling DPA Calc
------------------

After having all the dependencies compiled and installed just need to invoke the following command
```sh
make
```
If everything goes fine a binary executable file named *dpacalc* is created in folder *out*.

Executing DPA Calc
------------------

From the main folder, run the following command:  
```sh
./out/dpacalc --filename <Input File> --mlock
```
where 
  - **Input File** is the path of the input file

An example of an execution using the file *t10s5004fk16.dat* as input:
```sh
./out/dpacalc --filename t10s5004fk16.dat --mlock
```

### Input File Structure

The input file is in binary format, where bytes are interpreted as follows:

 - a header with informations about:
    - the number of traces
    - the number of samples per trace
    - the format of each sample (b for int8, f for float, d for double)
    - the length of a plain/cipher text in byte
 - a list of traces with a plain/cipher text attached to each trace

An input example:
```
--- header ---
00000000 00000000 00000000 00000010 [uint32 - 2]
00000000 00000000 00000000 00000011 [uint32 - 3]
01100010 [char - b]
00000010 [uint8 - 2]
--- trace 1 ----
00000100 00000011 00000010 [samples]
XXXXXXXX XXXXXXXX [plain / cipher text]
--- trace 2 ----
00000001 00000011 00000110 [samples]
XXXXXXXX XXXXXXXX [plain / cipher text]
```
### Interpreting Output

The output besides showing the most likely key also shows complementary information about execution of the DPA Calc. This information can be divided in three sections:
 1. Initialization
 2. Execution
 3. Results

#### Initialization

![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

This section starts by displaying the compilation options and the name of the classes to compute each module. Then, it is shown the how many platforms are installed in the used system, describing the platforms with one or more GPUs. Subsequently, a detailed information about each found GPU is presented. Finally, the header of the input file is shown. 

#### Execution

![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

This section is useful in runtime, as it shows the current state of the execution. The states are:
 1. Read Known Data
 2. Create command queues 
   -  create one for each thread created
 3. Create program object
   - Compile and build all kernels for each device
 4. Generate all hypothetical values and map them to power values
 5. Calculate the PM pre computation
 6. Compute the statistical test
   - For each batch is shown
     - the number of the batch
     - the number of the first sample
     - how many samples are in the batch
     - the number of the device that computed the batch
     - the number of the thread that issued the computation of the batch

#### Results

![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

The last section of the output presents the most likely key and the total execution time. In the example of the figure, the execution took *4 seconds*, approximately, and the best hypothesis for the first byte of the key is *2b*. 