# SECRET
![Status](https://img.shields.io/badge/Version-Experimental-green.svg)
![License](https://img.shields.io/cran/l/devtools.svg)

## Restriction

This is the AWS F1 version of Insider system. Since F1 does not open the partial reconfiguration interface to users, this version does not support dynamically reload the in-storage accelerators. Anytime when you wish to switch to a new accelerator, you need to reprogram the whole FPGA.

## Preliminaries

### AWS EC2

You typically needs two EC2 instances: 

(1) Used for code compilation and FPGA binary generation. This could be any type of EC2 instance as long as it has enough VCPU and RAM to run Xilinx Vivado for FPGA synthesis. For example, you can choose c4.4xlarge.

(2) Used for running the Insider system. This should be a FPGA instance, for example, f1.2xlarge.

For both instances, you should use the FPGA developer AMI, which can be found at the AWS Marketplace when you launching a new instance. Make sure that you configure the compilation instance to have more than 500 GB storage.

### AWS FPGA Toolchain

First, you need to clone the latest AWS FPGA repository.
```
[~]$ git clone https://github.com/aws/aws-fpga.git
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
[~]$ sudo grub2-mkconfig -o /boot/grub/grub.conf
[~]$ sudo grub2-set-default 0
```
Finally reboot your system and verify that your kernel version is >= 4.14.0.
```
[~]$ uname -r
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

The build and installation of Insider is easy. First, you need to set the environment variable LLVM_SRC_PATH to the path of the llvm source, and set LLVM_BUILD_PATH to the path of the llvm build folder. After that, execute the install.sh script.
```
$ export LLVM_SRC_PATH=PATH TO THE LLVM SOURCE
$ export LLVM_BUILD_PATH=PATH TO THE LLVM BUILD FOLDER
$ chmod a+x install.sh; ./install.sh
```
