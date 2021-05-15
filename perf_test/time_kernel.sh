echo -e "a.out"
time ./a.out 10000000
echo -e "\n\napp-test"
time ./app-test 10000000
echo -e "\n\nkernel-module"
time ./kernel-test 10000000