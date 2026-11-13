/* สวนรับฟัง ฉบับส่งงาน | gcc -std=c11 ListeningGarden.c -o GardenClass.exe
   บันทึกซอร์สเป็น UTF-8 เพื่อแสดงภาษาไทยและภาพจุด */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  /* ใช้เวลาเริ่มต้นลำดับเลขสุ่มเพียงครั้งเดียว */

#define TYPES 6
#define VISITS 8
#define STOCK 12
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
    int perfect;
} Result;

typedef struct {
    int type;
    const char *name, *story, *clarify, *reply;
    const char *question, *otherQuestion, *otherReply, *nextMorning, *listenAction;
} Story;

Move make_move(int a, int b) {
    Move move={a,b};
    return move;
}

/* เรื่องของลูกค้า */
const Story stories[] = {
    {0, "นิด · คนขายขนม","เช้านี้เด็กที่เคยยืนเขย่งซื้อขนมหน้าร้าน พาลูกตัวเองมาซื้อแล้วนะ เขายังจำได้ว่าฉันแถมชิ้นริมให้เสมอ","ขอเล่าอีกนิดนะ ตอนเห็นเขาสองคนยืนด้วยกัน ฉันนึกอะไรขึ้นมาได้ตั้งเยอะ","ลูกเขาก็ชอบชิ้นริมเหมือนกัน ฉันเลยตัดไว้ให้สองชิ้น",
     "แล้วลูกเขาเลือกชิ้นไหน", "ยังจำครั้งแรกที่เขามาซื้อได้ไหม", "จำได้สิ เขานับเหรียญบนฝ่ามืออยู่ตั้งนาน ฉันเลยวางขนมลงไปให้ก่อน", "วันนี้เก็บชิ้นริมไว้สามชิ้น เผื่อเขาพาคุณยายมาด้วย", "คุณถามนิดว่าตอนตัดชิ้นริม เด็กสองรุ่นยืนรอตรงเดียวกันไหม"},
    {1, "เมฆ · ช่างไม้","โต๊ะตัวแรกที่ทำเองขายออกแล้ว แต่พอเขาถามว่ารับทำอีกไหม ผมดันตอบไม่ออก","ช่วยผมคิดหน่อยว่าจะเริ่มรับงานยังไงดี","ถ้ารับทีละตัว ผมก็คงยังทำขาโต๊ะทุกขาได้ละเอียดเหมือนตัวแรก",
     "ถ้ารับงานอีกตัว อยากเริ่มยังไง", "ตรงไหนของโต๊ะที่ภูมิใจที่สุด", "ใต้โต๊ะครับ ผมขัดจนเอามือลูบแล้วไม่เจอเสี้ยน ลูกค้าไม่เห็นแต่ผมรู้", "ผมจดชื่อคนสั่งโต๊ะตัวที่สองแล้วครับ", "คุณถามเมฆว่า ถ้าสัปดาห์นี้มีเวลาทำโต๊ะแค่ตัวเดียว เขาอยากทำแบบไหน"},
    {0, "อุ่น · คนส่งจดหมาย","จดหมายที่ตามหาเจ้าของมาสามเดือนถึงมือสักที วันนี้ผมอยากนั่งจิบชาช้า ๆ สักแก้ว","นั่งดื่มชาด้วยกันสักครู่นะครับ","ชาหอมจัง ปกติผมดื่มไปมองนาฬิกาไป เพิ่งได้กลิ่นมันเต็ม ๆ วันนี้",
     "นั่งดื่มชาด้วยกันไหม", "ตอนเขาได้จดหมาย เขาว่ายังไง", "เขาพลิกดูชื่อคนส่ง แล้วไปหยิบแว่นมาอย่างช้า ๆ ผมเลยยืนรอเป็นเพื่อน", "วันนี้ผมพกกระติกชาไปส่งจดหมายด้วย", "คุณรินชาอีกถ้วยแล้วนั่งลงข้างอุ่น"},
    {1, "ฝน · นักว่ายน้ำ","เมื่อวานว่ายถึงอีกฝั่งครั้งแรก ใช้เวลาตั้งนาน แต่ครูยังยืนรออยู่","อยากเล่าตอนแตะขอบสระให้ใครสักคนฟัง","พอเงยหน้าขึ้น ครูยิ้มกว้างกว่าฉันอีก",
     "ครูทำหน้ายังไงตอนแตะขอบสระ", "กลางสระมีจังหวะที่อยากหยุดไหม", "มีสิ ฉันมองกระเบื้องสีฟ้าทีละแผ่น พอรู้ตัวอีกทีก็ถึงขอบแล้ว", "เช้านี้ฉันลงสระอีก คราวนี้ได้ยินเสียงน้ำด้วย", "คุณเท้าคางฟังฝน “ตอนแตะขอบสระ เงยหน้ามาเห็นอะไรเป็นอย่างแรกเหรอ?”"},
    {2, "ยายมะลิ · คนจัดดอกไม้","เผลอจัดดอกไม้ไว้สองกำอีกแล้ว เมื่อก่อนตาจะมารับกำหนึ่งกลับบ้านทุกวัน","ขอนั่งตรงนี้หน่อยนะ วันนี้ร้านมันเงียบไป","เอาดอกนี้ปักแจกันร้านหนูเถอะ เดี๋ยวยายเก็บอีกกำกลับบ้าน",
     "ให้ช่วยหาแจกันไว้ข้างเก้าอี้ไหม", "ตาชอบดอกไหนที่สุดเหรอจ๊ะ", "ดอกที่ก้านคด ๆ นั่นแหละ ตาบอกว่ามันหันมามองแกพอดี", "วันนี้ยายเก็บดอกก้านคดไว้ให้ร้านหนูดอกหนึ่ง", "คุณเลื่อนเก้าอี้ให้ยายมะลิ แล้วหาที่วางดอกไม้ทั้งสองกำ"},
    {3, "ป่าน · ครูฝึกสอน","พรุ่งนี้ต้องยืนหน้าห้องคนเดียว ฉันเขียนคำทักทายไว้เต็มกระดาษจนเลือกไม่ถูก","ลองช่วยคิดประโยคแรกด้วยกันได้ไหม","เริ่มด้วยการถามชื่อเด็ก ๆ ก็ดี ฉันเองก็อยากรู้จักพวกเขา",
     "ถ้าเริ่มด้วยการถามชื่อเด็ก ๆ ล่ะ", "สมัยเรียน ชอบครูคนไหนที่สุด", "ครูที่จำชื่อเล่นฉันได้ตั้งแต่วันแรกค่ะ แค่เขาเรียกชื่อ ห้องก็ดูเล็กลง", "วันนี้เด็กแถวหน้าบอกชื่อเล่นฉันด้วย ยังจำได้อยู่เลย", "คุณชวนป่านลองซ้อมดู “สมมติว่าฉันนั่งอยู่แถวหน้า ครูอยากรู้จักฉันยังไง?”"},
    {2, "กล้า · คนดูแลสนาม","ต้นหูกวางข้างสนามล้มแล้ว ตอนเด็กผมชอบซ่อนลูกแก้วไว้ตรงง่ามราก","ยังอยากเล่าเรื่องต้นนั้นอยู่น่ะครับ","ตอนเก็บกิ่ง ผมเจอลูกแก้วสีเขียวลูกหนึ่งด้วย นึกว่าหายไปแล้ว",
     "ตอนเก็บกิ่ง เจออะไรบ้าง", "ยังจำได้ไหมว่าเคยนั่งตรงไหน", "ตรงรอยแยกของรากครับ ผมกับน้องนั่งเบียดกันได้พอดี", "ผมเอาลูกแก้วไปให้น้องดู เขาจำสีมันได้ด้วย", "คุณเว้นที่บนโต๊ะให้กล้าวาดง่ามรากที่เขาเคยซ่อนลูกแก้ว"},
    {3, "ไหม · นักเดินทาง","ตั๋วเที่ยวแรกอยู่ในกระเป๋าแล้ว แต่พอนึกถึงสถานีที่ไม่รู้จัก มือฉันก็เย็นไปหมด","รอรถเป็นเพื่อนกันหน่อยได้ไหม","เสียงรถมาแล้ว ขอบคุณที่อยู่จนฉันลุกไหวนะ",
     "นั่งรอจนรถมาด้วยกันไหม", "ลงรถแล้วอยากเห็นอะไรก่อน", "ป้ายชื่อสถานีค่ะ ฉันอยากถ่ายไว้ส่งให้แม่ดูว่ามาถึงแล้ว", "ฉันส่งรูปป้ายสถานีมาให้ดูตามที่ตั้งใจไว้แล้วนะ", "คุณขยับม้านั่งให้ไหมนั่งข้างกัน ตรงนี้มองเห็นรถที่จะเข้าป้ายพอดี"},
    {4, "เต้ · ช่างป้าย","ป้ายที่ผมวาดทั้งคืนโดนแก้โดยไม่มีใครบอก อยากคุยกับหัวหน้า แต่กลัวจะขึ้นเสียง","ช่วยคิดคำเปิดให้หน่อยครับ ผมอยากให้เขาฟังจนจบ","ถามก่อนว่าเขาแก้ตรงไหน แล้วค่อยขอให้บอกกันคราวหน้า แบบนี้ผมพูดไหว",
     "อยากให้หัวหน้ารู้อะไรก่อน", "ป้ายเดิมมีตรงไหนที่อยากเก็บไว้", "ตรงหางตัวอักษรครับ ผมตั้งใจวาดให้เหมือนลายมือเจ้าของร้าน", "หัวหน้าเก็บแบบร่างของผมไว้ดูด้วยกันแล้ว", "คุณลองเป็นหัวหน้าให้เต้ซ้อมคุย “เอ้า ผมฟังอยู่ อยากถามอะไรก่อน?”"},
    {5, "ดาว · นักร้อง","ซ้อมเพลงนี้มาสองเดือน พอขึ้นเวทีไมค์กลับดับตรงท่อนที่รอร้องที่สุด","ตอนนี้ยังไม่อยากหาวิธีแก้ ขอเล่าให้จบก่อนนะ","มีคนแถวหน้าร้องท่อนนั้นต่อให้ด้วย ตอนแรกฉันมัวแต่เสียดายจนลืมไปเลย",
     "ตอนนั้นได้ยินอะไรจากหน้าเวทีบ้าง", "ท่อนไหนที่อยากร้องให้เขาฟังที่สุด", "ท่อนที่แม่เคยฮัมตอนล้างจานค่ะ ฉันเห็นแม่นั่งอยู่แถวสามพอดี", "วันนี้ลองร้องท่อนนั้นอีกรอบแล้วนะ", "คุณวางช้อนชาลง ดาวสูดลมหายใจแล้วเล่าต่อถึงตอนที่ไมค์เงียบ"},
    {4, "อิฐ · คนล้างจาน","วันนี้โดนว่าเรื่องจานแตกทั้งที่ผมเพิ่งเดินเข้าครัว ยังไม่มีใครถามผมสักคำ","อยากให้ใครสักคนฟังฝั่งผมบ้างครับ","แค่ได้พูดจนจบโดยไม่มีใครแทรก ผมก็หายกำหมัดแล้ว",
     "เล่าให้ฟังได้ไหมว่าเกิดอะไรขึ้น", "ตอนนี้อยากให้คนในครัวทำอะไร", "อยากให้เขาถามว่าใครเจ็บไหมก่อนครับ เศษจานอยู่เต็มพื้นเลย", "วันนี้คนในครัวเดินมาฟังผมเล่าจนจบแล้ว", "คุณให้อิฐเล่าตั้งแต่ตอนเปิดประตูครัว คราวนี้เขาได้พูดจนจบ"},
    {5, "หลิว · คนเพาะกล้า","ถาดที่หว่านไว้ไม่งอกสักต้น วันนี้ยังทำใจเปิดถุงเมล็ดใหม่ไม่ได้","นั่งพักมือข้าง ๆ กันสักหน่อยนะ","พรุ่งนี้ค่อยลองใหม่ วันนี้ขอเอาถาดไปล้างก่อนก็พอ",
     "นั่งพักมือด้วยกันไหม", "อยากเก็บอะไรจากถาดนี้ไว้บ้าง", "ป้ายวันที่หว่านค่ะ คราวหน้าจะได้รู้ว่าดินชื้นอยู่นานแค่ไหน", "เช้านี้ฉันเขียนวันที่บนป้ายใหม่แล้ว", "คุณช่วยหลิววางถาดเพาะไว้ข้างม้านั่ง แล้วนั่งพักมือด้วยกัน"}
};

