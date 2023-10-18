# mca

MODBUS Set Agent :  `a tool to Set Registers on MODBUS server`

Lead/TPO : Guy Francoeur

### Goal and Objectives

This __crossplatform__ **Agent/Tool** runs on edge. The objective is to set registers using the MODBUS protocol.

Fully autonomous tool. Fast, Light, Reliable, Scalable, Easy, Secure.

Siblings : mca MODBUS Collector Agent, mss MODBUS Server Service

Keywords : Online/Incremental capture (telemetry) data from any capable MODBUS assets or devices

- Language : C++
- Architecture : All
- OS : Windows / Linux


### Status

![](https://img.shields.io/static/v1?label=version&message=RC1-final&color=blue)
![](https://img.shields.io/static/v1?label=license&message=MIT&color=orange)
![](https://img.shields.io/badge/language-c%2B%2B-blueviolet)

#### Features
 - [x] Cross-Platform no changes/hardcode in code
	- [x] Linux ARM
	- [x] Linux Intel/AMD64
	- [x] Windows Intel/AMD64
 - [ ] MODBUS ...
	- [x] support signed, unsigned, float