#include "garden.h"
#include <stdio.h>
#include <stdlib.h>
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

static void care_notes(void) {

    puts("สุข / ภูมิใจ เลี้ยงใบและดอก     เศร้า / กลัว เลี้ยงราก");
    puts("โกรธ / ผิดหวัง เลี้ยงเปลือก     ผลึกหนึ่งชิ้นให้ 10 หน่วย");
    puts("ผสมผลึกต่างกลุ่มสองชิ้น จะได้ปุ๋ยของกลุ่มที่สาม 10 หน่วย");
    puts("ถ้าอยากเห็นดอก ให้ทั้งสามด้านเท่ากัน ด้านละอย่างน้อย 30");
}

static void guide(const Day *day) {
    puts("\nสมุดที่ป้าอิงฝากไว้");
    care_notes();
    puts(forecast[day->weather]);
}

static void meet_customers(const Day *day, Plan *plan) {

    for (int i=0; i<VISITS; i++) {
        const Story *s=&stories[day->guest[i]];
        printf("\n%02d:00  |  %s\n“%s”\n", 9+i, s->name, s->story);

        printf("1. %s\n2. %s\n3. ถามว่าอยากให้ช่วยอะไร\n",s->question,s->otherQuestion);
        int answer=choose(1,3);
        if (answer==3) {
            printf("“%s”\n",s->clarify);
            puts(s->listenAction);
        }
        puts(answer==2 ? s->otherReply : s->reply);
        plan->bag[s->type]+=day->gift[i];
        printf("รับผลึก%s %d ชิ้น เก็บเข้าคลังแล้ว\n",crystal[s->type],day->gift[i]);
    }
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

static void welcome(void) {
    puts("สวนรับฟัง");
    puts("                  ⣠⢴⣒⣒⣲⢤⡀\n"
         "          ⢀⣀⡤⠤⠤⠤⠒⡞⠒⠲⠤⣌⡹⣽⠘⡆\n"
         "       ⢠⠔⠋⠁⢀⣠⠤⢀⣠⢦⡳⣈⠓⠦⣄⠙⢾⡰⠃\n"
         "      ⣰⠃ ⣠⠞⣉⡤⠖⠋  ⠳⣌⠙⠒⠚⠻⡍⢳\n"
         "      ⡇⢀⣴⠛⠉       ⢀⣙⣲⢤⣀⡀ ⡇\n"
         "     ⢀⡇⡞⢀⡴⠾⠭⠽⢦⡀  ⣠⠝⠒⠚⠻⣆⢉⣡⣽⢤\n"
         "     ⣏⢯⢩⡟⠰⠒⠲⠦ ⢻⢉⢹⡃⠚⠉⠋⠓⢈⡏⢠⠘⡼\n"
         "     ⠸⡌⢸⠙⠦⠤⢤⠤⠴⢫⠏ ⠙⠢⠤⢤⡔⠋ ⡞⣰⠃\n"
         "      ⠿⠛⡆⠈⠙⠂  ⠘⠒⠚ ⢀⡀⠉  ⢸⠛⠟\n"
         "        ⠱⡄  ⠈⠳⣤⠤⢤⡶⠋   ⢠⠃\n"
         "         ⠙⠦⣀  ⠈⠉⠉   ⢀⡴⠁\n"
         "           ⣼⠙⠒⠦⠤⠤⠤⠖⢺⣅\n"
         "     ⢀⡠⠴⠒⠋⢻⡁⠙⠦⣄⣀⣠⠤⠞⠁⣨⠏⠙⠒⠲⢤⣀\n"
         "   ⣠⠞⠉     ⠙⠦⣄⡴⠋⠳⣄⡤⠖⠁     ⠈⠓⢤⡀\n"
         "   ⠁                         ⠁");
    puts("ป้าอิงยิ้มให้จากหน้าร้าน");
    puts("1. เล่นเลย");
    puts("2. รู้จักเกม ดูวิธีเล่นและสูตรผสม");
    if (choose(1,2)==1) return;

    puts("\nเกมนี้เป็นแนวไหน?");
    puts("คุณเปิดร้านเล็ก ๆ ให้คนแวะมาเล่าเรื่อง แลกกับผลึกความรู้สึก");
    puts("พอร้านปิด ผลึกที่เก็บไว้จะกลายเป็นปริศนา คุณจะใช้มันเลี้ยงต้นไม้ยังไงให้ผ่านคืนนี้?");
    puts("\nวิธีเล่น");
    puts("ตอนเช้า ฟังพยากรณ์จากป้าอิง แล้วเลือกคำถามที่อยากถามลูกค้า");
    puts("ตอนเย็น เลือกใส่ผลึกทีละชนิดหรือผสมเป็นปุ๋ย ลองจัดใหม่ได้จนกว่าจะยืนยันปลูก");
    care_notes();
    puts("\n1. เปิดหน้าสูตรผสม"); choose(1,1);
    puts("\nสูตรผสมผลึก");
    for (int a=0; a<TYPES; a++) {
        for (int b=a+1; b<TYPES; b++) {
            if (recipe(a,b)) describe((Move){a,b});
        }
    }
    puts("\n1. เริ่มเนื้อเรื่อง"); choose(1,1);
}

/* เลือกส่วนของต้นไม้ก่อน จะได้เห็นเฉพาะผลึกที่บำรุงส่วนนั้นได้ */
static bool feed_group(Plan *plan, int group) {
    for (;;) {
        Move options[6]; /* ผลึกตรงกลุ่ม 2 ชนิด และสูตรข้ามกลุ่ม 4 คู่ */
        int count=0;
        for (int a=0; a<TYPES; a++) {
            if (a/2!=group || !plan->bag[a]) continue;
            options[count++]=(Move){a,-1};
        }
        int direct=count;
        for (int a=0; a<TYPES; a++) {
            for (int b=a+1; b<TYPES; b++) {
                Move mix={a,b};
                if (!plan->bag[a] || !plan->bag[b] || !recipe(a,b) || target(mix)!=group) continue;
                options[count++]=mix;
            }
        }
        if (!direct) {
            printf("\nผลึกสำหรับ%sหมดแล้ว\n",part[group]);
            if (!count) {
                puts("วัตถุดิบที่เหลือยังผสมปุ๋ยส่วนนี้ไม่ได้");
                puts("0. กลับไปเลือกส่วนอื่น");
                choose(0,0);
                return false;
            }
            printf("1. ไปผสมปุ๋ยสำหรับ%s   0. กลับไปเลือกส่วนอื่น\n",part[group]);
            if (choose(0,1)==0) return false;
            printf("\nผสมปุ๋ยสำหรับ%s (+10 ต่อสูตร)\n",part[group]);
        } else {
            printf("\nเลือกปุ๋ยสำหรับ%s (+10 ต่อครั้ง)\n",part[group]);
        }
        for (int i=0; i<count; i++) {
            Move m=options[i];
            if (m.b<0) printf("%d. ใช้%s (เหลือ %d ชิ้น)\n",i+1,crystal[m.a],plan->bag[m.a]);
            else printf("%d. ผสม%s + %s เป็น%s\n",i+1,crystal[m.a],crystal[m.b],recipe(m.a,m.b));
        }
        puts(direct ? "เลือกเลขเพื่อใส่ปุ๋ย หรือ 0 เพื่อเลือกส่วนอื่น" : "เลือกสูตรเพื่อผสมแล้วใส่ปุ๋ย หรือ 0 เพื่อกลับ");
        int choice=choose(0,count);
        if (!choice) {
            if (!direct) continue;
            return false;
        }
        Move move=options[choice-1];
        int amount=1;
        if (move.b<0 && plan->bag[move.a]>1) {
            printf("ใช้กี่ชิ้น? 1–%d หรือ 0 เพื่อเลือกปุ๋ยใหม่\n",plan->bag[move.a]);
            amount=choose(0,plan->bag[move.a]);
            if (!amount) continue;
        }
        for (int i=0; i<amount; i++) apply(plan,move);
        printf("เติม%s +%d\n",part[group],10*amount);
        return true;
    }
}

static bool feed(Plan *plan) {
    for (;;) {
        puts("\nอยากบำรุงส่วนไหน?");
        puts("1. ใบและดอก   2. ราก   3. เปลือก   0. กลับโต๊ะปลูก");
        int group=choose(0,3)-1;
        if (group<0) return false;
        if (feed_group(plan,group)) return true;
    }
}

static void planting(const Day *day, Plan *plan) {
    int before[STOCK], actions=0;
    for (;;) {
        printf("\n17:00  โต๊ะปลูก  •  คืนนี้%s\n",weather[day->weather]);
        puts("อยากให้ต้นออกดอก เลี้ยงทั้งสามส่วนให้เท่ากัน ด้านละอย่างน้อย 30");
        printf("ใบ %d   ราก %d   เปลือก %d\n",plan->value[0],plan->value[1],plan->value[2]);
        puts("1. ใส่ปุ๋ย   2. ดูหรือแก้แผน   3. ปลูกและปิดร้าน");
        int action=choose(1,3);
        if (action==1) {
            int previous=plan->count;
            if (feed(plan)) before[actions++]=previous;
        } else if (action==2) {
            puts("\nต้นไม้ตามแผนตอนนี้");
            show_tree(plan,NULL);
            for (int i=0; i<3; i++) {
                int value=plan->value[i];
                printf("%s %d หน่วย · ",part[i],value);
                if (value<30) printf("อีก %d หน่วยถึงขั้นต่ำ\n",30-value);
                else puts("ถึงขั้นต่ำแล้ว");
            }
            puts("ผลึกที่เหลือ");
            for (int i=0; i<TYPES; i++)
                printf("%s × %d%s",crystal[i],plan->bag[i],i%2 ? "\n" : "   ");
            puts("1. ย้อนครั้งล่าสุด   2. คืนผลึกทั้งหมด   3. เปิดคู่มือ   0. กลับ");
            int edit=choose(0,3);
            if (edit==1 && actions) {
                int stop=before[--actions];
                while (plan->count>stop) undo(plan);
                puts("คืนผลึกที่ใส่ครั้งล่าสุดแล้ว");
            } else if (edit==1) puts("ยังไม่มีรายการให้ย้อน");
            else if (edit==2) {
                while (plan->count) undo(plan);
                actions=0;
                puts("คืนผลึกทั้งหมดแล้ว");
            } else if (edit==3) guide(day);
        } else {
            if (!plan->count) puts("เมล็ดนี้ยังไม่ได้ใส่ปุ๋ย");
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

/* ผลึกชนิดเดียวกันใช้ของลูกค้าที่มาก่อน เลือกเล่าถึงคนที่ฝาก
   วัตถุดิบของสูตรแรก ถ้าไม่ได้ผสมให้เล่าถึงผลึกชิ้นแรกแทน */
static void visitor_returns(const Day *day, const Plan *p) {
    if (!p->count) return;
    int index=0;
    for (int i=0; i<p->count; i++) {
        if (p->moves[i].b>=0) { index=i; break; }
    }
    Move move=p->moves[index];
    int type=move.a, used=0;
    for (int i=0; i<index; i++) {
        if (p->moves[i].a==type || p->moves[i].b==type) used++;
    }
    for (int i=0; i<VISITS; i++) {
        int id=day->guest[i];
        if (stories[id].type!=type) continue;
        if (used>=day->gift[i]) { used-=day->gift[i]; continue; }
        printf("ผลึก%sจาก %s เมื่อวาน ",crystal[type],stories[id].name);
        if (move.b>=0) printf("ผสมเป็น “%s” ",recipe(move.a,move.b));
        printf("ได้เลี้ยง%sของต้นนี้\n",part[target(move)]);
        printf("“%s”\n",stories[id].nextMorning);
        break;
    }
}

static void morning(const Day *day,const Plan *p) {
    const char *event[]={"ฝนสาดเข้าที่กิ่ง ยอดเอนตามแรงลม","ลมร้อนพัดจนผิวดินเริ่มแตกระแหง","แมลงไต่จากโคนต้นขึ้นไปหายอดอ่อน"};
    Result r=evaluate(p->value,day->weather);
    printf("\nกลางคืน  |  %s\n%s\n",weather[day->weather],event[day->weather]);
    puts("1. รอจนฟ้าสาง"); choose(1,1);
    puts("\nเช้าวันถัดมา"); show_tree(p,&r);
    printf("ป้าอิงก้มลงมองต้นไม้ “%s”\n",ending(p,r));
    visitor_returns(day,p);
    puts("1. เปิดบันทึกการปลูก"); choose(1,1);
    puts("สัดส่วนปุ๋ยที่ใส่ทั้งหมด");
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
    welcome();
    for (;;) {
        Day day;
        Plan plan={0};
        new_day(&day);
        puts("\nสวนรับฟัง\n08:00  |  ป้าอิงวางห่อกระดาษลงบนเคาน์เตอร์");
        puts("“ฝากเมล็ดไว้สักคืนนะ วันนี้ใครแวะมาก็ลองฟังเขาดู พอตกเย็นค่อยเอาผลึกที่เขาฝากไว้มาปลูกกัน”");
        puts("ป้าอิงวางสมุดดูแลเมล็ดไว้ข้างห่อกระดาษ แล้วชี้ให้ดูพยากรณ์คืนนี้");
        puts(forecast[day.weather]);
        puts("1. เปิดประตูรับลูกค้า"); choose(1,1);
        meet_customers(&day,&plan);
        planting(&day,&plan);
        grow_tree(&plan);
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
