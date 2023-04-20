# Clova Lamp Opensource 

# How to build

0. Setup build environment
   - please refer to https://source.android.com/source/initializing for the build environment.
   - setup repo
     ```
     mkdir ~/bin
     PATH=~/bin:$PATH
     curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
     chmod a+x ~/bin/rep
     ```
  
1. Get Android open source from CodeAurora and Clova Lamp kernel
   - android version: 7.1.2  
   ```
   repo init -u git://codeaurora.org/platform/manifest.git -b release -m LA.BR.1.2.9-03910-8x09.0.xml --repo-url=git://codeaurora.org/tools/repo.git
   ```

2. Download the kernel
   ```
   git clone https://github.com/clovadevice/clovalamp.git
   ```

3. Build
   ```
   cd kernel
   make ARCH=arm if_s1200n_defconfig
   make mrproper
   make bootimage   
   ```

# License
Please see licenses/NOTICE.html for the full open source license.
