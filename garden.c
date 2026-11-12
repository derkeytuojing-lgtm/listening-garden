#include "garden.h"

const char *const crystal[] = {"สุข","ภูมิใจ","เศร้า","กลัว","โกรธ","ผิดหวัง"};
const char *const part[] = {"ใบและดอก","ราก","เปลือก"};
const char *const weather[] = {"พายุ","แล้ง","แมลง"};
const char *const forecast[] = {
 "คืนนี้มีพายุ • รากอย่างน้อย 30 และไม่น้อยกว่าใบ ส่วนเปลือกต้องถึง 20 และอย่างน้อยครึ่งหนึ่งของใบ",
 "คืนนี้ดินจะแห้ง • รากต้องถึง 30 และไม่น้อยกว่าใบ จึงจะส่งน้ำไปเลี้ยงยอดได้ทั่ว",
 "คืนนี้แมลงจะลง • เปลือกต้องถึง 30 และไม่น้อยกว่าใบ จึงจะป้องกันยอดอ่อนได้"};
const char *recipe(int a,int b) {
 static const char *const names[6][6]={
  {0,0,"อบอุ่นใจ","กล้าหาญ","มุ่งมั่น","มีหวัง"},
  {0,0,"ยอมรับตัวเอง","เชื่อมั่นในตัวเอง","เคารพตัวเอง","หนักแน่น"},
  {0,0,0,0,"ฮึดสู้","เห็นอกเห็นใจ"},
  {0,0,0,0,"เด็ดเดี่ยว","ทำใจยอมรับ"}};
 if (a <= b) return names[a][b];
 return names[b][a];
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
bool apply(Plan *p,Move m) {
 if(m.a<0 || m.a>=TYPES || m.b < -1 || m.b>=TYPES || !p->bag[m.a] || p->count>=STOCK) return false;
 if(m.b>=0 && (m.a/2==m.b/2 || !p->bag[m.b])) return false;
 p->bag[m.a]--;
 if(m.b>=0) p->bag[m.b]--;
 p->value[target(m)]+=10;
 p->moves[p->count++]=m;
 return true;
}
void undo(Plan *p) {
 if(!p->count) return;
 Move m=p->moves[--p->count];
 p->bag[m.a]++;
 if(m.b>=0) p->bag[m.b]++;
 p->value[target(m)]-=10;
}
Result evaluate(const int v[3],int w) {
 Result r={0};
 int low=v[0],high=v[0];
 for(int i=0;i<3;i++) {
  low=(int)fmin(low,v[i]); high=(int)fmax(high,v[i]);
  r.growth+=fmin(v[i],30)/3.0;
 }
 int rootGoal = 0, barkGoal = 0;
 if (w == 0 || w == 1) rootGoal = (int)fmax(30, v[0]);
 if (w == 0) barkGoal = (int)fmax(20, v[0] / 2);
 if (w == 2) barkGoal = (int)fmax(30, v[0]);
 r.missing[1] = (int)fmax(0, rootGoal - v[1]);
 r.missing[2] = (int)fmax(0, barkGoal - v[2]);
 r.health=(int)fmax(0,100-2*(r.missing[1]+r.missing[2]));
 r.weatherScore=.3*r.health;
 if (high > 0) r.balance = 40.0 * low / high;
 r.perfect=low>=30 && low==high && r.health==100;
 return r;
}
/* ทั้งสามชุดขาดได้เพียงกลุ่มเดียว เก็บของตรงกลุ่มไว้ด้านละ 3 ชิ้นก่อน
   ถ้าด้านใดยังไม่ถึง 30 ให้ผสมของที่เหลือจากอีกสองด้านจนพอดี */
static int remaining(const Plan *p,int group) {
 int first=group*2;
 if(p->bag[first]>0) return first;
 if(p->bag[first+1]>0) return first+1;
 return -1;
}
static bool find_answer(Day *d) {
 Plan p={0};
 memcpy(p.bag,d->stock,sizeof p.bag);
 for(int g=0;g<3;g++) {
  while(p.value[g]<30 && remaining(&p,g)>=0) {
   Move direct={remaining(&p,g),-1};
   apply(&p,direct);
  }
 }
 int mixes=0;
 for(int g=0;g<3;g++) {
  while(p.value[g]<30) {
   int a=remaining(&p,(g+1)%3);
   int b=remaining(&p,(g+2)%3);
   if(a<0 || b<0) return false;
   Move mix={a,b};
   apply(&p,mix);
   mixes++;
  }
 }
 const int required[]={0,1,3};
 d->answer=p;
 return mixes==required[d->kind] && evaluate(p.value,d->weather).perfect;
}

void new_day(Day *d) {
 const int sets[3][3]={{4,4,4},{5,5,2},{6,6,0}};
 do {
  memset(d,0,sizeof *d);
  int roll=rand()%10;
  if(roll<4) d->kind=0;
  else if(roll<8) d->kind=1;
  else d->kind=2;
  d->weather=rand()%3;
  int shift=rand()%3;
  int left[3], used[3]={0}, firstGuest[3]={0};
  for(int g=0;g<3;g++) left[(g+shift)%3]=sets[d->kind][g];

  /* แจกอย่างน้อยคนละหนึ่งชิ้น เลือกเรื่องจากกลุ่มที่ยังมีผลึก */
  int offset=rand()%4;
  int count=0;
  while(count<VISITS) {
   for(int g=0;g<3 && count<VISITS;g++) {
    if(left[g]==0) continue;
    if(used[g]==0) firstGuest[g]=count;
    int id=4*g+(offset+used[g])%4;
    d->guest[count]=id;
    d->gift[count]=1;
    left[g]--; used[g]++; count++;
   }
  }
  for(int g=0;g<3;g++) d->gift[firstGuest[g]]+=left[g];
  for(int i=0;i<VISITS;i++) {
   int j=i+rand()%(VISITS-i);
   int id=d->guest[j], gift=d->gift[j];
   d->guest[j]=d->guest[i]; d->gift[j]=d->gift[i];
   d->guest[i]=id; d->gift[i]=gift;
   d->stock[stories[id].type]+=gift;
  }
 } while(!find_answer(d));
}
