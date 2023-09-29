# Hello World

In this application, we have two applications `app0` and `app1` that print
`hello, world from <app name>`.

Application `app0` is the initial process, it first setups access to uart,
enabling it to write message, then initializes `app0` by giving it access to
memory, uart and a slice of time on another core.

Application `app1` simply prints `hello, world from app1`.
