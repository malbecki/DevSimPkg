# DeviceSimPkg

## Introduction

DeviceSimPkg is a package aimed at providing an environment to write OS-executable unit tests for code that interacts directly with devices.
This code is referred to as driver code below although code under test doesn't have to follow UEFI driver model.

## How to use it

You can refer to UnitTest modules to see the usage details. In general you will need to link to package libraries (instead of the ones normally provided by MdePkg/MdeModulePkg) and initialize your device mock. Details of initialization will vary based on which REGISTER_SPACE_MOCK implementer library you will
choose. Please refer to corresponding libraries Readmes for details.

## General architecture

On high-level package consists of:

* REGISTER_SPACE_MOCK - interface between device model and driver code
* REGISTER_SPACE_MOCK wrappers - libraries that wrap REGISTER_SPACE_MOCK and present it as a standard UEFI library
* REGISTER_SPACE_MOCK implementers - libraries that implement REGISTER_SPACE_MOCK interface

## Wrapper libraries

Currently package implements following wrappers:

* IoLib in MockIoLib
* PciSegmentLib in MockPciSegmentLib
* PCI_IO_PROTOCOL in MockPciIoLib

## REGISTER_SPACE_MOCK implementations

Currently only a single implementation is fully supported - LocalRegisterSpaceMockLib. Please see its Readme for more details on how to use it.
