CC = $(shell which clang gcc | head --lines=1)
CXX = $(shell which clang++ g++ | head --lines=1)
CFLAGS = -W -Wall -Wextra -pedantic -O3 -flto -DNDEBUG

ez80sf.rom: external/fasmg ez80sf.src
	@git submodule update --init --recursive -- external/fasmg-ez80
	$^ $@
ez80sf.src: src/*.inc
	@for file in $(addprefix external/fasmg-ez80/,ez80.inc symbol_table.inc) $^; do \
		echo "include '$$file'"; \
	done >$@

external/fasmg: external/fasmg.zip
	unzip -o $< -d $(@D) $(@F)
	chmod +x $@
external/fasmg.zip:
	wget https://flatassembler.net/fasmg.zip --output-document=$@

check: ez80sf.rom
	@git submodule update --init --recursive -- external/CEmu
	$(MAKE) -C external/CEmu/core CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" libcemucore.a
	@grep "^\w\+: *; *CHECK: *" src/*.inc | while read check; do \
		$(CC) $(CFLAGS) $(LDFLAGS) -I external/CEmu/core test/tester.c \
			external/CEmu/core/libcemucore.a -o test/tester; \
		echo -n "Testing $${check%%:*}..."; \
		test/tester $^ `grep "^$${check%%:*} = " ez80sf.lab | cut -d\  -f3-`; \
	done

clean:
	$(MAKE) -C external/CEmu/core clean
	rm --force --recursive ez80sf.* test/tester
distclean:
	git submodule deinit --all
	rm --force --recursive ez80sf.* test/tester external/fasmg

.INTERMEDIATE: ez80sf.src external/fasmg.zip
.PHONY: check clean distclean