/* ผลึก คะแนน และการสุ่มวัน */
const char crystal[][64] = {"สุข","ภูมิใจ","เศร้า","กลัว","โกรธ","ผิดหวัง"};
const char part[][64] = {"ใบและดอก","ราก","เปลือก"};
const char weather[][64] = {"พายุ","แล้ง","แมลง"};
const char forecast[][512] = {
 "คืนนี้มีพายุ • รากอย่างน้อย 30 และไม่น้อยกว่าใบ ส่วนเปลือกต้องถึง 20 และอย่างน้อยครึ่งหนึ่งของใบ",
 "คืนนี้ดินจะแห้ง • รากต้องถึง 30 และไม่น้อยกว่าใบ จึงจะส่งน้ำไปเลี้ยงยอดได้ทั่ว",
 "คืนนี้แมลงจะลง • เปลือกต้องถึง 30 และไม่น้อยกว่าใบ จึงจะป้องกันยอดอ่อนได้"};
const char *mix_names[6][6]={
  {0,0,"อบอุ่นใจ","กล้าหาญ","มุ่งมั่น","มีหวัง"},
  {0,0,"ยอมรับตัวเอง","เชื่อมั่นในตัวเอง","เคารพตัวเอง","หนักแน่น"},
  {0,0,0,0,"ฮึดสู้","เห็นอกเห็นใจ"},
  {0,0,0,0,"เด็ดเดี่ยว","ทำใจยอมรับ"}};

