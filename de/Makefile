# It builds the de/ project.


all:
# Client-side GUI applications
	make -C apps/
	make -C apps/browser/
# Display server and libraries
	make -C ds/

clean:
# Client-side GUI applications
	-rm apps/bin/*.BIN
	-rm apps/browser/bin/*.BIN
# Display server and libraries
	-rm ds/server/bin/*.BIN

clean-all:
# Client-side GUI applications
	-rm apps/bin/*.BIN
	-rm apps/browser/bin/*.BIN
# Display server and libraries
	-rm ds/server/bin/*.BIN
