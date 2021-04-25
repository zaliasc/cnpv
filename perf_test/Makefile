all :
	gcc test.c -g -o a.out
	gcc test.c -g -o app-test
	$(MAKE) -C ../ns_agent
	cp ../ns_agent/agent.so .
	$(MAKE) clean -C ../ns_agent

test :
	$(MAKE) all
	bash time.sh

clean :
	rm -rf *.data *.folded *.data *.data.old *.perf app-test *.out *.svg *.log *.so