const char *recipe(int a,int b) {
 if (a <= b) return mix_names[a][b];
 return mix_names[b][a];
}
int target(Move m) {
    int first = m.a / 2;
    if (m.b == -1) return first;
    int second = m.b / 2;
    /* เลือกกลุ่มที่ต่างจากวัตถุดิบทั้งสองชิ้น */
    for (int group = 0; group < 3; group++) {
        if (group != first && group != second) return group;
    }
    return -1;
}
int apply(Plan *p,Move m) {
 if(m.a<0 || m.a>=TYPES || m.b < -1 || m.b>=TYPES || !p->bag[m.a] || p->count>=STOCK) return 0;
 if(m.b>=0 && (m.a/2==m.b/2 || !p->bag[m.b])) return 0;
 p->bag[m.a]--;
 if(m.b>=0) p->bag[m.b]--;
 p->value[target(m)]+=10;
 p->moves[p->count++]=m;
 return 1;
}
void undo(Plan *p) {
 if(!p->count) return;
 Move m=p->moves[--p->count];
 p->bag[m.a]++;
 if(m.b>=0) p->bag[m.b]++;
 p->value[target(m)]-=10;
}
int larger(int a, int b) {
    if (a>b) return a;
    return b;
}

int smaller(int a, int b) {
    if (a<b) return a;
    return b;
}

