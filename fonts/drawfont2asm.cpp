// g++ -g -O0 -Wall -Wextra drawfont2asm.cpp -o drawfont2asm -lX11 && ./drawfont2asm |tee x.s && as x.s && ods a.out |v

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

struct {
    int width, height;
    Display *display;
    int screen;
    Window root;
    Window window;
    unsigned long black_pixel;
    unsigned long white_pixel;
    GC gc;
    XFontStruct *font;
    int fontw, fonth;
} text_box;

/* Connect to the display, set up the basic variables. */

static void x_connect()
{
    text_box.display = XOpenDisplay(NULL);
    if (!text_box.display) {
        fprintf(stderr, "Could not open display.\n");
        exit(1);
    }
    text_box.screen = DefaultScreen(text_box.display);
    text_box.root = RootWindow(text_box.display, text_box.screen);
    text_box.black_pixel = BlackPixel(text_box.display, text_box.screen);
    text_box.white_pixel = WhitePixel(text_box.display, text_box.screen);
}

/* Create the window. */

static void create_window()
{
    text_box.width = 300;
    text_box.height = 300;
    text_box.window = XCreateSimpleWindow(text_box.display, text_box.root,
        1 /*x*/, 1 /*y*/, text_box.width, text_box.height, 0 /* border width */,
        text_box.white_pixel /* border pixel */,
        text_box.black_pixel /* background */
    );
    XSelectInput(text_box.display, text_box.window, ExposureMask | KeyPressMask);
    XMapWindow(text_box.display, text_box.window);
}

/* Set up the GC (Graphics Context). */

static void set_up_gc()
{
    text_box.screen = DefaultScreen(text_box.display);
    text_box.gc = XCreateGC(text_box.display, text_box.window, 0, 0);
    XSetBackground(text_box.display, text_box.gc, text_box.black_pixel);
    XSetForeground(text_box.display, text_box.gc, text_box.white_pixel);
}

/* Set up the text font. */

static void set_up_font()
{
    const char *fontname = "w-8x13"; //"-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*";
    text_box.font = XLoadQueryFont(text_box.display, fontname);
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (!text_box.font) {
        fprintf(stderr, "unable to load font %s: using fixed\n", fontname);
        text_box.font = XLoadQueryFont(text_box.display, "fixed");
    }
    XSetFont(text_box.display, text_box.gc, text_box.font->fid);

    text_box.fontw = text_box.font->max_bounds.width;
    text_box.fonth = text_box.font->max_bounds.ascent + text_box.font->max_bounds.descent;
    printf("# font %dx%d\n", text_box.fontw, text_box.fonth);

    text_box.width = 16 * text_box.fontw;
    text_box.height = 16 * text_box.fonth;
    XResizeWindow(text_box.display, text_box.window, text_box.width, text_box.height);
}

/* Draw the window. */

static void draw_screen()
{
    static int done = 0;

    XClearWindow(text_box.display, text_box.window);

    for (int i = 0; i < 256; i++) {
        char c[2] = { char(i), '\0' };
        XDrawString(text_box.display, text_box.window, text_box.gc,
            i % 16 * text_box.fontw, i / 16 * text_box.fonth + text_box.font->max_bounds.ascent, c, 1);
    }

    if (!done) {
        done = 1;

        char fname[64] = {};
        sprintf(fname, "font%dx%d", text_box.fontw, text_box.fonth);

        XImage *xi = XGetImage(text_box.display, text_box.window, 0, 0,
            text_box.width, text_box.height, 0xffffffff, ZPixmap);
        printf("# XGetImage() %dx%d @%d format=%d data=%p byte_order=%d bitmap_unit=%d "
                "bitmap_bit_order=%d bitmap_pad=%d depth=%d bytes_per_line=%d "
                "bits_per_pixel=%d rm=%lx gm=%lx bm=%lx obdata=%p\n",
                xi->width, xi->height, xi->xoffset, // number of pixels offset in X direction
                xi->format,                // XYBitmap, XYPixmap, ZPixmap
                xi->data,                  // (char *) pointer to image data
                xi->byte_order,            // data byte order, LSBFirst, MSBFirst
                xi->bitmap_unit,           // quant. of scanline 8, 16, 32
                xi->bitmap_bit_order,      // LSBFirst, MSBFirst
                xi->bitmap_pad,            // 8, 16, 32 either XY or ZPixmap
                xi->depth,                 // depth of image
                xi->bytes_per_line,        // accelerator to next scanline
                xi->bits_per_pixel,        // bits per pixel (ZPixmap)
                xi->red_mask,              // bits in z arrangement
                xi->green_mask,
                xi->blue_mask,
                xi->obdata                 // hook for the object routines to hang on
        );

        printf("	.data\n");
        printf("	.global %s\n", fname);
        printf("%s:\n", fname);
        for (int c = 0; c < 256; c++) {
            int x0 = c % 16 * text_box.fontw;
            int y0 = c / 16 * text_box.fonth;
            printf("#%sc%d:\n", fname, c);
            for (int y = 0; y < text_box.fonth; y++) {
                printf("	.byte 0b");
                const char *p = &xi->data[(y0 + y) * xi->bytes_per_line];
                for (int x = 0; x < text_box.fontw; x++)
                    printf("%d", p[(x0 + x) * xi->bits_per_pixel/8] ? 1 : 0);
                printf("\n");
            }
            printf("\n");
        }
        printf("	.size %s, . - %s\n", fname, fname);
        printf("	.data\n");
        printf("	.global %ss\n", fname);
        printf("%ss:\n", fname);
        for (int c = 0; c < 256; c++) {
            int x0 = c % 16 * text_box.fontw;
            int y0 = c / 16 * text_box.fonth;
            printf("#%ss%d:\n", fname, c);
            for (int y = 0; y < text_box.fonth; y++) {
                printf("	.ascii \"");
                const char *p = &xi->data[(y0 + y) * xi->bytes_per_line];
                for (int x = 0; x < text_box.fontw; x++)
                    printf("%c", p[(x0 + x) * xi->bits_per_pixel/8] ? '@' : ' ');
                printf("\"\n");
            }
            printf("\n");
        }
        printf("	.size %ss, . - %ss\n", fname, fname);
    }
}

/* Loop over events. */

static void event_loop()
{
    while (1) {
        XEvent e;
        XNextEvent(text_box.display, &e);
        if (e.type == Expose) {
            draw_screen();
        } else if (e.type == KeyPress) {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    x_connect();
    create_window();
    set_up_gc();
    set_up_font();
    event_loop();
    return 0;
}
