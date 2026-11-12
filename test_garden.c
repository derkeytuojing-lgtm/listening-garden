#include "garden.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
    int coverage[3][3]={0};
    srand(17);
    for (int sample=0; sample<10000; sample++) {
        Day d;
        Plan player={0};
        int visited[12]={0}, mixes=0, groups[3]={0};
        new_day(&d);
        coverage[d.kind][d.weather]++;
        for (int i=0;i<VISITS;i++) {
            int id=d.guest[i];
            assert(!visited[id] && d.gift[i]>0);
            visited[id]=1;
            player.bag[stories[id].type]+=d.gift[i];
        }
        assert(memcmp(player.bag,d.stock,sizeof d.stock)==0);
        for(int i=0;i<TYPES;i++) groups[i/2]+=player.bag[i];
        assert(groups[0]+groups[1]+groups[2]==STOCK);
        bool direct=groups[0]>=3 && groups[1]>=3 && groups[2]>=3;
        assert(direct==(d.kind==0));
        for (int i=0;i<d.answer.count;i++) {
            Move m=d.answer.moves[i];
            assert(apply(&player,m));
            if(m.b>=0) mixes++;
        }
        assert(mixes==(d.kind==0?0:d.kind==1?1:3));
        Result r=evaluate(player.value,d.weather);
        assert(r.perfect && r.growth+r.balance+r.weatherScore==100);
        while(player.count) undo(&player);
        assert(memcmp(player.bag,d.stock,sizeof d.stock)==0);
        for(int i=0;i<3;i++) assert(player.value[i]==0);
    }
    for (int i=0;i<3;i++) for(int j=0;j<3;j++) assert(coverage[i][j]>0);
    for (int a=0;a<TYPES;a++) for(int b=0;b<TYPES;b++) {
        Plan p={0};
        p.bag[a]++; p.bag[b]++;
        Move m={a,b};
        assert(apply(&p,m)==(a/2!=b/2));
        if (a/2!=b/2) {
            assert(!strcmp(recipe(a,b),recipe(b,a)));
            assert(!apply(&p,m) && p.value[target(m)]==10);
        }
    }
    int tree[]={50,30,20};
    assert(evaluate(tree,0).health==50);
    assert(evaluate(tree,1).health==60);
    assert(evaluate(tree,2).health==40);
    puts("PASS: 10000 days, all weather and day types, recipes, scoring, undo");
    return 0;
}
