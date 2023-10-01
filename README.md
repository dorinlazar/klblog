# KL Blog engine

For the time being everything is organized as a monorepo, but I'm thinking about actually moving to a more acceptable
split, at least a separation of the kl library which is theoretically reusable.

KLBlog will eventually be an independent blog engine - it should depend for its own code on nothing but the standard
libraries. This might seem like a fool's errand. Everyone taught us that we should reuse. But in this case, reuse is
just shoving corporate code in your own projects. So I'll try to reuse the tools, and fully embrace the Not Invented
Here attitude, against all the rules that corporations keep repeating to us. The point is not to write code fast, but to
be nice to human readers, and to allow them to reason about the code.

## Setup

We have several scripts one can use:

- `tools/setup.sh` will set things up on Fedora
- `tools/build.sh` will build things

Look at the scripts in tools/ to see what other nice things are there.
