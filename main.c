#include "garden.h"
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

/* อ่านทีละบรรทัด ถ้าพิมพ์ผิด ให้เลือกใหม่โดยไม่เสียผลึก */
static int choose(int low, int high) {
    char line[100], extra;
    int answer;
    for (;;) {
        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) exit(0);
        if (sscanf(line, "%d %c", &answer, &extra) == 1 &&
            answer >= low && answer <= high) return answer;
        printf("เลือกเลข %d ถึง %d แล้วกด Enter\n", low, high);
    }
}

static void guide(const Day *day) {
    puts("\nสมุดที่ป้าอิงฝากไว้");
    puts("สุข / ภูมิใจ เลี้ยงใบและดอก     เศร้า / กลัว เลี้ยงราก");
    puts("โกรธ / ผิดหวัง เลี้ยงเปลือก     ผลึกหนึ่งชิ้นให้ 10 หน่วย");
    puts("ผสมผลึกต่างกลุ่มสองชิ้น จะได้ปุ๋ยของกลุ่มที่สาม 10 หน่วย");
    puts("ถ้าอยากเห็นดอก ให้ทั้งสามด้านเท่ากัน ด้านละอย่างน้อย 30");
    puts(forecast[day->weather]);
}

static void meet_customers(const Day *day, Plan *plan) {
    const char *responses[]={"เล่าต่อได้เลย","ลองคิดด้วยกันไหม","นั่งด้วยกันสักพักนะ"};
    for (int i=0; i<VISITS; i++) {
        const Story *s=&stories[day->guest[i]];
        printf("\n%02d:00  |  %s\n“%s”\n", 9+i, s->name, s->story);
        for (int j=0; j<3; j++) printf("%d. %s\n", j+1, responses[j]);
        int answer=choose(1,3)-1;
        if (answer != s->need) {
            printf("“%s”\n",s->clarify);
            printf("1. %s   2. เว้นจังหวะให้เขาเล่าต่อ\n",responses[s->need]);
            if (choose(1,2)==2) puts("คุณวางถ้วยชาลง แล้วรอจนเขาพร้อมพูด");
        }
        printf("“%s”\n",s->reply);
        plan->bag[s->type]+=day->gift[i];
        printf("รับผลึก%s %d ชิ้น เก็บเข้าคลังแล้ว\n",crystal[s->type],day->gift[i]);
    }
}

static void tree(const Plan *p, bool morning, Result r) {
    int leaves=p->value[0], roots=p->value[1], bark=p->value[2];
    if (p->count==0) { puts("    .\n~~~~o~~~~   เมล็ดใต้ผิวดิน"); return; }
    if (morning && r.perfect) puts("   * @ *\n  (@ * @)    ดอกแรกของเช้านี้");
    else if (leaves>=30) puts("  ( & & )\n (& & & &)   พุ่มใบกว้าง");
    else if (leaves>0) puts("    (,)      ยอดอ่อน");
    else puts("     Y       กิ่งยังไม่มีใบ");
    if (morning && r.missing[1]>0 && leaves>roots) puts("      /      ยอดเอน");
    else if (bark>=30) puts("    |||      เปลือกหนา");
    else puts("     |       ลำต้นบาง");
    if (roots>=30) puts("~~~~/|\\~~~~\n  _/ | \\_    รากแผ่กว้าง");
    else puts("~~~~~|~~~~~  รากสั้น");
}

static void proportions(const Plan *p) {
    int total=p->value[0]+p->value[1]+p->value[2];
    if (total==0) { puts("ยังไม่ได้ใส่ปุ๋ย"); return; }
    for (int i=0; i<3; i++)
        printf("%s %d (%.1f%%)   ",part[i],p->value[i],100.0*p->value[i]/total);
    puts("");
}

static void describe(Move move) {
    if (move.b==-1) printf("%s",crystal[move.a]);
    else printf("%s + %s = %s",crystal[move.a],crystal[move.b],recipe(move.a,move.b));
    printf(" → %s +10\n",part[target(move)]);
}

static void planting(const Day *day, Plan *plan) {
    for (;;) {
        puts("\n17:00  |  โต๊ะผสมปุ๋ย");
        for (int i=0; i<TYPES; i++)
            printf("%d. %s × %d%s",i+1,crystal[i],plan->bag[i],i%2 ? "\n":"     ");
        proportions(plan);
        tree(plan,false,evaluate(plan->value,day->weather));
        puts("1. ใช้ผลึกตรง ๆ   2. ผสมสองชิ้น   3. ย้อนหนึ่งครั้ง");
        puts("4. เริ่มจัดใหม่     5. เปิดคู่มือ      6. ปลูกและปิดร้าน");
        int action=choose(1,6);
        if (action==1 || action==2) {
            puts("เลือกเลขผลึกชิ้นแรก");
            Move move={choose(1,TYPES)-1,-1};
            if (action==2) {
                puts("เลือกเลขผลึกชิ้นที่สอง");
                move.b=choose(1,TYPES)-1;
                if (move.a/2==move.b/2) { puts("ต้องเลือกผลึกต่างกลุ่มกัน"); continue; }
            }
            if (!plan->bag[move.a] || (move.b>=0 && !plan->bag[move.b])) {
                puts("ผลึกที่เลือกหมดแล้ว"); continue;
            }
            describe(move);
            puts("1. ใส่ตามนี้   2. กลับไปเลือกใหม่");
            if (choose(1,2)==1) apply(plan,move);
        } else if (action==3) undo(plan);
        else if (action==4) { while (plan->count>0) undo(plan); }
        else if (action==5) guide(day);
        else {
            puts("ปิดร้านแล้วจะแก้แผนไม่ได้   1. ยืนยันปลูก   2. กลับไปจัดแผน");
            if (choose(1,2)==1) return;
        }
    }
}

