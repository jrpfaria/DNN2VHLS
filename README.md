# DNN2VHLS – DNN Architecture to Vitis HLS Descriptions

## 📖 Overview

This repository contains the work developed during my Master's Dissertation.
**DNN2VHLS** is a tool for generating **C++ files** that describe a neural network’s behaviour adapted to the specified architecture and learnable parameters.
It is intended for **FPGA acceleration workflows** (specifically supported by Vitis HLS), where DNN configurations must be defined in C++.

The workflow is:

1. Define your network’s **learnable parameters (LPs)** in `param.csv`.
2. Run the **GUI tool** (`dnn2vhls.py`) and describe the intended architecture.
3. Move the generated files to you Vitis HLS workspace and implement it.
4. Integrate the generated files with your FPGA project.

---

## 📚 Citation

If you use this tool, please cite:

J. R. Faria, F. D. L. Coutinho, and A. S. R. Oliveira,
"Using Automatic Code Generation to Streamline the Real-Time FPGA Deployment of DNNs at the Edge,"
in *Proc. 2025 32nd IEEE Int. Conf. on Electronics, Circuits and Systems (ICECS)*,
Marrakech, Morocco, Nov. 17–19, 2025, doi: 10.1109/ICECS66544.2025.11270598.

GitHub: https://github.com/jrpfaria/DNN2VHLS

---

## 📂 Repository Structure

```
.
├── generator/          # Internal source files for the generator
├── dnn2vhls.py         # GUI tool (requires python3-tk on Ubuntu)
├── generate.sh         # Script used by the GUI to generate the files
├── param.csv           # File specifying the learnable parameters
├── clean.sh            # Utility script to remove generated output files
├── testBench.cpp       # Template file for C Synthesis and C/RTL Co-Simulation in Vitis HLS 
└── README.md           # This file
```

---

## ⚙️ Requirements

* **OS**: Ubuntu (tested)
* **Python**: 3.x
* **Packages**:

  * `python3-tk` (for the GUI)

Install Tkinter if missing:

```bash
sudo apt-get install python3-tk
```

---

## 📝 Usage

### 1. Define Network Parameters (`param.csv`)

The `param.csv` file specifies the **Layer Parameters (LPs)** for your network.
Each row represents a filter, bias or alpha and should be ordered as shown in the file.

* First comes the kernels data, ordered by layer, kernel, and channel
* Following that, the biases, ordered by layer and kernel
* Lastly, the alphas, ordered by layer

This file serves as the input blueprint for the generator.

---

### 2. Run the GUI

Start the generator with:

```bash
python3 dnn2vhls.py
```

The GUI allows you to:

* Describe the model architecture
* Load and validate `param.csv`
* Generate the corresponding C files

---

### 3. Output Files

Generated headers are placed in the project directory.
They contain `#define` macros for kernel weights, biases, and layer configurations, which can be directly included in Vitis HLS code.
The top function should be the `model`

---

### 4. Clean Build

To remove generated files, run:

```bash
./clean.sh
```

---

## 🚀 Quickstart

```bash
# Clone repo
git clone https://github.com/yourusername/dnn2vhls.git
cd dnn2vhls

# Install Tkinter (Ubuntu)
sudo apt-get install python3-tk

# Edit param.csv with your network’s LPs
vim param.csv

# Launch the generator GUI
python3 dnn2vhls.py

# Clean output files (if needed)
./clean.sh
```

---

## 📌 Notes

* The generator itself is implemented in the `generator/` folder. Users normally only interact with:

  * `param.csv`
  * `dnn2vhls.py`
  * `clean.sh`
* Ensure CSV formats are strictly followed, otherwise the generator will reject input.
* Output headers are designed for **direct inclusion in FPGA/HLS projects**.
* Additionally, users can access the `generator/` folder to understand the inner workings of the generator and update it as needed.
---
