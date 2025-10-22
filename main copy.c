//
//  main.c
//  苏丹的游戏之变法！
//
//  Created by mei Tong on 10/11/25.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
typedef struct{
    char* name;
    int intellect;
    int charm;
    int military;
    int law;
    int education;
    int finances;
    int politics;
} npc;

typedef struct{
    char* name;
    int value;
} tool;

typedef struct{
    char*level;
    char*type;
    int bfscore;
    int status;
} Sultancard;


typedef struct{
    char* name;
    int completion;
} bf;

typedef struct{
    char* name;
    int (*checkattribute[3])(npc*);//需要哪种个人属性判定
    int requirement[3];//需要的判定点数
    int bfsupport;// 对变法支持度造成的后果
} incidentopt;

typedef struct{
    char* name;
    char* desc;
    incidentopt* options[4];
    char targetLevel[5];  // 触发事件的卡牌等级（如"石"）
    char targetType[10];
} incident;

typedef struct{
    char* book;
    char* desc;
    int price;
    int (*addattname[2])(npc*);//一本书至多能加两种属性
    int addattpoint[2];
} bookstore;

typedef struct{
    char* name;
    int totalsupport;
    int money;
    int bfpoints;
} panel;//个人面板


// 工具函数：获取npc的各种属性（用于事件选项的判定）
int getIntellect(npc* person) { return person->intellect; }
int getCharm(npc* person) { return person->charm; }
int getMilitary(npc* person) { return person->military; }
int getLaw(npc* person) { return person->law; }
int getEducation(npc* person) { return person->education; }
int getFinances(npc* person) { return person->finances; }
int getPolitics(npc* person) { return person->politics; }
//个人面板
panel personalpanel={
    "阿尔图", 20,20,0
};
//下面是人物的名字、智力、魅力、军事能力、法律能力、教育能力、经济能力、立场（支持为正）
npc character[10]={
    {"阿尔图",2,2,2,2,2,2,2},{"奈费勒",5,5,2,5,5,5,5},{"梅姬",4,4,4,4,4,5,2}
};
//下面是书店
bookstore library[8]={
    {"高等数学","一本神秘的读物，但给你带来的是疯狂还是智慧呢？\n",10,getIntellect,NULL,1,0},
    {"线性代数","有人在读完以后精神恍惚，嘴里喊着矩阵行列式就开始狂奔了\n",10,getIntellect,NULL,1,0},
    {"精灵宝钻","似乎是一个阖家欢乐，兄友弟恭，全员存活，得偿所愿的美好HE故事？\n",20,getCharm,getIntellect,2,1},
    {"青苗法","来自神秘的东方古国的一本旧书，似乎承载着一位改革家的夙愿\n", 15, getLaw, getFinances, 1,1},
    {"募役法","你翻开它的书页，似乎看到了一段悠悠往事，和一场转瞬即逝的梦。\n",15, getLaw, getMilitary, 1,1},
    {"资治通鉴","这是一本给帝王的教科书，但是其中守成的思想太重了，或许把它留给你的后人会是更好的选择\n",15, getLaw, getPolitics, 1,1},
    {"政治的艺术：让苏丹再次伟大！","好奇怪，你翻开这本书，脑海里却立刻浮现了一段陌生却慷慨激昂的旋律\n",15, getCharm, getPolitics, 1,1},
    {"教育学：从入门到入土","",15,getEducation,NULL,2,0}
    
};
//下面是苏丹卡的级别属性变法值和是否被使用过
Sultancard cards[] = {
    {"石","杀戮",10, 0}, {"石","杀戮",10, 0},
    {"铜","杀戮",20, 0}, {"铜","杀戮",20, 0},
    {"银","杀戮",50, 0}, {"银","杀戮",50, 0},
    {"金","杀戮",100, 0}, {"石","纵欲",10, 0},
    {"石","纵欲",10, 0}, {"铜","纵欲",20, 0},
    {"铜","纵欲",20, 0}, {"银","纵欲",50, 0},
    {"银","纵欲",50, 0}, {"金","纵欲",100, 0},
    {"石","奢靡",10, 0}, {"石","奢靡",10, 0},
    {"铜","奢靡",20, 0}, {"铜","奢靡",20, 0},
    {"银","奢靡",50, 0}, {"银","奢靡",50, 0},
    {"金","奢靡",100, 0}, {"石","征服",10, 0},
    {"石","征服",10, 0}, {"铜","征服",20, 0},
    {"铜","征服",20, 0}, {"银","征服",50, 0},
    {"银","征服",50, 0}, {"金","征服",100, 0}
};

//下面是变法的具体事务以及变法值
bf content[7]={
    {"教育",0},{"军事",0},{"经济",0},{"人权",0},{"政治",0},{"文化",0},{"科技",0}
};