Result evaluate(const int v[3],int w) {
 Result r={0};
 int low=v[0],high=v[0];
 for(int i=0;i<3;i++) {
  low=smaller(low,v[i]); high=larger(high,v[i]);
  r.growth+=smaller(v[i],30)/3.0;
 }
 int rootGoal = 0, barkGoal = 0;
 if (w == 0 || w == 1) rootGoal = larger(30, v[0]);
 if (w == 0) barkGoal = larger(20, v[0] / 2);
 if (w == 2) barkGoal = larger(30, v[0]);
 r.missing[1] = larger(0, rootGoal - v[1]);
 r.missing[2] = larger(0, barkGoal - v[2]);
 r.health=larger(0,100-2*(r.missing[1]+r.missing[2]));
 r.weatherScore=.3*r.health;
 if (high > 0) r.balance = 40.0 * low / high;
 r.perfect=low>=30 && low==high && r.health==100;
 return r;
}
/* หาผลึกที่ยังเหลือในกลุ่ม คืน -1 ถ้าใช้หมดแล้ว */
int remaining(const Plan *p,int group) {
 int first=group*2;
 if(p->bag[first]>0) return first;
 if(p->bag[first+1]>0) return first+1;
 return -1;
}
int find_answer(Day *d, int scarce, int mixes) {
 Plan p={0};
 for (int i=0; i<TYPES; i++) p.bag[i]=d->stock[i];
 /* ผสมให้ส่วนที่ขาดก่อน แล้วเติมผลึกตรงกลุ่มให้ครบด้านละ 30 */
 for (int i=0; i<mixes; i++) {
    int a=remaining(&p,(scarce+1)%3), b=remaining(&p,(scarce+2)%3);
    if (a<0 || b<0 || !apply(&p,make_move(a,b))) return 0;
 }
 for (int g=0; g<3; g++) {
    while (p.value[g]<30) {
        int a=remaining(&p,g);
        if (a<0 || !apply(&p,make_move(a,-1))) return 0;
    }
 }
 d->answer=p;
 return evaluate(p.value,d->weather).perfect;
}

