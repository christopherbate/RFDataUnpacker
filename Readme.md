## RF Data Unpacker 
[![CircleCI](https://circleci.com/gh/christopherbate/RFDataUnpacker.svg?style=svg)](https://circleci.com/gh/christopherbate/RFDataUnpacker)

# Build
`git submodule init && git submodule update --remote && make`

# Test
Normal `make` builds the tests. Run `./tests`. Automatically run by CircleCI on commit.

# Run
Fill out the information in settings.txt. Here's an example:
```
bits 2

channels 2

complex 1

file /media/gnssLive/shortTest/glo.bin

out /media/gnssLive/shortTest/gloL1.bin

out /media/gnssLive/shortTest/gloL2.bin

limit 1000000000000000
```

`file` is the input file, `out` specifies an output file. There must be the same number of `out` declarations as `channels`.

This specification says "the data is packed in I/Q format, with 2 bits per each I and Q, and is alternating 2 different channels.". 

Then run:
`./unpacker`
to extract.

#TODO:
1. Ability to specify different bit mapping. (Current mapping is specified in the code.)
2. Add command line args as an option relative to the settings file.