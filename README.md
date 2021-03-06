Pajdeg
======

[![CI Status](http://img.shields.io/travis/kallewoof/PajdegCore.svg?style=flat)](https://travis-ci.org/kallewoof/PajdegCore)

`Pajdeg` is a C library for modifying existing PDF documents by passing them through a stream with tasks assigned based on object ID's. [Check out the documentation.](http://kallewoof.github.io/pajdeg/)

Note: Pajdeg is now available as a pod in CocoaPods. Check out pod "[PajdegCore](https://github.com/kallewoof/PajdegCore)" for the C library version, and "[PajdegPDF](https://github.com/kallewoof/PDObC)" for the Objective-C wrapped version!

Typical usage involves three things:

1. Setting up a `PDPipe` with in- and out paths.
2. Adding filters and/or mutator `PDTask`s to the pipe.
3. Executing.

Tasks can be chained together, and appended to the stream at any time through other tasks, with the caveat that the requested object has not yet passed through the stream.

## Wrappers

`Pajdeg` has wrappers in other languages to make things more convenient.

- [PDObC] (http://github.com/kallewoof/PDObC) (Objective-C)

## Getting Started

- [Download Pajdeg](https://github.com/kallewoof/pajdeg/archive/master.zip)
- Check out samples directory
- [Read Quick Start](https://github.com/kallewoof/pajdeg/wiki/Quick-Start)
