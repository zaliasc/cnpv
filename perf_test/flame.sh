perf record --call-graph dwarf bash -c "./a.out 1000000"
perf script > out.perf
stackcollapse-perf.pl out.perf > out.folded
flamegraph.pl out.folded > a.out.svg

perf record --call-graph dwarf  bash -c "./app-test 1000000"
perf script > out.perf
stackcollapse-perf.pl out.perf > out.folded
flamegraph.pl out.folded > app-test.svg

perf record --call-graph dwarf bash -c "LD_PRELOAD=./agent.so ./a.out 1000000"
perf script > out.perf
stackcollapse-perf.pl out.perf > out.folded
flamegraph.pl out.folded > preload.svg