static const char *ending(const Plan *p, Result r) {
    if (!p->count) return "เมล็ดยังนอนอยู่ใต้ดิน พรุ่งนี้ลองหยิบผลึกสักชิ้นมาฝังข้าง ๆ กันนะ";
    if (r.perfect) return "เห็นดอกตรงนั้นไหม วันที่ยิ้มได้ช่วยเลี้ยงใบ วันที่ใจหายก็ช่วยเลี้ยงราก ส่วนวันที่ต้องลุกขึ้นปกป้องตัวเอง ก็กลายมาเป็นเปลือกให้มัน";
    if (r.missing[1] && p->value[0]>p->value[1]) return "ยอดมันไปไกลกว่ารากเสียแล้ว ลองแบ่งที่ให้ความกลัวบ้าง รากจะได้โตทันตอนที่ใบพากันผลิ";
    if (r.missing[2]) return "เปลือกยังบางไปสำหรับคืนนี้ บางทีความโกรธก็บอกเราว่า ตรงไหนควรมีขอบไว้ป้องกันตัว";
    if (p->value[1]>p->value[0]) return "ลองคุ้ยดินดูสิ รากแผ่ไปตั้งไกล ถ้าได้แรงจากเรื่องที่ชื่นใจอีกสักหน่อย ยอดคงผลิใบตามทัน";
    if (p->value[2]>p->value[0]) return "เปลือกหนาดีจัง มันรู้จักปกป้องตัวเองแล้ว ค่อย ๆ เลี้ยงส่วนอื่นให้โตทันกันนะ";
    if (r.health<100) return "ต้นยังเล็กอยู่ เมื่อคืนเลยใช้แรงไปเยอะ ลองเติมปุ๋ยให้รากกับเปลือกอีกหน่อยนะ";
    return "มันผ่านคืนนี้มาได้แล้ว พรุ่งนี้ลองเดินดูรอบต้น ยังมีบางส่วนให้เราค่อย ๆ เติม";
}

static void morning(const Day *day,const Plan *p) {
    const char *event[]={"ฝนสาดเข้าที่กิ่ง ยอดเอนตามแรงลม","ลมร้อนพัดจนผิวดินเริ่มแตกระแหง","แมลงไต่จากโคนต้นขึ้นไปหายอดอ่อน"};
    Result r=evaluate(p->value,day->weather);
    printf("\nกลางคืน  |  %s\n%s\n",weather[day->weather],event[day->weather]);
    puts("1. รอจนฟ้าสาง"); choose(1,1);
    puts("\nเช้าวันถัดมา"); tree(p,true,r);
    printf("ป้าอิงก้มลงมองต้นไม้ “%s”\n",ending(p,r));
    puts("1. เปิดบันทึกการปลูก"); choose(1,1);
    proportions(p);
    printf("อากาศเมื่อคืน: %s   ขาดราก %d หน่วย / เปลือก %d หน่วย   สุขภาพ %d/100\n",
           weather[day->weather],r.missing[1],r.missing[2],r.health);
    printf("เติบโต %.1f/30   สมดุล %.1f/40   หลังผ่านอากาศ %.1f/30\n",r.growth,r.balance,r.weatherScore);
    printf("คะแนนรวม %.0f/100%s\n",r.growth+r.balance+r.weatherScore,r.perfect?"   Perfect!":"");
}

int main(void) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    srand((unsigned)time(NULL));
    for (;;) {
        Day day;
        Plan plan={0};
        new_day(&day);
        puts("\nสวนรับฟัง\n08:00  |  ป้าอิงวางห่อกระดาษลงบนเคาน์เตอร์");
        puts("“ฝากเมล็ดไว้สักคืนนะ วันนี้ใครแวะมาก็ลองฟังเขาดู พอตกเย็นค่อยเอาผลึกที่เขาฝากไว้มาปลูกกัน”");
        guide(&day);
        puts("1. เปิดประตูรับลูกค้า"); choose(1,1);
        meet_customers(&day,&plan);
        planting(&day,&plan);
        morning(&day,&plan);
        puts("1. ดูสูตร Perfect ของวันนี้   2. เล่นรอบใหม่   3. ออกจากเกม");
        int action=choose(1,3);
        if (action==1) {
            puts("เริ่มจากคลังก่อนใส่ปุ๋ย แล้วทำตามนี้ทีละบรรทัด");
            for (int i=0;i<day.answer.count;i++) describe(day.answer.moves[i]);
            puts("2. เล่นรอบใหม่   3. ออกจากเกม");
            action=choose(2,3);
        }
        if (action==3) return 0;
    }
}