//下面是各式各样的选项
incidentopt sslopt1={
    "杀戮奈费勒的花瓶",
    {getMilitary, NULL, NULL},
    {3, 0, 0},
    0
};
incidentopt sslopt2={
    "杀戮更多的公文",
    {getIntellect, NULL, NULL},
    {3, 0, 0},
    5
};
incidentopt tslopt1={
    "把他们就地正法",
    {getMilitary, NULL, NULL},
    {4, 0, 0},
    -15
};
incidentopt tslopt2={
    "扭送公堂后就地正法",
    {getLaw, NULL, NULL},
    {4, 0, 0},
    5
};
incidentopt yslopt1={
    "干掉他",
    {getCharm, getMilitary, NULL},
    {7, 7, 0},
    -15
};
incidentopt yslopt2={
    "扭送公堂后就地正法",
    {getLaw, NULL, NULL},
    {6, 0, 0},
    -5
};
incidentopt jslopt1={
    "根基深厚的大臣",
    {getCharm, getMilitary, NULL},
    {9, 9, 0},
    -15
};
incidentopt jslopt2={
    "愣头青",
    {getMilitary, NULL, NULL},
    {7, 0, 0},
    -5
};
incidentopt ssmopt1={
    "孤儿院",
    {getCharm, NULL, NULL},
    {2, 0, 0},
    5
};
incidentopt ssmopt2={
    "街头的混混",
    {getCharm, NULL, NULL},
    {2, 0, 0},
    0
};
incidentopt ssmopt3={
    "流民和乞丐",
    {getCharm, NULL, NULL},
    {3, 0, 0},
    0
};
incidentopt tsmopt1 = {
    "邀请亲朋好友",
    {NULL, getCharm, NULL},
    {0, 3, 0},
    0
};
incidentopt tsmopt2 = {
    "邀请同事小聚",
    {getCharm, NULL, getIntellect},
    {3, 0, 3},
    3
};
incidentopt tsmopt3 = {
    "邀请反对派来一场鸿门宴",
    {getIntellect, getCharm, getPolitics},
    {4, 4, 4},
    -5
};

