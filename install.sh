if [ -z "$LLVM_SRC_PATH" ]; then
    echo "Need to set LLVM_SRC_PATH"
    exit 1
fi

if [ -z "$LLVM_BUILD_PATH" ]; then
    echo "Need to set LLVM_BUILD_PATH"
    exit 1
fi

orig_path=`pwd`
sudo mkdir /usr/include/staccel
sudo cp STAccel/inc/* /usr/include/staccel
sudo mkdir /usr/staccel/
sudo cp STAccel/template /usr/staccel
g++ STAccel/src/iopinChecker.cpp -std=c++11 -O3 \
    -o staccel_syn
sudo mv staccel_syn /usr/bin
sudo cp STAccel/shell/* /usr/bin

sudo mkdir /usr/include/insider
sudo cp Insider/inc/* /usr/include/insider
g++ Insider/src/insider_reset_syn.cpp -std=c++11 -O3 \
    -o insider_reset_syn
sudo mv insider_reset_syn /usr/bin
sudo cp Insider/shell/* /usr/bin
cd Insider/lib; ./compile.sh; 
sudo mv libinsider_runtime.so /usr/lib64;
cd ../..;

echo -e "\nadd_clang_subdirectory(s2s-kernel)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(s2s-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(csim-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-app)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-interconnect)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
echo "add_clang_subdirectory(insider-cosim-intc)" \
    >> $LLVM_SRC_PATH/tools/clang/tools/CMakeLists.txt
cp -r Insider/llvm/* $LLVM_SRC_PATH/tools/clang/tools
cp -r STAccel/llvm/* $LLVM_SRC_PATH/tools/clang/tools
cd $LLVM_BUILD_PATH
cmake3 $LLVM_SRC_PATH
make -j16
sudo cp bin/insider* /usr/bin
sudo cp bin/staccel* /usr/bin
cd $orig_path
