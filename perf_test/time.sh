echo -e "a.out"
time ./a.out 1000000
echo -e "\n\napptest"
time ./app-test 1000000
echo -e "\n\npreload-nocache"
time LD_PRELOAD=./agent.so ./a.out 1000000
echo -e "\n\npreload-withcache"
time LD_PRELOAD=./agent-cache.so ./a.out 1000000