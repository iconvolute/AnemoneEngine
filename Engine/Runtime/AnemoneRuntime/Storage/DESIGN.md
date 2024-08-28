# Storage System Design

## Overview

Storage system is responsible for handling the file system operations.

## Native File System

Engine uses native file system to access files on the local machine.

## File Manager

File manager is responsible for accessing files using mount points. It allows to access files and directories using preconfigured filesystem layout.

Top level directory in file manager allows to access all mount points

## Mount points

The mount point is a directory or container that is mounted to the storage system. Multiple mount points can be attached to specific directory in file manager, allowing to create virtual filesystem layout with overlayed directories. Mount points can access only files and directories in the specified path on native file system. Recursive mount points are not allowed - that is, a file from one mount point cannot be mounted as a separate mount point.

## Readers and writers

Both native file system and file manager allows to access files using readers and writers.

The `DataReader` and `DataWriter` classes provides abstraction for reading data. Implementations of these classes are responsible for reading and writing data from the storage.

### Native File System support

The `FileDataReader` and `FileDataWriter` classes provide implementation for reading and writing data from the `FileHandle` class.

### Memory reader and writer

The `MemoryDataReader` allows to read data from the memory buffer. The `MemoryDataWriter` allows to write data to the memory.

### File Manager support

File manager allows to access files using mount point specific data readers and writers.

- native file system mount point will use native `FileDataReader` and `FileDataWriter` classes to access storage.
- resource package mount point will use `ResourcePackageDataReader` and `ResourcePackageDataWriter` classes to access storage, with support for compressed data.
- archive mount point will allow to access files packed in zip/7z files


### Scenarios

#### Accessing 

- The `FileSystemMountPoint` class allows to access files in the specified path.
- The `ResourcePackageMountPoint` class allows to access files in the resource package.


## Things to implement
- [ ] file watcher API
- [ ] mount points API
