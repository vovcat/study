all: cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe
all+: cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe

cvq_lin: cvq_lin.cpp
	g++ -g -O0 -Wall -Wextra cvq_lin.cpp -o cvq_lin -lpthread #&& ./cvq_lin
cvq_win.exe: cvq_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mwindows -static-libgcc -static-libstdc++ cvq_win.cpp -o cvq_win.exe #&& wine ./cvq_win.exe
cv_win.exe: cv_win.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mwindows -static-libgcc -static-libstdc++ cv_win.cpp -o cv_win.exe #&& wine ./cv_win.exe
wx_all: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti wx_all.cpp wxasm.cpp -o wx_all -lX11 -lXext -lpthread #&& ./wx_all
wx_all.exe: wx_all.cpp wxasm.cpp auto.h debX11.cpp debWin.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc -static-libstdc++ wx_all.cpp wxasm.cpp -o wx_all.exe -lgdi32 -lws2_32 -lwinmm #&& wine ./wx_all.exe
wx: wx.cpp wxasm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti wx.cpp wxasm.cpp -o wx -lX11 -lXext -lpthread #&& ./wx
wx.exe: wx.cpp wxasm.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc -static-libstdc++ wx.cpp wxasm.cpp -o wx.exe -lgdi32 -lws2_32 -lwinmm #&& wine ./wx.exe
test_asm: test_asm.cpp
	g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_asm.cpp -o test_asm #&& ./test_asm
test_usleep: test_usleep.cpp
	g++ -g -O0 -Wall -Wextra test_usleep.cpp -o test_usleep
test_usleep.exe: test_usleep.cpp
	i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ test_usleep.cpp -o test_usleep.exe -lws2_32 -lwinmm -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic

dw:
	diff -wu wx_all.cpp wx.cpp ||:
g:
	egrep -r $(S) /usr/share/mingw-w64/include/ /usr/i686-w64-mingw32/include/ /usr/lib/gcc/i686-w64-mingw32/*/include*

clean:
	rm -f cvq_lin cvq_win.exe cv_win.exe wx_all wx_all.exe wx wx.exe test_asm test_usleep test_usleep.exe
