SRC=asm1.cbp asm1c.cpp asm1.cpp asm1.depend asm1.layout asm1s.S font8x13.s star.bmp wx_all.cpp wxasm.cpp wx.cpp

all: cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe asm1 asm1.exe

cvq_lin: cvq_lin.cpp
	g++ -g -O0 -Wall -Wextra cvq_lin.cpp -o $@ -lpthread #&& ./cvq_lin
cvq_win.exe: cvq_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ cvq_win.cpp -o $@ #&& wine ./cvq_win.exe
cv_win.exe: cv_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ cv_win.cpp -o $@ #&& wine ./cv_win.exe
wx_all: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) wx_all.cpp wxasm.cpp -o $@ -lX11 -lXext -lpthread #&& ./wx_all
wx_all.exe: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) -mwindows -static-libgcc -static-libstdc++ wx_all.cpp wxasm.cpp -o $@ -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi #&& wine ./wx_all.exe
wx: wx.cpp font8x13.s wxasm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) wx.cpp font8x13.s wxasm.cpp -o $@ -lX11 -lXext -lpthread #&& ./wx
wx.exe: wx.cpp font8x13.s wxasm.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) -mwindows -static-libgcc -static-libstdc++ wx.cpp font8x13.s wxasm.cpp -o $@ -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi #&& wine ./wx.exe
asm1: wx.cpp font8x13.s asm1s.S asm1.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) wx.cpp font8x13.s asm1s.S asm1.cpp -o $@ -lX11 -lXext -lpthread
asm1.exe: wx.cpp font8x13.s asm1s.S asm1.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) -mwindows -static-libgcc wx.cpp font8x13.s asm1s.S asm1.cpp -o $@ -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi
asm1bench: wx.cpp font8x13.s asm1s.S asm1.cpp asm1bench.cpp
	g++ -g -O2 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) -DMAIN asm1s.S asm1bench.cpp -o $@  -lX11 -lXext -lpthread
test_asm: test_asm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) test_asm.cpp -o $@
test_usleep: test_usleep.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) test_usleep.cpp -o $@
test_usleep.exe: test_usleep.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti $(CFLAGS) -mconsole -static-libgcc -static-libstdc++ test_usleep.cpp -o $@ -lws2_32 -lwinmm -lavrt -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic

dw:
	diff -wu wx_all.cpp wx.cpp ||:
g:
	egrep -r $(S) /usr/share/mingw-w64/include/ /usr/i686-w64-mingw32/include/ /usr/lib/gcc/i686-w64-mingw32/*/include*

clean:
	rm -f cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe asm1 asm1.exe asm1bench

vars =	$(foreach v, $(filter-out .VARIABLES vars, $(.VARIABLES)),	\
		$(if $(filter-out environment, $(origin $(v))),		\
			$(info [$(origin $(v))] $(v) = $($(v)))		\
		)							\
	)

MNTd=/mnt/win/deti/d
MNTr=/mnt/win/reb/d
MNTs=/mnt/win/fs/d
DIR=vmproj/asm1

put: putd putr

putd:
	mount $(MNTd)
	-cp -p -t "$(MNTd)/$(DIR)" $(SRC)
	umount $(MNTd)
putr:
	mount $(MNTr)
	-cp -p -t "$(MNTr)/$(DIR)" $(SRC)
	umount $(MNTr)
puts:
	mount $(MNTs)
	-cp -p -t "$(MNTs)/$(DIR)" $(SRC)
	umount $(MNTs)

getd:
	mount $(MNTd)
	-cd "$(MNTd)/$(DIR)" && cp -p -t "$(CURDIR)" $(SRC)
	umount $(MNTd)
getr:
	mount $(MNTr)
	-cd "$(MNTr)/$(DIR)" && cp -p -t "$(CURDIR)" $(SRC)
	umount $(MNTr)
gets:
	mount $(MNTs)
	-cd "$(MNTs)/$(DIR)" && cp -p -t "$(CURDIR)" $(SRC)
	umount $(MNTs)
