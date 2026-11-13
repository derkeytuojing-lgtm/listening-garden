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

static void show_aunt_ing(void) {
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
}

static void welcome(void) {
    puts("สวนรับฟัง");
    const Plan title_tree={.value={30,30,30}, .count=9}; /* ต้นไม้ประดับหน้าเมนู */
    show_tree(&title_tree,NULL);
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
        int options[2], count=0;
        printf("\nผลึกสำหรับ%s\n",part[group]);
        for (int i=group*2; i<group*2+2; i++) {
            if (!plan->bag[i]) continue;
            options[count++]=i;
            printf("%d. ใช้%s (เหลือ %d ชิ้น)\n",count,crystal[i],plan->bag[i]);
        }
        if (!count) {
            puts("ผลึกส่วนนี้หมดแล้ว");
            return 0;
        }
        puts("เลือกผลึก หรือ 0 เพื่อเลือกส่วนอื่น");
        int choice=choose(0,count);
        if (!choice) return 0;
        Move move={options[choice-1],-1};
        int amount=1;
        if (plan->bag[move.a]>1) {
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

static int mix_fertilizer(Plan *plan) {
    int options[TYPES], count=0, groups[3]={0};
    puts("\nโต๊ะผสมปุ๋ย");
    for (int i=0; i<TYPES; i++) {
        if (!plan->bag[i]) continue;
        groups[i/2]=1;
        options[count++]=i;
        printf("%d. %s × %d\n",count,crystal[i],plan->bag[i]);
    }
    if (groups[0]+groups[1]+groups[2]<2) {
        puts("ต้องมีผลึกต่างกลุ่มกันสองชิ้นจึงจะผสมได้");
        return 0;
    }
    puts("เลือกผลึกชิ้นแรก หรือ 0 เพื่อกลับโต๊ะปลูก");
    int first=choose(0,count);
    if (!first) return 0;
    puts("เลือกผลึกชิ้นที่สอง หรือ 0 เพื่อกลับโต๊ะปลูก");
    int second=choose(0,count);
    if (!second) return 0;
    Move move={options[first-1],options[second-1]};
    if (move.a/2==move.b/2) {
        puts("ผลึกสองชิ้นนี้อยู่กลุ่มเดียวกัน จึงผสมกันไม่ได้");
        return 0;
    }
    describe(move);
    puts("1. ผสมแล้วใส่ลงต้นไม้   0. กลับโต๊ะปลูก");
    if (!choose(0,1)) return 0;
    return apply(plan,move);
}

static void planting(const Day *day, Plan *plan) {
    int before[STOCK], actions=0;
    for (;;) {
        printf("\n17:00  โต๊ะปลูก  •  คืนนี้%s\n",weather[day->weather]);
        puts("อยากให้ต้นออกดอก เลี้ยงทั้งสามส่วนให้เท่ากัน ด้านละอย่างน้อย 30");
        printf("ใบ %d   ราก %d   เปลือก %d\n",plan->value[0],plan->value[1],plan->value[2]);
        puts("1. ใส่ปุ๋ย   2. ดูหรือแก้แผน   3. ปลูกและปิดร้าน   4. ผสมปุ๋ย");
        int action=choose(1,4);
        if (action==1) {
            int previous=plan->count;
            if (feed(plan)) before[actions++]=previous;
        } else if (action==4) {
            int previous=plan->count;
            if (mix_fertilizer(plan)) before[actions++]=previous;
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

static const char *ending(const Plan *p, Result r, int weather_type) {
    if (p->count==0)
        return "ดินยังเรียบอยู่เลย เมื่อวานเราเก็บผลึกไว้หมด เมล็ดเลยยังหลับอยู่ตรงที่ป้าฝังไว้";
    if (r.perfect)
        return "เห็นดอกเล็ก ๆ ตรงปลายกิ่งไหม เมื่อวานยังไม่มีเลยนะ ทั้งใบ ราก แล้วก็เปลือก โตมาทันกันพอดี";
    if (r.health==0) {
        if (weather_type==2)
            return "ต้นนี้ไปไม่รอดแล้วลูก ดูรอยที่โคนสิ แมลงเจาะผ่านเปลือกเข้าไปถึงเนื้อไม้แล้ว";
        if (weather_type==1)
            return "ต้นนี้ไปไม่รอดแล้วลูก ดินแห้งลงไปลึกกว่าที่รากจะหยั่งถึง ต่อให้มีใบเต็มกิ่งก็หาน้ำมาเลี้ยงไม่ทัน";
        if (r.missing[1] && r.missing[2])
            return "ต้นนี้ไปไม่รอดแล้วลูก พายุดึงรากจนดินเปิด เปลือกตรงกิ่งก็ปริ เห็นรอยตรงนี้ไหม";
        return "ต้นนี้ไปไม่รอดแล้วลูก เมื่อคืนลมแรง รากยึดดินไว้ไม่อยู่จนต้นล้มทั้งต้น";
    }
    if (r.missing[2])
        return "ต้นนี้รอดมาได้นะ ดูรอยบนเปลือกสิ คราวหน้าลองเก็บผลึกไว้เลี้ยงเปลือกอีกหน่อย";
    if (r.missing[1])
        return "ต้นนี้รอดมาได้นะ แต่รากยังสั้นอยู่ คราวหน้าฝังผลึกให้รากอีกหน่อย มันจะได้มีกำลังพอเลี้ยงยอด";
    return "ต้นนี้โตได้ดีเลยลูก ผ่านเมื่อคืนมาได้เต็มแรง ถึงเช้านี้ยังไม่มีดอก ป้าก็อยากมาดูอีกว่าพรุ่งนี้มันจะโตไปทางไหน";

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
    printf("ป้าอิงก้มลงมองต้นไม้ “%s”\n",ending(p,r,day->weather));
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
        show_aunt_ing();
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
