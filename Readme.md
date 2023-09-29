# DeviceSimPkg

## Introduction

DeviceSimPkg is a package aimed at providing an environment to write OS-executable unit tests for code that interacts directly with devices.
This code is referred to as driver code below although code under test doesn't have to follow UEFI driver model.

## How to use it

### Getting the package

Recommended way of getting the package is to add it as submodule into your EDK2 repo. This can be easily done with

```
git submodule add https://github.com/malbecki/DeviceSimPkg.git
git add ./
git commit
git submodule update --init
```
when in your EDK2 repo.

### Using the code

You can refer to UnitTest modules to see the usage details. In general you will need to link to package libraries (instead of the ones normally provided by MdePkg/MdeModulePkg) and initialize your device access. Details of initialization will vary based on which REGISTER_ACCESS_INTERFACE implementer library you will
choose. Please refer to corresponding libraries Readmes for details.

## General architecture

On high-level package consists of:

* REGISTER_ACCESS_INTERFACE - interface between device model and driver code
* REGISTER_ACCESS_INTERFACE wrappers - libraries that wrap REGISTER_ACCESS_INTERFACE and present it as a standard UEFI library
* REGISTER_ACCESS_INTERFACE implementers - libraries that implement REGISTER_ACCESS_INTERFACE interface

## Wrapper libraries

Currently package implements following wrappers:

* IoLib in RegisterAccessIoLib
* PciSegmentLib in RegisterAccessPciSegmentLib
* PCI_IO_PROTOCOL in RegisterAccessPciIoLib

## REGISTER_ACCESS_INTERFACE implementations

Currently only a single implementation is fully supported - FakeRegisterSpaceLib. Please see its Readme for more details on how to use it.
