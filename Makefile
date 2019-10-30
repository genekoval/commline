project = cli++
version = 0.1.0

libcli++.type = shared

cli++.type = executable
cli++.libs = color++ cli++
cli++.deps = libcli++

include base.mk
