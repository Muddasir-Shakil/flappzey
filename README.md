# Zephyr-RTOS blinky template

This repo contains an example template for a new Zephyr project using the T2 (Star topology) where the application is the manifest repository.

The application supplied as example is the original Blinky app found in Zephyr samples folder.

More information on workspaces and supported topologies within a zephyr project can be found in the [Zephyr Project Documentation](https://docs.zephyrproject.org/latest/guides/west/workspaces.html?highlight=topologies)

## Requirements

The instructions bellow are for MacOS.

### ARM GCC Compiler

This zephyr project uses the ARM GCC Compiler and assumes it is installed and the GNUARMEMB_TOOLCHAIN_PATH environment variable is set and pointing to the install folder.

For more information on available toolchains visit the [Zephyr Toolchain Documentation](https://docs.zephyrproject.org/latest/guides/beyond-GSG.html#set-up-a-toolchain)

If another toolchain is used make sure to change the ZEPHYR_TOOLCHAIN_VARIANT variable in CMakeLists.txt

### Python

Python3 is required to use and compile the Zephyr RTOS. 

To install on MacOS:

```
brew install python3
```

## Usage

### Create the workspace

Create a folder for the new workspace. This new workspace will host an app folder where the contents of our repo will be installed as well as a full version of the zephyr-rtos with all the needed submodules for all the supported boards/architectures.


```bash
mkdir our_app_workspace

cd our_app_workspace
```

We only need to create the workspace folder, the rest will be handled by west tool.

```
our_app_workspace/
│
├── app/                   # This is where our repo will be installed
│   ├── CMakeLists.txt
│   ├── prj.conf
│   ├── src/
│   │   └── main.c
│   └── west.yml           # main manifest with optional import(s) and override(s)
│                                    
├── modules/
│   └── lib/
│       └── tinycbor/
│
└── zephyr/
```

### Install west tool

The most convenient way to install west is to create a python virtual environment.

```bash
python3 -m venv .venv
```

Activate the virtual environment.
```bash
source .venv/bin/activate
```

Install the west tool
```bash 
pip install west
```

### Initialize the project

Using west we clone our repo into the app folder and using *west update* we import the rest of the project from the Zephyr repository.

```bash
west init -m <repo_url>

west update
```
### Install python requirements

Install the rest of the python requirements

```bash
pip install -r zephyr/scripts/requirements.txt
```


## Build the blinky example.

With the project completly setup we can test it by building the blinky example provided.

For example to build the project for a STM32F429ZI Nucleo dev board:
 ```bash
 west build -p -b nucleo_f429zi
 ```