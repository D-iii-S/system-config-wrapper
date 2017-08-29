# System Config Wrapper

A small utility to override values returned by the sysconf system call.
Expects environment variables to describe what value to override.

`SCW<number>=<value> LD_PRELOAD=system-config-wrapper.so <command>`

Configuration numbers are defined as enum values in `/usr/include/bits/confname.h`.
These are likely pretty stable because change would break existing binaries.

## Example: Override Processor Count

To make the `sysconf` call report 256 processors:

`SCW84=256 LD_PRELOAD=system-config-wrapper.so <application>`

## Example: Override Page Size

To make the `sysconf` call report 512 byte pages:

`SCW30=512 LD_PRELOAD=system-config-wrapper.so <application>`