/* ตัวอย่าง draw(3) ให้ผลเป็น 0, 1 หรือ 2 */
int draw(int count) {
    return rand() % count;
}

void shuffle(int ids[], int size) {
    for (int i=size-1; i>0; i--) {
        int j=draw(i+1), temp=ids[i];
        ids[i]=ids[j];
        ids[j]=temp;
    }
}

void new_day(Day *d) {
    for (;;) {
        Day empty={0};
        *d=empty;
        int roll=draw(10);
        int mixes=0;
        if (roll>=8) { d->kind=2; mixes=3; }
        else if (roll>=4) { d->kind=1; mixes=1; }
        d->weather=draw(3);

        /* เริ่มที่ด้านละสามชิ้น แล้วแทนชิ้นที่ต้องผสมด้วยวัตถุดิบสองกลุ่ม */
        int scarce=draw(3), left[3]={3,3,3};
        left[scarce]-=mixes;
        left[(scarce+1)%3]+=mixes;
        left[(scarce+2)%3]+=mixes;
        for (int i=0; i<STOCK-9-mixes; i++) {
            int group;
            if (mixes>0) group=(scarce+1+draw(2))%3;
            else group=draw(3);
            left[group]++;
        }

        /* สับบัตรชื่อ 12 ใบ แล้วลองหยิบ 8 ใบแรกจนแจกผลึกได้ครบทุกกลุ่ม */
        int ids[12], people[3], valid;
        for (int i=0; i<12; i++) ids[i]=i;
        do {
            shuffle(ids,12);
            for (int g=0; g<3; g++) people[g]=0;
            for (int i=0; i<VISITS; i++) people[stories[ids[i]].type/2]++;
            valid=1;
            for (int g=0; g<3; g++) {
                if (people[g]>left[g] || (left[g]>0 && people[g]==0)) valid=0;
            }
        } while (!valid);
        for (int i=0; i<VISITS; i++) {
            d->guest[i]=ids[i];
            d->gift[i]=1;
            left[stories[ids[i]].type/2]--;
        }
        for (int g=0; g<3; g++) {
            while (left[g]>0) {
                int i=draw(VISITS);
                if (stories[ids[i]].type/2==g) {
                    d->gift[i]++;
                    left[g]--;
                }
            }
        }
        for (int i=0; i<VISITS; i++) d->stock[stories[ids[i]].type]+=d->gift[i];
        if (find_answer(d,scarce,mixes)) return;
    }
}

