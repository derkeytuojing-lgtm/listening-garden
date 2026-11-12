#include "garden.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* ภาพละ 8 บรรทัด วาดจากบนลงล่าง จุดด้านล่างของทุกภาพอยู่ตรงเมล็ด */
static const char *const seed[] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "            ⢀⡤⢤⡀",
    "            ⠻⠤⠞⠁"
};
static const char *const sprout[] = {
    "",
    "",
    "",
    "",
    "",
    "              ⣠⢖⡶",
    "           ⠘⢿⣶⠋⠉",
    "             ⢸"
};
static const char *const sapling[] = {
    "",
    "",
    "         ⣀⣀",
    "         ⢳⡈⠙⢦  ⢀⣠⠤⢤",
    "          ⠙⠲⠬⣇⢠⠋ ⢀⠎",
    "          ⢠⡤⠤⣜⡯⠤⠖⠋",
    "           ⠳⠤⠼⡇",
    "             ⢠⠇"
};
static const char *const branches[] = {
    "",
    "         ⢠",
    "          ⢧⡀ ⡄     ⣠",
    "      ⢀    ⠙⠦⡇   ⣀⠜⠁",
    "      ⠈⠳⡀    ⣇⣠⠤⠚⠁",
    "        ⠈⠳⢤⣀ ⡗",
    "            ⠉⡇",
    "             ⡇"
};
static const char *const young[] = {
    "",
    "           ⡼⠳⡄      ⣀⣀⡀",
    "           ⠹⣄⡽⢀⣤⡀ ⢠⠎⢁⡰⠃",
    "    ⠸⡉⠙⢦⢠⣄⡀ ⠈⢹⢸⡀⢳⢀⡞⠉⠉",
    "     ⠙⠦⠤⢿⡀⠙⡆ ⢸ ⣳⠞⠋",
    "         ⠙⠲⢧⣀⣸⠋⠁",
    "             ⢹",
    "             ⡞"
};
static const char *const canopy[] = {
    "       ⢀⣀⣀⡀⣠⠖⠒⠒⢦⣠⠤⠤⣄",
    "    ⣀⣀⣰⠋  ⠉   ⠰⠋⠉⠁ ⠈⠧⠤⠤⣄",
    "   ⢸⠁     ⠞⠉⠉          ⠈⡆",
    " ⢀⡴⠋  ⣀⣀⡀           ⠞⠉⠉ ⠙⠲⡀",
    " ⠘⢦⡀ ⠘⠁     ⠠⡀  ⢠⠖⠒⠂    ⢀⡴⠁",
    "   ⣇⣀⣀⡀  ⠐⣄  ⢹⡀  ⡴⠊    ⢀⡼",
    "    ⠈ ⠙⠢⠴⠚⠛⠷⡤⢤⣧⣶⠟⠦⢤⠴⠊⠉⠉⠉",
    "            ⢱⠘⢰⠃"
};

static void draw_frame(const Plan *p, int frame) {
    const char *const *picture = seed;
    if (frame == 1) picture = sprout;
    if (frame == 2) picture = sapling;
    if (frame >= 3) picture = young;
    if (frame >= 4 && p->value[0] >= 30) picture = canopy;
    if (frame >= 2 && p->value[0] == 0) picture = branches;
    for (int row = 0; row < 8; row++) puts(picture[row]);

    if (frame >= 3 && p->value[2] >= 30) puts("            ⢸⡇");
    else if (frame >= 2) puts("             ⡇");
    else puts("             ⠂");
    puts("  ⠒⠂⠐⠒⠒⠂⠐⠒⠂⠐⠒⠂⠸⠒⠂⠐⠒⠒⠂⠐⠒⠂⠐⠒");
    if (frame >= 3 && p->value[1] >= 30) {
        puts("        ⢀⡠⠔⠊⢹⠑⠢⢄⡀");
        puts("      ⠠⠔⠁    ⠘⠄   ⠉⠒⠤");
    } else {
        puts(frame ? "             ⠣" : "");
        puts("");
    }
}

void grow_tree(const Plan *p) {
    puts("\nสวนหลังร้าน");
    int last = p->count ? 4 : 0;
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(out, &mode) &&
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        /* วาดทับที่เดิมทีละภาพจนถึงภาพสุดท้าย */
        for (int frame = 0; frame <= last; frame++) {
            if (frame > 0) printf("\033[12A");
            /* ล้างบรรทัดที่ภาพเก่าเคยอยู่ รวมทั้งส่วนที่กว้างกว่าภาพใหม่ */
            printf("\033[J");
            draw_frame(p, frame);
            fflush(stdout);
            if (frame < last) Sleep(480);
        }
        SetConsoleMode(out, mode);
        return;
    }
#endif
    draw_frame(p, last);
}
