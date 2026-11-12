#include "garden.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* แต่ละภาพวางชิดดินที่แถว 7 เช่น [6] คือให้เริ่มวาดที่แถว 6 */
static const char *const seed[8] = {
    [6] = "            ⢀⡤⢤⡀",
    "            ⠻⠤⠞⠁"
};
static const char *const sprout[8] = {
    [5] = "              ⣠⢖⡶",
    "           ⠘⢿⣶⠋⠉",
    "             ⢸"
};
static const char *const sapling[8] = {
    [2] = "         ⣀⣀",
    "         ⢳⡈⠙⢦  ⢀⣠⠤⢤",
    "          ⠙⠲⠬⣇⢠⠋ ⢀⠎",
    "          ⢠⡤⠤⣜⡯⠤⠖⠋",
    "           ⠳⠤⠼⡇",
    "             ⢠⠇"
};
static const char *const branches[8] = {
    [1] = "         ⢠",
    "          ⢧⡀ ⡄     ⣠",
    "      ⢀    ⠙⠦⡇   ⣀⠜⠁",
    "      ⠈⠳⡀    ⣇⣠⠤⠚⠁",
    "        ⠈⠳⢤⣀ ⡗",
    "            ⠉⡇",
    "             ⡇"
};
static const char *const young[8] = {
    [1] = "           ⡼⠳⡄      ⣀⣀⡀",
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

static void draw_frame(const Plan *p, int frame, const Result *morning) {
    const char *const *picture = seed;
    if (frame == 1) picture = sprout;
    if (frame == 2) picture = sapling;
    if (frame >= 3) picture = young;
    if (frame >= 4 && p->value[0] >= 30) picture = canopy;
    if (frame >= 2 && p->value[0] == 0) picture = branches;
    bool leaning=morning && morning->missing[1]>0 && p->value[0]>p->value[1];
    for (int row=0; row<8; row++) {
        /* กิ่งเอนออกจากโคนมากขึ้น ยิ่งอยู่ใกล้ยอด */
        if (leaning) printf("%*s",8-row,"");
        if (morning && morning->perfect && row==2)
            puts("   ⢸⠁  ✿  ⠞⠉⠉    ✿     ⠈⡆");
        else if (morning && morning->perfect && row==4)
            puts(" ⠘⢦⡀ ⠘⠁  ✿  ⠠⡀  ⢠⠖⠒⠂ ✿  ⢀⡴⠁");
        else puts(picture[row] ? picture[row] : "");
    }

    if (leaning) puts("             ⢀⠎");
    else if (frame && morning && morning->missing[2]>0) puts("             ⡏⠄");
    else if (frame >= 3 && p->value[2] >= 30) puts("            ⢸⡇");
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

void show_tree(const Plan *p, const Result *morning) {
    draw_frame(p,p->count ? 4 : 0,morning);
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
            draw_frame(p, frame,NULL);
            fflush(stdout);
            if (frame < last) Sleep(480);
        }
        SetConsoleMode(out, mode);
        return;
    }
#endif
    draw_frame(p, last,NULL);
}