/* ภาพต้นไม้ */
/* รูปหนึ่งรูปมี 8 บรรทัด เว้นแถวบนให้โคนต้นอยู่ที่เดิม */
const char pictures[6][8][120]={
    { /* seed */
        "",
        "",
        "",
        "",
        "",
        "",
        "            ⢀⡤⢤⡀",
        "            ⠻⠤⠞⠁"
    },
    { /* sprout */
        "",
        "",
        "",
        "",
        "",
        "              ⣠⢖⡶",
        "           ⠘⢿⣶⠋⠉",
        "             ⢸"
    },
    { /* sapling */
        "",
        "",
        "         ⣀⣀",
        "         ⢳⡈⠙⢦  ⢀⣠⠤⢤",
        "          ⠙⠲⠬⣇⢠⠋ ⢀⠎",
        "          ⢠⡤⠤⣜⡯⠤⠖⠋",
        "           ⠳⠤⠼⡇",
        "             ⢠⠇"
    },
    { /* branches */
        "",
        "         ⢠",
        "          ⢧⡀ ⡄     ⣠",
        "      ⢀    ⠙⠦⡇   ⣀⠜⠁",
        "      ⠈⠳⡀    ⣇⣠⠤⠚⠁",
        "        ⠈⠳⢤⣀ ⡗",
        "            ⠉⡇",
        "             ⡇"
    },
    { /* young */
        "",
        "           ⡼⠳⡄      ⣀⣀⡀",
        "           ⠹⣄⡽⢀⣤⡀ ⢠⠎⢁⡰⠃",
        "    ⠸⡉⠙⢦⢠⣄⡀ ⠈⢹⢸⡀⢳⢀⡞⠉⠉",
        "     ⠙⠦⠤⢿⡀⠙⡆ ⢸ ⣳⠞⠋",
        "         ⠙⠲⢧⣀⣸⠋⠁",
        "             ⢹",
        "             ⡞"
    },
    { /* canopy */
        "       ⢀⣀⣀⡀⣠⠖⠒⠒⢦⣠⠤⠤⣄",
        "    ⣀⣀⣰⠋  ⠉   ⠰⠋⠉⠁ ⠈⠧⠤⠤⣄",
        "   ⢸⠁     ⠞⠉⠉          ⠈⡆",
        " ⢀⡴⠋  ⣀⣀⡀           ⠞⠉⠉ ⠙⠲⡀",
        " ⠘⢦⡀ ⠘⠁     ⠠⡀  ⢠⠖⠒⠂    ⢀⡴⠁",
        "   ⣇⣀⣀⡀  ⠐⣄  ⢹⡀  ⡴⠊    ⢀⡼",
        "    ⠈ ⠙⠢⠴⠚⠛⠷⡤⢤⣧⣶⠟⠦⢤⠴⠊⠉⠉⠉",
        "            ⢱⠘⢰⠃"
    }
};

void draw_frame(const Plan *p, int frame, const Result *morning) {
    int image=0;                      /* เมล็ด */
    if (frame==1) image=1;            /* แตกหน่อ */
    if (frame==2) image=2;            /* ต้นกล้า */
    if (frame>=3) image=4;            /* มีใบ */
    if (frame>=4 && p->value[0]>=30) image=5;
    if (frame>=2 && p->value[0]==0) image=3;
    int leaning=morning && morning->missing[1]>0 && p->value[0]>p->value[1];
    for (int row=0; row<8; row++) {
        /* กิ่งเอนออกจากโคนมากขึ้น ยิ่งอยู่ใกล้ยอด */
        if (leaning) printf("%*s",8-row,"");
        if (morning && morning->perfect && row==2)
            puts("   ⢸⠁  ✿  ⠞⠉⠉    ✿     ⠈⡆");
        else if (morning && morning->perfect && row==4)
            puts(" ⠘⢦⡀ ⠘⠁  ✿  ⠠⡀  ⢠⠖⠒⠂ ✿  ⢀⡴⠁");
        else puts(pictures[image][row]);
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
        if (frame>0) puts("             ⠣");
        else puts("");
        puts("");
    }
}

void show_tree(const Plan *p, const Result *morning) {
    if (p->count>0) draw_frame(p,4,morning);
    else draw_frame(p,0,morning);
}

void grow_tree(const Plan *p) {
    puts("\nสวนหลังร้าน");
    if (!p->count) {
        draw_frame(p,0,NULL);
        return;
    }
    char line[100];
    for (int frame=0; frame<=4; frame++) {
        draw_frame(p,frame,NULL);
        if (frame<4) {
            puts("กด Enter เพื่อดูต้นไม้โตต่อ");
            if (!fgets(line,sizeof line,stdin)) return;
        }
    }
}

