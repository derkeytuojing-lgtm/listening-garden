#ifndef GARDEN_H
#define GARDEN_H
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

enum { TYPES = 6, VISITS = 8, STOCK = 12 };

/* ผลึกเรียงเป็นคู่ 0–1 ใบ, 2–3 ราก, 4–5 เปลือก */
typedef struct {
    int a, b;              /* b เท่ากับ -1 เมื่อใช้ผลึกตรง ๆ */
} Move;

typedef struct {
    int bag[TYPES];        /* ผลึกดิบที่เหลือ */
    int value[3];          /* หน่วยที่ใส่ให้ใบ ราก เปลือก */
    int count;
    Move moves[STOCK];     /* จำสิ่งที่ทำไว้สำหรับปุ่มย้อน */
} Plan;

typedef struct {
    int weather, kind;
    int guest[VISITS], gift[VISITS], stock[TYPES];
    Plan answer;           /* แผนที่ตรวจแล้ว เก็บไว้เฉลยตอนจบ */
} Day;

typedef struct {
    double growth, balance, weatherScore;
    int missing[3], health;
    bool perfect;
} Result;

typedef struct {
    int type, need;
    const char *name, *story, *clarify, *reply;
} Story;

extern const Story stories[12];
extern const char *const crystal[TYPES], *const part[3], *const weather[3], *const forecast[3];
const char *recipe(int a, int b);
int target(Move m);
bool apply(Plan *p, Move m);
void undo(Plan *p);
Result evaluate(const int v[3], int w);
void new_day(Day *d);
#endif