incidentopt ysmopt1 = {
    "说服贵族们捐钱",
    {getIntellect, getCharm, NULL},
    {7, 7, 0},
    -10
};
incidentopt ysmopt2 = {
    "说服大臣们捐钱",
    {getCharm, getFinances, getIntellect},
    {7, 5, 7},
    -5
};
incidentopt ysmopt3 = {
    "说服商人们捐钱",
    {getIntellect, getCharm, NULL},
    {6, 6, 0},
    5
};
incidentopt ysmopt4 = {
    "自己能捐点就捐点",
    {getIntellect, NULL, NULL},
    {5, 0, 0},
    0
};
incidentopt jsmopt1 = {
    "盖逸夫科技楼",
    {getIntellect, getCharm, getFinances},
    {6, 6, 6},
    10
};
incidentopt jsmopt2 = {
    "盖文化广场",
    {getCharm, getFinances, getIntellect},
    {7, 5, 7},
    10
};
incidentopt jsmopt3 = {
    "盖新的议事厅",
    {getIntellect, getCharm, getPolitics},
    {6, 6, 0},
    15
};
incidentopt jsmopt4 = {
    "盖新的断头台",
    {getIntellect, NULL, NULL},
    {3, 0, 0},
    -20
};
incidentopt szfopt1 = {
    "征服早八",
    {getIntellect, NULL, getPolitics},
    {2, 0, 2},
    5
};
incidentopt szfopt2 = {
    "征服假期！向奈费勒发起挑战！",
    {getCharm, NULL, getIntellect},
    {3, 0, 3},
    0
};
incidentopt szfopt3 = {
    "征服政务！",
    {getIntellect, NULL, getPolitics},
    {2, 0, 3},
    2
};
incidentopt szfopt4 = {
    "征服皇位！图奈一起睡懒觉，从此苏丹不早朝！",
    {getIntellect, NULL, NULL},
    {3, 0, 0},
    -5
};
incidentopt tzfopt1 = {
    "尊重河流的意志",
    {getIntellect, NULL, NULL},  // 需要判定：智力
    {4, 0, 0},                    // 智力要求3点
    0                           // 后果：变法支持值不变
};
incidentopt tzfopt2 = {
    "派人去救灾",
    {getMilitary, getFinances, NULL},  // 需要判定：军事和经济
    {4, 5, 0},                          // 军事要求4，经济要求5
    10                                  // 后果：变法支持值+10
};
incidentopt tzfopt3 = {
    "大声斥责当地",
    {getCharm, NULL, NULL},  // 需要判定：魅力
    {4, 0, 0},                // 魅力要求2点
    -20                        // 后果：变法支持值-5
};
incidentopt tzfopt4 = {
    "规划新河道，控制自然",
    {getIntellect, getLaw, NULL},  // 需要判定：智力和法律
    {5, 3, 0},                      // 智力要求5，法律要求3
    5                             // 后果：变法支持值+5
};
incidentopt yzfopt1 = {
    "你通过讲奈费勒的八卦，征服了苗圃",
    {getIntellect, getCharm, getEducation},
    {5, 5, 5},
    0
};
incidentopt yzfopt2 = {
    "你通过长期投喂，征服了贝姬夫人的猫窝",
    {getCharm, NULL, NULL},
    {6, 0, 0},
    0
};
incidentopt yzfopt3 = {
    "你通过让奈费勒不早朝，征服了议事厅（至于为什么不早朝你别问",
    {getIntellect, getCharm, getPolitics},
    {5, 5, 5},
    2
};
incidentopt yzfopt4 = {
    "你征服了城内的一片混乱街区，建立了更美好的秩序",
    {getIntellect, getCharm, getMilitary},
    {5, 5, 5},
    4
};
incidentopt jzfopt1 = {
    "你依靠写同人文和野史，征服了当下少女的心和后代的史学家（当代fml",
    {getIntellect, getCharm, getEducation},
    {6, 7, 6},
    3
};
incidentopt jzfopt2 = {
    "你依靠举行讲座，征服了国家的知识分子（曰：前苏丹之所以终其一生没有再立宰相，是因为他忘不了abd那一双忧郁的眼睛！",
    {getCharm, getEducation, getIntellect},
    {7, 7, 7},
    0
};
incidentopt jzfopt3 = {
    "你依靠甘愿为玛希尔的研究而献身的精神，征服了整个科学界！",
    {getIntellect, getMilitary, getPolitics},
    {6, 7, 5},
    3
};
incidentopt jzfopt4 = {
    "你依靠派一只猫就能赚钱的奇异本领，征服了整个商界！",
    {getIntellect, getFinances, getCharm},
    {6, 7, 7},
    5
};
incidentopt szyopt1 = {
    "一批起公文来就发狠了！忘情了！",
    {getIntellect, getPolitics, getFinances},
    {2, 2, 2},
    0
};
incidentopt szyopt2 = {
    "一开起会来就发狠了！忘情了！",
    {getCharm, getPolitics, getIntellect},
    {2, 2, 2},
    0
};
incidentopt szyopt3 = {
    "一修起法条来就发狠了！忘情了！",
    {getIntellect, getLaw, getPolitics},
    {2, 2, 2},
    2
};
incidentopt szyopt4 = {
    "一骂起旧宕来就发狠了！忘情了！",
    {getIntellect, getCharm, getPolitics},
    {2, 2, 2},
    4
};
incidentopt tzyopt1 = {
    "畅快的和哲巴尔一起打打猎",
    {NULL, getMilitary, NULL},
    {0, 3, 0},
    0
};
incidentopt tzyopt2 = {
    "和玛希尔一起进行有趣♂的科学研究",
    {getIntellect, getMilitary, 0},
    {3, 3, 0},
    0
};
incidentopt tzyopt3 = {
    "拜访一下夏玛吧",
    {getCharm, NULL, NULL},
    {4, 0, 0},
    2
};
incidentopt tzyopt4 = {
    "举行施粥活动",
    {getIntellect, getCharm, NULL},
    {3, 3, 0},
    4
};
incidentopt yzyopt1 = {
    "进入书店释放一些阅读的欲望",
    {NULL, getIntellect, NULL},
    {0, 6, 0},
    0
};
incidentopt yzyopt2 = {
    "进入苗圃释放一些教学的欲望",
    {getIntellect, getEducation, 0},
    {5, 5, 0},
    0
};
incidentopt yzyopt3 = {
    "进入反对派的家里释放一下联络感情的欲望",
    {getCharm, getMilitary, getPolitics},
    {6, 5, 6},
    2
};
incidentopt yzyopt4 = {
    "找到鲁梅拉和梅姬释放一下贴贴的欲望（一起去看音乐剧吧！",
    {getIntellect, getCharm, NULL},
    {6, 6, 0},
    0
};
incidentopt jzyopt1 = {
    "银瓶乍破水浆迸，铁骑突出刀枪鸣。\n轻拢慢捻抹复挑，从此君王不早朝。",
    {getCharm, getMilitary, NULL},
    {8, 7, 0},
    0
};
incidentopt jzyopt2 = {
    "既然人当上苏丹以后就会猫化，那我想撸猫也是可以的吧(你这是正经撸猫吗",
    {getCharm, getMilitary, NULL},
    {8, 7, 0},
    0
};
incidentopt jzyopt3 = {
    "既然人当上苏丹以后就会猫化，那我想撸猫也是可以的吧(这回是正经撸猫",
    {getCharm, getIntellect, NULL},
    {8, 7, 0},
    0
};
//下面是事件的名称、描述、四个选项、触发事件的卡牌等级与类型
incident matters[]={
    {"被公文逼疯的你","快被无穷无尽的公文逼疯的你，来到了奈费勒的书房里。",&sslopt1,&sslopt2,NULL,NULL,"石","杀戮"},
    {"诋毁变法之人","你在酒吧里，听到了有两个守旧派的小贵族正在蛐蛐变法，甚至在讨论弑君一事。",&tslopt1,&tslopt2,NULL,NULL,"铜","杀戮"},
    {"清除蛀虫","发现官员挪用变法资金，阻碍事务推进",&yslopt1,&yslopt2,NULL,NULL,"银","杀戮"},
    {"朝堂决斗","你可以向朝廷上的一位反对派发起决斗",&jslopt1,&jslopt2,NULL,NULL,"金","杀戮"},
    {"超级大撒币","啊，有钱也是甜蜜的烦恼～把钱给谁好呢",&ssmopt1,&ssmopt2,&ssmopt3,NULL,"石","奢靡"},
    {"宴会！人群！歌舞！美酒！","邀请大家来吃一趟大餐吧",&tsmopt1,&tsmopt2,&tsmopt3,NULL,"铜","奢靡"},
    {"募捐","给苗圃进行一次募捐吧",&ysmopt1,&ysmopt2,&ysmopt3,&ysmopt4,"银","奢靡"},
    {"盖大楼","给改革事业捐一栋楼？哈哈，对我超级大撒币先生而言可是小菜一碟啊。",&jsmopt1,&jsmopt2,&jsmopt3,&jsmopt4,"金","奢靡"},
    {"被工作逼疯的你","我爱工作，工作爱我",&szfopt1,&szfopt2,&szfopt3,&szfopt4,"石","征服"},
    {"河道改道","国家内有一条河流自由的溜达了起来",&tzfopt1,&tzfopt2,&tzfopt3,&tzfopt4,"铜","征服"},
    {"我有地了","你征服了一块小小的地盘",&yzfopt1,&yzfopt2,&yzfopt3,&yzfopt4,"银","征服"},
    {"你有了甚么？","这个征服不寻常",&jzfopt1,&jzfopt2,&jzfopt3,&jzfopt4,"金","征服"},
    {"人被逼急了...","都能对工作下手",&szyopt1,&szyopt2,&szyopt3,&szyopt4,"石","纵欲"},
    {"谁说休假不是一种欲望","我要假期",&tzyopt1,&tzyopt2,&tzyopt3,&tzyopt4,"铜","纵欲"},
    {"还有什么欲望","全方位多层次宽领域的欲望",&yzyopt1,&yzyopt2,&yzyopt3,&yzyopt4,"银","纵欲"},
    {"我不怜悯你了","求您怜悯怜悯我吧",&jzyopt1,&jzyopt2,&jzyopt3,NULL,"金","纵欲"}
};
// 定义在代码开头（函数声明后）
void clearBuffer() {
    int c;  // 用int接收getchar()的返回值，避免EOF判断错误
    while ((c = getchar()) != '\n' && c != EOF) {
        // 循环读取并丢弃所有残留字符，直到遇到换行符或文件结束
    }
}
//下面开始编写展示个人面板的程序
void showpanel(panel*personalpanel,bf*content){
    printf("\n==================== 个人面板 ====================\n");
    printf("玩家姓名：%s\n", personalpanel->name);
    printf("当前金钱：%d  |  剩余变法点：%d  |  变法支持度：%d\n", personalpanel->money, personalpanel->bfpoints, personalpanel->totalsupport);
    printf("\n【变法进度】\n");
    for (int i = 0; i < 7; i++) {
        printf("%s：%d%%  ", content[i].name, content[i].completion);
        if ((i + 1) % 3 == 0) printf("\n"); // 每3个换行，排版好看
    }
    printf("\n==================================================\n");

}

