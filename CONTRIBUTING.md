# Contributing to Xeus-Octave

Xeus and Xeus-Octave are subprojects of Project Jupyter and subject to the
[Jupyter governance](https://github.com/jupyter/governance) and
[Code of conduct](https://github.com/jupyter/governance/blob/master/conduct/code_of_conduct.md).

## General Guidelines

For general documentation about contributing to Jupyter projects, see the
[Project Jupyter Contributor Documentation](https://jupyter.readthedocs.io/en/latest/contributor/content-contributor.html).

## Community

The Xeus team organizes public video meetings. The schedule for future meetings and
minutes of past meetings can be found on our
[team compass](https://jupyter-xeus.github.io/).

## Setting up a development environment

First, you need to fork the project. Then setup your environment:

```bash
# create a new conda environment
mamba create -n xeus-octave -f environment-dev.yml
mamba activate -n xeus-octave

# download xeus-octave from your GitHub fork
git clone https://github.com/<your-github-username>/xeus-octave.git
```

Development tools such as a C++ compiler, CMake, *etc.* are provided in the `environment-dev.yml`
dependency file.

## Building and installing Xeus-Octave

```bash
# Generate the project configuration files with Cmake in "build" directory
cmake -B build \
    -D CMAKE_BUILD_TYPE=Release  \
    -D CMAKE_PREFIX_PATH="${CONDA_PREFIX}" \
    -D CMAKE_INSTALL_PREFIX="${CONDA_PREFIX}"
# Build xeus-octave
cmake --build build --parallel 2
# Install xeus-octave
cmake --install build
```

## Running the tests
To run Python tests, from the build directory, type

```bash
python -m pytest test
```
