echo -e "a.out"
time ./a.out 10000000
echo -e "\n\napptest"
time ./app-test 10000000
echo -e "\n\npreload"
time LD_PRELOAD=./agent.so ./a.out 10000000