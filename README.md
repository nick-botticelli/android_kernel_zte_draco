# Android Kernel for ZTE Maven 3 (ZTE Z835)
[![CircleCI](https://circleci.com/gh/nick-botticelli/android_kernel_zte_z835.svg?style=svg)](https://circleci.com/gh/nick-botticelli/android_kernel_zte_z835)

# Building
1. Get dependencies (`sudo apt-get install -y build-essential kernel-package libncurses5-dev bzip2`; make sure you
execute `apt update` first)
2. Make sure you have the toolchain (i.e. `git submodule update --init --recursive`)
3. Run `build.sh` script
4. Kernel binary is located at `output/arch/arm/boot/zImage`

# Notes
* The stock kernel source for this device originated from opensource.ztedevices.com (located in `stock` branch).
* The original stock kernel source did not contain some source (e.g. `drivers/soc/qcom/vendor/`). The ZIP with the
complete source is located [here](http://download.ztedevices.com/device/global/support/opensource/2/20200628_01/Z835_Nougat(7.1.1)_kernel(3.10.49).tar.gz).