/* เมนูและลำดับการเล่น */
/* อ่านทีละบรรทัด ถ้าพิมพ์ผิด ให้เลือกใหม่โดยไม่เสียผลึก */
int choose(int low, int high) {
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

void care_notes(void) {

    puts("สุข / ภูมิใจ เลี้ยงใบและดอก     เศร้า / กลัว เลี้ยงราก");
    puts("โกรธ / ผิดหวัง เลี้ยงเปลือก     ผลึกหนึ่งชิ้นให้ 10 หน่วย");
    puts("ผสมผลึกต่างกลุ่มสองชิ้น จะได้ปุ๋ยของกลุ่มที่สาม 10 หน่วย");
    puts("ถ้าอยากเห็นดอก ให้ทั้งสามด้านเท่ากัน ด้านละอย่างน้อย 30");
}

void guide(const Day *day) {
    puts("\nสมุดที่ป้าอิงฝากไว้");
    care_notes();
    puts(forecast[day->weather]);
}

void meet_customers(const Day *day, Plan *plan) {

    for (int i=0; i<VISITS; i++) {
        const Story *s=&stories[day->guest[i]];
        printf("\n%02d:00  |  %s\n“%s”\n", 9+i, s->name, s->story);

        printf("1. %s\n2. %s\n3. ถามว่าอยากให้ช่วยอะไร\n",s->question,s->otherQuestion);
        int answer=choose(1,3);
        if (answer==3) {
            printf("“%s”\n",s->clarify);
            puts(s->listenAction);
        }
        if (answer==2) puts(s->otherReply);
        else puts(s->reply);
        plan->bag[s->type]+=day->gift[i];
        printf("รับผลึก%s %d ชิ้น เก็บเข้าคลังแล้ว\n",crystal[s->type],day->gift[i]);
    }
}

void proportions(const Plan *p) {
    int total=p->value[0]+p->value[1]+p->value[2];
    if (total==0) { puts("ยังไม่ได้ใส่ปุ๋ย"); return; }
    for (int i=0; i<3; i++)
        printf("%s %d (%.1f%%)   ",part[i],p->value[i],100.0*p->value[i]/total);
    puts("");
}

void describe(Move move) {
    if (move.b==-1) printf("%s",crystal[move.a]);
    else printf("%s + %s = %s",crystal[move.a],crystal[move.b],recipe(move.a,move.b));
    printf(" → %s +10\n",part[target(move)]);
}

void show_aunt_ing(void) {
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

void welcome(void) {
    puts("สวนรับฟัง");
    Plan title_tree={0};
    for (int i=0; i<3; i++) title_tree.value[i]=30;
    title_tree.count=9; /* ต้นไม้ประดับหน้าเมนู */
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
            if (recipe(a,b)) describe(make_move(a,b));
        }
    }
    puts("\n1. เริ่มเนื้อเรื่อง"); choose(1,1);
}

/* เลือกส่วนของต้นไม้ก่อน จะได้เห็นเฉพาะผลึกที่บำรุงส่วนนั้นได้ */
int feed_group(Plan *plan, int group) {
    for (;;) {
        Move options[6]; /* ผลึกตรงกลุ่ม 2 ชนิด และสูตรข้ามกลุ่ม 4 คู่ */
        int count=0;
        for (int a=0; a<TYPES; a++) {
            if (a/2!=group || !plan->bag[a]) continue;
            options[count++]=make_move(a,-1);
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
                return 0;
            }
            printf("1. ไปผสมปุ๋ยสำหรับ%s   0. กลับไปเลือกส่วนอื่น\n",part[group]);
            if (choose(0,1)==0) return 0;
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
            return 0;
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
        return 1;
    }
}

int feed(Plan *plan) {
    for (;;) {
        puts("\nอยากบำรุงส่วนไหน?");
        puts("1. ใบและดอก   2. ราก   3. เปลือก   0. กลับโต๊ะปลูก");
        int group=choose(0,3)-1;
        if (group<0) return 0;
        if (feed_group(plan,group)) return 1;
    }
}

void planting(const Day *day, Plan *plan) {
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

const char *ending(const Plan *p, Result r) {
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
void visitor_returns(const Day *day, const Plan *p) {
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

void morning(const Day *day,const Plan *p) {
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
    system("chcp 65001 >nul"); /* ให้คอนโซล Windows อ่านข้อความเป็น UTF-8 */
#endif
    srand((unsigned)time(NULL));
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
