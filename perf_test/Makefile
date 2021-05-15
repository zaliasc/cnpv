all :
	gcc test.c -g -o a.out
	gcc test.c -g -o app-test
	gcc test.c -g -o kernel-test
	$(MAKE) -C ../ns_agent
	cp ../ns_agent/agent.so .
	$(MAKE) clean -C ../ns_agent

agent :
	$(MAKE) all
	bash time.sh

kernel :
	$(MAKE) all
	bash time.sh	

clean :
	rm -rf *.data *.folded *.data *.data.old *.perf app-test kernel-test *.out *.svg *.log *.so