//下面开始编写判定的程序
void gameover(Sultancard* cards, int leftcard, bf* content, const char* name, panel*personalpanel){
    printf("女术士：您的游戏结束了，让我们来结算吧！");
    //检查是否还有苏丹卡
    int hascard = 0;
    for (int i=0; i<leftcard; i++) {
        if(cards[i].status == 0){
            hascard = 1;//还有
            break;
        }
    }
    printf("\n最终变法进度：\n");
    int success = 1;
    for (int i = 0; i < 7; i++) {
        printf("%s: %d%%\n", content[i].name, content[i].completion);
        if (content[i].completion < 100) {
            success = 0;
        }
    }
    if ((success==0)&&(hascard==0)) {
        printf("===========获得结局：尽吾志而不能至===========\n你们折断了所有的苏丹卡。\n但哪怕在卡片力量的加持下，变法却仍然失败了。生活正是如此，有时哪怕有足以杀死太阳的决心，有着足以凌驾万物的力量，却也无法到达那瑰丽险境的尽头。\n或称此谓天命。\n也罢，也罢。你收拾收拾东西，和奈费勒一起搬去了苗圃，也许过上几十年，那里将走出一位能完成你和奈费勒未竟之志的人。\n");
    }
    else if((success==1)&&(personalpanel->totalsupport>50)){
        printf("===========获得结局：不畏浮云遮望眼============\n在日复一日的操劳下，你们终于完成了变法，建立了理想中的王国。在后代的记载中，你和奈费勒的名字永远紧挨着，被作为对后世君臣而言最光荣、最崇高的形容。");
    }
    else if((success==1)&&(personalpanel->totalsupport<=50)){
        printf("===========获得结局：今人未可非商鞅============\n你和奈费勒成功推行了所有的政策。\n可惜，苍天无情，你的苏丹在未能看到政策成效之前就早早离去，新苏丹在愤怒的贵族的压力之下，下令将你五马分尸。\n不幸中的万幸？你们的政策延续下来了。\n");
    }
}
//下面编写对各种时间投骰子的程序
void rollcheck(Sultancard* currentcard, bf*content, const char*name, incident*matters, incidentopt* opt, panel*personalpanel){
    npc* selectednpc=&character[0];
    if (opt == NULL) {
        printf("选项无效，跳过判定\n");
        return;
    }
    printf("这种事一般不能假手于人，必须得自己上\n喜闻乐见的判定时间到了！");
    printf("\n=== 选项判定: %s ===\n", opt->name);
    int success = 1;//先假设成功
    int total[3];
    for (int i=0; i<3; i++) {
        if (opt->checkattribute[i]!=NULL) {
            int dice=rand()%6;//六面骰子 0-5
            int npc_value=opt->checkattribute[i](selectednpc);//传送选择的npc的数值
            total[i]=npc_value+dice;
            printf("判定属性: ");
                        // 用“函数指针地址对比”判断当前是哪个属性（因为每个getXXX函数地址唯一）
                        if (opt->checkattribute[i] == getIntellect) printf("智力");
                        else if (opt->checkattribute[i] == getCharm) printf("魅力");
                        else if (opt->checkattribute[i] == getMilitary) printf("军事");
                        else if (opt->checkattribute[i] == getLaw) printf("法律");
                        else if (opt->checkattribute[i] == getEducation) printf("教育");
                        else if (opt->checkattribute[i] == getFinances) printf("经济");
            printf(" (要求: %d) - NPC值: %d, 骰子: %d, 总计: %d\n", opt->requirement[i], npc_value, dice, total[i]);
            if (total[i]<opt->requirement[i]) {
                success--;
            }
        }
        
    }
    if (success!=1) {
        printf("失败！");//还可以设计重投和金骰子
        currentcard->status=0;
    }
    else{
        personalpanel->totalsupport+=opt->bfsupport;
        printf("成功！您折断了一张%s%s卡！",currentcard->level,currentcard->type);
        personalpanel->bfpoints+=currentcard->bfscore;
        if(currentcard->status==0){
            currentcard->status=1;
        }//折断卡了
        //注意⚠️理论上要写一段变法支持度变化的 但是打算把totalsupport放在主程序里 或者再struct一个个人面板之类的东西 往上翻 有bfmattersprintf("失败！");
        // 检查是否还有未使用的卡
        int hasUnusedCard = 0;
        int leftcard=28;
        for (int i = 0; i < 28; i++) {
            if (cards[i].status == 0) {
                hasUnusedCard = 1;
                break;
            }
        }
        if (!hasUnusedCard){
            gameover(cards, leftcard, content, name, personalpanel);
        }
    }
}
//赚钱mode
void earnmoney(bf*content, const char*name, panel*personalpanel){
    int rollnpc;  // 足够存NPC名字（如“阿尔图”）
    npc* selectednpc=NULL;
    printf("您选择了治理家业，当前可用队友：\n");
    for (int i = 0; i < 3; i++) {  // 目前定义了3个NPC（阿尔图、奈费勒、梅姬），所以循环3次
        if (character[i].name != NULL) {  // 防止名字为空的无效NPC
            printf("%d. %s ", i + 1, character[i].name);  // 显示“1.阿尔图”这样的选项
        }
    }
    clearBuffer();
    printf("请输入您想让哪位队友去治理家业：");
    scanf("%d", &rollnpc);
    selectednpc=&character[rollnpc-1];
    if (selectednpc == NULL) {
        printf("未找到该队友，治理家业失败！\n");
        return;
    }
    int dice=rand()%3;//六面骰子 0-5
    int npc_value=selectednpc->charm+selectednpc->intellect;//传送选择的npc的数值
    int total=npc_value+dice;
    printf("判定属性: \n");
    printf("你选择了%s，ta的魅力和智慧一共有%d点，您roll到了%d点。\n",selectednpc->name,selectednpc->charm+selectednpc->intellect,dice);
    if (total>=5) {
        printf("恭喜，你赚到了10块金币！\n");
        personalpanel->money+=10;
    }
    else if ((total>=3)&&(total<5)) {
        printf("恭喜，你赚到了5块金币！\n");
        personalpanel->money+=5;
    }
    else if ((total>=1)&&(total<3)) {
        printf("恭喜，你赚到了3块金币！\n");
        personalpanel->money+=3;
    }
    else{
        printf("哈哈哈，你今天空手而归！\n");
    }
}
//下面开始编写去书店的程序
void buybooks(bookstore*readbooks, panel*personalpanel){
    int readerseq=0;
    int bookseq = 0;
    npc* selectednpc;
    printf("欢迎来到书店，您想要什么书都应有尽有！\n我们目前有：\n");
    for (int i=0; i<8; i++) {
        printf("\n%d. %s，价钱是%d",i+1,library[i].book,library[i].price);
    }
    clearBuffer();
    printf("您要买哪本？请输入序号：");
    scanf("%d",&bookseq);
    getchar();
    bookseq-=1;
    readbooks=&library[bookseq];
    if (readbooks->price<=personalpanel->money) {
        personalpanel->money=personalpanel->money-readbooks->price;
    }
    else{
        printf("您的钱不够！");
        return;
    }
    printf("%s",library[bookseq].desc);
    printf("您想让谁来读这本书？您的队友有：\n");
    for (int i=0; i<3; i++) {
        if (character[i].name!=NULL) {
            printf("%d. %s",i+1, character[i].name);
        }
    }
    clearBuffer();
    printf("请输入序号：");
    scanf("%d",&readerseq);
    readerseq-=1;
    selectednpc=&character[readerseq];
    for (int i=0; i<2; i++) {
        if (readbooks->addattname[i]!=NULL) {
            if (readbooks->addattname[i]==getIntellect) {
                selectednpc->intellect+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的智力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getCharm) {
                selectednpc->charm+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的魅力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getEducation) {
                selectednpc->education+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的教育能力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getMilitary) {
                selectednpc->military+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的战斗能力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getPolitics) {
                selectednpc->politics+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的政治才能增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getFinances) {
                selectednpc->finances+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的经济能力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
            if (readbooks->addattname[i]==getLaw) {
                selectednpc->law+=readbooks->addattpoint[i];
                printf("读完这本书后，%s的法律能力增长了%d点！\n",selectednpc->name,readbooks->addattpoint[i]);
            }
        }
    }
    printf("欢迎下次光临!");
}
//变法
void revolution(bf*content, panel*personalpanel, const char*name){
    printf("\n我们来变法吧！");
    printf("当前变法进度：\n");
    for (int i=0; i<7; i++) {
        printf("%s: %d%%\n", content[i].name, content[i].completion);
    }
    
    while (1) {
        int ans;  // 用整数接收“1/0”，比字符串更可靠
        clearBuffer();  // 关键：每次循环前先清空缓冲区
        printf("\n您当前拥有的变法点为：%d，您要变法吗？（是=1 / 否=0）：", personalpanel->bfpoints);
        
        // 读取输入并检查是否有效
        if (scanf("%d", &ans) != 1) {  // 如果输入不是数字，视为“否”
            printf("输入无效，退出变法。\n");
            clearBuffer();  // 清除无效输入
            break;
        }
        
        if (ans == 1) {  // 用户选择“是”
            if (personalpanel->bfpoints < 10) {
                printf("变法点不足10，无法变法！\n");
                break;
            }
            
            int bfchoice = 0;
            clearBuffer();  // 读取序号前清空缓冲区
            printf("请选择变法事务（输入序号1-7）：\n");
            printf("1.教育 2.军事 3.经济 4.人权 5.政治 6.文化 7.科技\n");
            printf("您的选择：");
            
            if (scanf("%d", &bfchoice) != 1) {  // 检查序号输入是否有效
                printf("输入无效，退出变法。\n");
                clearBuffer();
                break;
            }
            
            // 序号范围检查
            if (bfchoice <1 || bfchoice >7) {
                printf("输入序号错误！\n");
                continue;
            }
            bfchoice -=1;  // 转数组索引
            
            // 更新变法进度（原有逻辑）
            personalpanel->bfpoints -=10;
            if (personalpanel->totalsupport >=100) {
                content[bfchoice].completion +=30;
            } else if (personalpanel->totalsupport >=60) {
                content[bfchoice].completion +=20;
            } else if (personalpanel->totalsupport >=40) {
                content[bfchoice].completion +=10;
            } else if (personalpanel->totalsupport >=20) {
                content[bfchoice].completion +=5;
            } else {
                content[bfchoice].completion +=2;
            }
            if (content[bfchoice].completion >100) {
                content[bfchoice].completion =100;
            }
            printf("变法成功！%s进度更新为：%d%%\n", content[bfchoice].name, content[bfchoice].completion);
            
        } else {  // 用户选择“否”（任何非1的输入）
            printf("退出变法。\n");
            break;
        }
    }
}

//下面写的是触发苏丹卡事件
void TriggerIncident(Sultancard* currentcard, incident* matters, const char* name, panel*personalpanel){
    incident* matchedIncident = NULL;
    int mattersnum=16;
    for (int i = 0; i < mattersnum; i++) {
    // 对比卡片的等级（如“铜”）和类型（如“奢靡”）与事件的触发条件
        if (strcmp(currentcard->level, matters[i].targetLevel) == 0 && strcmp(currentcard->type, matters[i].targetType) == 0) {
            matchedIncident = &matters[i]; // 找到匹配的事件
            break;
        }
    }
    //如果还没有对应事件
    if (matchedIncident==NULL) {
        printf("暂无对应事件，直接获得变法点。\n");
        personalpanel->bfpoints+=currentcard->bfscore;
        currentcard->status=1;
        return;
    }
    printf("触发事件:%s",matchedIncident->name);
    printf("\n事件描述:%s",matchedIncident->desc);
    printf("\n可选择的选项：");
    int validOptions[4];  // 记录有效选项的索引
    int validCount = 0;
    for (int i = 0; i < 4; i++) {
        if (matchedIncident->options[i] != NULL) {
            validOptions[validCount] = i;  // 保存有效选项的真实索引
            printf("\n%d. %s", validCount + 1, matchedIncident->options[i]->name);
            validCount++;
        }
    }
    // 强制等待用户输入选项（核心修复：确保输入被正确读取）
        int optchoice;
        while (1) {  // 循环直到输入有效
            clearBuffer();  // 彻底清空缓冲区
            printf("  请输入选项序号（1-%d）：", validCount);
            if (scanf("%d", &optchoice) == 1 && optchoice >= 1 && optchoice <= validCount) {
                break;  // 输入有效，退出循环
            }
            printf("输入错误，请重新输入！\n");
        }
    incidentopt* selectedopt = matchedIncident->options[optchoice - 1];
    if (selectedopt == NULL)
    {
        printf("没这个选项\n");
        return;
    }
    rollcheck(currentcard, content, name, matters, selectedopt, personalpanel);
}

//下面以七天为单位
Sultancard* currentcard = NULL;  // 全局变量，整个程序可用
void week(bf*content, panel*personalpanel, int leftcard, const char*name, incident*matters, bookstore*readbooks){
    leftcard = 28;
    int dontgo=0;
    // 每周一开始就抽卡，赋值给全局currentcard
    // 检查是否有未使用的卡
    int hascard = 0;
    for (int i=0; i<leftcard; i++) {
        if(cards[i].status == 0){
            hascard = 1;
            break;
        }
    }
    // 随机抽一张未使用的卡，赋值给全局 currentcard
    currentcard = NULL;  // 先清空
    while (currentcard == NULL) {  // 确保抽到有效卡
        int cardnum = rand()%leftcard;
        if (cards[cardnum].status == 0) {
            currentcard = &cards[cardnum];
        }
    }
    printf("本周抽到的卡：%s%s卡（基础变法点：%d）\n", currentcard->level, currentcard->type, currentcard->bfscore);
    for (int i=7; i>=1; i--) {
        printf("\n==================== 第%d天 ====================\n", 8-i);
        printf("离处刑日还有%d天！\n", i);
        printf("今天想做什么？");
        printf("1.去变法  2.去书店  3.治理家业  4.跳过今天  5.查询个人面板 6.折断苏丹卡");
        int choice;
        clearBuffer();
        printf("   请输入选择（数字1-6）：");
        scanf("%d", &choice);
        getchar();  // 清除换行符（和 clearBuffer 功能重复，可保留）
        switch(choice){
            case 1:
                revolution(content, personalpanel, name);
                break;
            case 2:
                buybooks(readbooks, personalpanel);
                break;
            case 3:
                earnmoney(content, name, personalpanel);
                break;
            case 4:
                break;
            case 5:
                showpanel(personalpanel, content);
                break;
            case 6:
                // 触发当前卡的事件（全局 currentcard 已指向本周抽到的卡）
                if (currentcard->status == 0) {
                    TriggerIncident(currentcard, matters, name, personalpanel);
                } else {
                    printf("这张卡已经折断了！\n");
                }
                break;
                
        }
        // 询问是否继续下周
        if (currentcard->status == 1) {
            printf("\n是否继续下一周？（输入1继续，其他键结束）：");
            int continuechoice;
            scanf("%d", &continuechoice);
            getchar(); // 吸收换行符
            if (continuechoice != 1){
                dontgo=1;
                printf("=========游戏结束=========\n");
                printf("\n最终变法进度：\n");
                for (int i = 0; i < 7; i++) {
                    printf("%s: %d%%\n", content[i].name, content[i].completion);
                    if (content[i].completion < 100) {
                    }
                }
                printf("=========获得结局：茫然忘了邯郸道=========\n你曾经日复一日、夜复一夜地俯身研究那些法令政策，可换来的却只是朝廷上的反对和民间的谩骂。时常，你站在青金石宫的窗口看向灰暗的天空，思索自己做的这一切究竟有何意义。\n而梅姬死了。\n自然，你有一百个理由可找：你手头有一份新的法案、关系到全国上下农民的命运；一个愚蠢但实在权势强大的贵族和他人发生了冲突；你每日都要焦头烂额地面对半个朝廷的攻讦，就好像你不是带领他们推翻前苏丹、将他们从暴虐残酷中拯救的人。你……\n你忽视了那些微小的迹象、错过了梅姬的病重，只赶上了她的葬礼。\n你不顾奈费勒的再三挽留，在一块风水宝地开辟了一个小院子，贝姬夫人和你在那里住了很多年，久到你几乎不再关注王宫传来的消息。\n");
                break;
            }
            else{
                week(content, personalpanel, leftcard, name, matters, library);
            }
        }
    }
    if (dontgo==0) {
        printf("\n----- 第8天（处刑日） -----\n");
        if (currentcard->status == 0) {
            printf("本周的苏丹卡没有折断，你被贬了！\n");
            printf("=========游戏结束=========\n");
            printf("最终变法进度：\n");
            for (int i = 0; i < 7; i++) {
                printf("%s: %d%%\n", content[i].name, content[i].completion);
                if (content[i].completion < 100) {
                }
            }
            if (strcmp(currentcard->type,"奢靡")==0) {
                printf("=========获得结局：明月何时照我还=========\n你难以折断那一张奢靡卡。\n连年的大旱已经花光了国库的每一分钱，社会上开始流传这样一则预言：罢免新法和维齐尔，老天就会降下霖雨。尽管奈费勒坚决反对，但你仍然坚持自请离去。在你离开青金石殿的那天，果真降下了绵绵细雨。\n");
                return;
            }
            else if (strcmp(currentcard->type,"杀戮")==0) {
                printf("=========获得结局：纵被春风吹作雪=========\n你没来得及折断那一张杀戮卡。\n奈费勒死于一场意外的刺杀。在苏丹的葬礼上，愤怒的旧贵族把你包围了起来，利箭带着死亡的尖啸刺向你。你跌跌撞撞地跑到奈费勒的棺椁边上，鲜血和箭羽装点了厚重的木材。这是一场葬礼，死者却不止一位。\n");
                return;
            }
            else if (strcmp(currentcard->type,"征服")==0) {
                printf("=========获得结局：=========\n你的征服卡最终还是没能折断。\n改革甫一开始，你们就急不可待地颁布了各种政策，你操纵政局的能力让朝堂上下很快就是清一色的支持。\n在那时，征服卡是最容易折断的————这里有领主不肯恢复他奴隶的自由身，你便征服他的领地；那里有聚众抗议新税法的暴民，你便镇压片片躁动。可在高歌猛进的改革之下，诋毁的言辞渗透着你们臣民的心。当人们视新政如啖人猛虎时，执政者就是那放虎出山的恶人。\n可这次，当你手握征服的权柄时，社会上下诡异的平静。执新法案立于高堂的你低下头，却只对上了一双双淡漠眼睛。\n征服土地过于容易，以至于你忘了，最难征服的是人心。\n");
                return;
            }
            else if (strcmp(currentcard->type,"纵欲")==0) {
                printf("=========获得结局：妾亦不忘旧时约=========\n你试了很多办法折断那一张纵欲卡。\n不是和梅姬，在改革的初期，为了防止她被反对派陷害，你就让阿迪莱带着她离开了这个国家。\n不是和欢愉之女们————如果有什么政绩是你最为骄傲的，那一定是废除欢愉之馆，把里面的姑娘们都送去学一两门手艺，让她们从此以后能靠自己的劳动更生。当然，这也意味着消除纵欲卡变得困难了。\n那其他的欲望呢？休假的、读书的、去苗圃看看孩子们的————这些欲望呢？\n也没有。你轻笑一声答道。我也没有这些欲望啦，工作太繁忙就会是这样吧。\n那奈费勒呢？\n奈费勒呢？\n奈费勒早早的死去了————是你的错，你让他当了苏丹，让他成了一个招牌、一个靶子。你带着人逃出宫殿又第二次带着他们杀进青金石宫，你再次修订、颁布、执行法律————这次的效果异常的好，毕竟，反对者的头还在城门外挂着呢。你找来女术士，和她讨价还价，一再拖延折卡的期限————直到处刑日最终到来。\n哥们，算我求你了。奈布哈尼看着那张空位子。哪怕你说你最大的欲望是想当苏丹我都不会有意见的。\n其实你也不知道奈费勒会怎么想，毕竟，你们的改革还没有进行彻底。但是，奈费勒，你这个该死的禁欲教团头头。第八日的晨光照亮了你摩挲着的那张苏丹卡，我都为你禁欲这么久了，你就不能容许我这一次的放纵吗？\n");
                return;
            }
            return;
        }
        else{
            return;
            
        }
    }
    
}

// 函数前置声明（告诉编译器这些函数的存在及参数类型）
void showpanel(panel* personalpanel, bf* content);
void gameover(Sultancard* cards, int leftcard, bf* content, const char* name, panel*playerpanel);
void rollcheck(Sultancard* currentcard, bf* content, const char* name, incident* matters, incidentopt* opt, panel* personalpanel);
void earnmoney(bf* content, const char* name, panel* personalpanel);
void revolution(bf* content, panel* personalpanel, const char* name);
void TriggerIncident(Sultancard* currentcard, incident* matters, const char* name, panel* personalpanel);
void week(bf*content,panel*personalpanel,int leftcard,const char*name,incident*matters, bookstore*readbooks);
void buybooks(bookstore*readbooks,panel*personalpanel);
void clearBuffer();
int main() {
    
    char name[21];  // 定义一个能存20个字符的字符串（留1位给 '\0'）
    printf("你和奈费勒成功建立了新的国家，但旧制度仍然在禁锢着发展，为了改变在前苏丹手中积贫积弱的局面，你和奈费勒开始了轰轰烈烈的变法……\n但就在这时，女术士又出现了，她带来了一套新的卡牌：《苏丹的游戏•变法》。这套卡牌和之前的虽然名称数目一样，但在使用方法和目的上却很不一样。\n");
    printf("总之，欢迎来到“苏丹的游戏之变法！”，在这个游戏里，你（阿尔图）要扮演一位拥有神秘卡片的男子，通过折断卡片（或者其他神秘方式）获得变法值。（当然，不同品阶的变法值不同。)变法值是您变法成功的关键！您可以用变法值去不断完成各项变法事务，以来取得最后的成功。\n");
    printf("给自己取个名字吧(只支持英文且小于20个字母):\n (默认为阿尔图)");
    fgets(name, 20, stdin);  // 最多读19个字符，自动加 '\0'
    unsigned long int len = strlen(name);
    if (len > 0 && name[len - 1] == '\n') {
        name[len - 1] = '\0';  // 去掉末尾的换行符
    }
        // 判断输入是否为空（去掉换行符后长度为0）
    if (strlen(name) == 0) {
        strcpy(name, "阿尔图");  // 空输入时，设置为"Arzu"
    }
    printf("\n女术士：我是一个会具体问题具体分析的术士，俗话说，啥锅配啥盖，啥苏丹配啥牌，强国有酥油，请苏丹放心！\n欢迎欢迎,%s, 来抽你的第一张苏丹卡吧!\n",name);
    
    int leftcard=28;
    srand((unsigned int)time(NULL));
    
    printf("女术士：新版苏丹卡依然延续了良好的7天传统！然而，如果7天没能销卡，迎来的不是死亡--而是变法失败。怎么样，奈老师，这是不是比杀了你更让你难受啊？\n");
    int totalweeks=0;
    totalweeks++;
    week(content, &personalpanel, leftcard, name, matters, library);

    return 0;
}
