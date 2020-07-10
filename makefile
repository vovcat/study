SRC=asm1.cbp asm1c.cpp asm1.cpp asm1.depend asm1.layout asm1s.S font8x13.s star.bmp wx_all.cpp wxasm.cpp wx.cpp

all: cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe asm1 asm1.exe
all+: cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe asm1 asm1.exe

cvq_lin: cvq_lin.cpp
	g++ -g -O0 -Wall -Wextra cvq_lin.cpp -o cvq_lin -lpthread #&& ./cvq_lin
cvq_win.exe: cvq_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mwindows -static-libgcc -static-libstdc++ cvq_win.cpp -o cvq_win.exe #&& wine ./cvq_win.exe
cv_win.exe: cv_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mwindows -static-libgcc -static-libstdc++ cv_win.cpp -o cv_win.exe #&& wine ./cv_win.exe
wx_all: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti wx_all.cpp wxasm.cpp -o wx_all -lX11 -lXext -lpthread #&& ./wx_all
wx_all.exe: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc -static-libstdc++ wx_all.cpp wxasm.cpp -o wx_all.exe -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi #&& wine ./wx_all.exe
wx: wx.cpp font8x13.s wxasm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti wx.cpp font8x13.s wxasm.cpp -o wx -lX11 -lXext -lpthread #&& ./wx
wx.exe: wx.cpp font8x13.s wxasm.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc -static-libstdc++ wx.cpp font8x13.s wxasm.cpp -o wx.exe -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi #&& wine ./wx.exe
asm1: wx.cpp font8x13.s asm1s.S asm1.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti wx.cpp font8x13.s asm1s.S asm1.cpp -o asm1 -lX11 -lXext -lpthread
asm1.exe: wx.cpp font8x13.s asm1s.S asm1.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc wx.cpp font8x13.s asm1s.S asm1.cpp -o asm1.exe -lgdi32 -lws2_32 -lwinmm -lavrt -ldwmapi
test_asm: test_asm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_asm.cpp -o test_asm
test_usleep: test_usleep.cpp
	g++ -g -O0 -Wall -Wextra test_usleep.cpp -o test_usleep
test_usleep.exe: test_usleep.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ test_usleep.cpp -o test_usleep.exe -lws2_32 -lwinmm -lavrt -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic

dw:
	diff -wu wx_all.cpp wx.cpp ||:
g:
	egrep -r $(S) /usr/share/mingw-w64/include/ /usr/i686-w64-mingw32/include/ /usr/lib/gcc/i686-w64-mingw32/*/include*

clean:
	rm -f cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe asm1 asm1.exe

vars =	$(foreach v, $(filter-out .VARIABLES vars, $(.VARIABLES)),	\
		$(if $(filter-out environment, $(origin $(v))),		\
			$(info [$(origin $(v))] $(v) = $($(v)))		\
		)							\
	)

MNTd=/mnt/win/deti/d
MNTs=/mnt/win/fs/d
DIR=vmproj/asm1

put:
	mount $(MNTd)
	-cp -p -t "$(MNTd)/$(DIR)" $(SRC)
	umount $(MNTd)
	mount $(MNTs)
	-cp -p -t "$(MNTs)/$(DIR)" $(SRC)
	umount $(MNTs)
getd:
	mount $(MNTd)
	-cd "$(MNTd)/$(DIR)" && cp -p -t "$(CURDIR)" $(SRC)
	umount $(MNTd)
gets:
	mount $(MNTs)
	-cd "$(MNTs)/$(DIR)" && cp -p -t "$(CURDIR)" $(SRC)
	umount $(MNTs)
