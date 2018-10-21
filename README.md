# SECRET
![Status](https://img.shields.io/badge/Version-Experimental-green.svg)
![License](https://img.shields.io/cran/l/devtools.svg)

## Restriction

This is the AWS F1 version of Insider system. Since F1 does not open the partial reconfiguration interface to users, this version does not support dynamically reload the in-storage accelerators. Anytime when you wish to switch to a new accelerator, you need to reprogram the whole FPGA.

## Preliminaries

### AWS EC2

You typically needs two EC2 instances: 

  1. Used for code compilation and FPGA binary generation. This could be any type of EC2 instance as long as it has enough VCPU and RAM to run Xilinx Vivado for FPGA synthesis. For example, you can choose c4.4xlarge.

  2. Used for running the Insider system. This should be an FPGA instance, for example, f1.2xlarge.

For both instances, you should use the FPGA developer AMI, which can be found at the AWS Marketplace when you launching a new instance. There are two caveats.

  1. Make sure you choose the AMI version as 1.3.3. It could be found at the panel of "FPGA Developer AMI" via `Previous versions -> Continue to Configuration`. The reason behinds this is that the newer version of AMI adopts more recent Vivado HLS tool, which has significant lower performance in synthesizing some of our kernels.
  
  2. Make sure that you configure the compilation instance to have more than 500 GB storage.
  
In order to save your time, you could follow the instructions below to set up the compilation instance first. After that you could copy the drive volumn (via creating a snapshot and then creating a volumn based on the snapshot in the EC2 web console) to the FPGA instance so that you don't need redo all the steps. 

### AWS FPGA Toolchain

First, you need to clone the latest AWS FPGA repository.
```
[~]$ git clone https://github.com/aws/aws-fpga.git
[~]$ cd aws-fpga; git checkout v1.3.4
```
Next, add the following commands into the rc file of your shell. For example, if you use bash, please append the followings into ~/.bashrc.
```
AWS_FPGA_DIR=The absolute path of your cloned repository
source $AWS_FPGA_DIR/sdk_setup.sh
source $AWS_FPGA_DIR/hdk_setup.sh
```
After that please logout your ssh terminal and re-login. The initialization would then be executed (It would be slow for the first time since it needs to download and build related files). 

### Linux Kernel

To reflect the performance in the latest system, we adapt the Insider drivers to the latest Linux version. Most of Linux versions that are >= 4.14.0 should work. However, the default Linux version of AWS FPGA AMI is 3.10.0. You should update the kernel version manually.
```
[~]$ sudo rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
[~]$ sudo rpm -Uvh http://www.elrepo.org/elrepo-release-7.0-3.el7.elrepo.noarch.rpm
[~]$ sudo yum --enablerepo=elrepo-kernel install kernel-ml
[~]$ sudo yum --enablerepo=elrepo-kernel -y swap kernel-headers -- kernel-ml-headers
[~]$ sudo yum install --skip-broken --enablerepo=elrepo-kernel kernel-ml-devel
[~]$ sudo grub2-mkconfig -o /boot/grub/grub.conf
[~]$ sudo grub2-set-default 0
```
Finally reboot your system and verify that your kernel version is >= 4.14.0.
```
[~]$ uname -r
```

### Boost

Install the boost library.
```
$ sudo yum install boost-devel
```

### LLVM & Clang

Some functionality of Insider compiler is implemented based on LLVM and Clang, which should be built first.
```
$ sudo yum install cmake3 svn
$ cd PATH_TO_LLVM
$ svn co http://llvm.org/svn/llvm-project/llvm/trunk llvm
$ cd llvm/tools; svn co http://llvm.org/svn/llvm-project/cfe/trunk clang
$ cd PATH_TO_LLVM; mkdir build; cd build; cmake3 PATH_TO_LLVM/llvm
$ make -j16 # Replace 16 with the number of cores of your instance
```

## BUILD & Installation

The build and installation of Insider is easy. First, you need to set the environment variable `LLVM_SRC_PATH` to the path of the llvm source, and set `LLVM_BUILD_PATH` to the path of the llvm build folder. After that, execute the `install.sh` script.
```
$ export LLVM_SRC_PATH=PATH TO THE LLVM SOURCE
$ export LLVM_BUILD_PATH=PATH TO THE LLVM BUILD FOLDER
$ chmod a+x install.sh; ./install.sh
```
Finally, please logout and relogin. 

## Usage

### Compiling Device Code

Caveat: you should perform all the compilation related stuff at the compilation instance to save your cost (since the FPGA instance is expensive!).

We provide six applications in the respository, whose source code are located at apps folder. Their device code are located at `apps/device`. Take "grep" for example, first execute insider device compiler to generate an STAccel project folder.
```
$ cd apps/device/grep
$ insider_device_compiler
```
Next, use STAccel toolchain to generate a AWS-compatible RTL project folder. It will generate a report of the pipeline performance of each kernel. Make sure that every kernel has final II (Initiation Interval) equals to one, which means that the kernel will be reinvoked every clock cycle (and achieve the best performance). Thus, if II equals two, the performance will degrade into the half.
```
$ cd staccel
$ staccel_syn
```
Then, You can simply follow the [standard process in AWS F1](https://github.com/aws/aws-fpga/tree/master/hdk#step2) to continue. Simply invoke the script to trigger the Vivado suite to synthesize the project.
```
$ project/build/scripts/aws_build_dcp_from_cl.sh
```
You can track the progress via tailing the log file. The synthesis usually takes about three hours. 
```
$ tail -f project/build/scripts/last_log
```
After that, update your tarball (locates at project/build/checkpoints/to_aws) into AWS S3 and start AFI creation. This corresponds the [step 3](https://github.com/aws/aws-fpga/tree/master/hdk#step3) in the AWS F1 tutorial above. The device code compilation ends here; you don't need to go beyond step 3 in their tutorial. You can use the following command to check whether the image generation has finished.
```
$ aws ec2 describe-fpga-images --fpga-image-ids AFI_ID
```

### Compiling Host Code

The host code is located at `apps/host`. For each application, we provide two version of host code: the offloading version and the pure-cpu version. Take "grep" for example, you will find `apps/host/grep/src/offload` and `apps/host/grep/src/pure_cpu`. The pure-cpu version can be directly compiled normally via `g++`. The offloading version should be compiler via `insider_host_g++` or `insider_host_gcc` depending whether it's written in C++ or C. For the "grep" case, you should invoke the following command:

```
$ cd apps/host/grep/src/offload
$ insider_host_g++ -O3 grep.cpp -o grep
```
If you see error message like `relocation truncated to fit:`, please append the compilation flag `-mcmodel=medium`.

### Configuring Drive Parameters

All system parameters are listed at `/driver/nvme/const.h`.

|Parameters|Formats|Comments|
|-----------|---------|--------|
|HOST_DELAY_CYCLE_CNT|(X)|Instructs X delaying cycles at the interconnection (note that <br/>setting X to zero does not mean the interconnection delay <br/>would be zero since it has the inherent hardware delay).|
|HOST_READ_THROTTLE_PARAM|((X << 16) \| Y)|Throttles the interconnection read bandwidth into X / (X + Y) <br/>of the original unthrottled value.|
|HOST_WRITE_THROTTLE_PARAM|((X << 16) \| Y)|Ditto, but throttles the write bandwidth.|
|DEVICE_DELAY_CYCLE_CNT|(X)|Instructs X delaying cycles at the internal storage (note that <br/>setting X to zero does not mean the interconnection delay <br/>would be zero since it has the inherent hardware delay).|
|DEVICE_READ_THROTTLE_PARAM|((X << 16) \| Y)|Throttles the internal storage read bandwidth into X / (X + Y)<br/> of the original unthrottled value.|
|DEVICE_WRITE_THROTTLE_PARAM|((X << 16) \| Y)|Ditto, but throttles the write bandwidth.|

For example, you can set the following values:
```
...
#define HOST_READ_THROTTLE_PARAM ((9 << 16) | 13)
...
#define DEVICE_DELAY_CYCLE_CNT (2500)
...
```

### Executing

All the commands in the section should be performed at the FPGA instance, i.e., AWS F1.
First, use `load_image.sh` in this repository to install your previously generated image via its `AGFI_ID`.
```
$ ./load_image.sh AGFI_ID
```
If there's no error message (there will be some log which is fine), you will find a 64GiB-size Insider drive is mounted at `/mnt`. 
Now you can use Linux tools like `fio` to check the drive bandwidth and latency. You iteratively tune the drive parameters until they meet your requirement.

Before executing the host program, we first need to prepare the input data (which is served as the raw real file, and the offloading version would create the virtual file based on that). Take "grep" for example, now you can use the data generator provided in `apps/host/grep/data_gen`.
```
$ cd apps/host/grep/data_gen
$ ./compile.sh
$ ./data_gen # This step takes some time since we generate near 60GiB-size input.
$ cp grep_input.txt /mnt
```
Now you can run the host program. You can run the offloading version and the pure-cpu version separately to see the performance difference.

### C Simulation

### C-RTL Co-Simulation
