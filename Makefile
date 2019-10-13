project = cli++
version = 0.1.0

targets = cli++ sample

cli++.type = shared

sample.type = executable
sample.libs = color++ cli++

include base.mk
