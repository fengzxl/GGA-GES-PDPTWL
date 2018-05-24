#pragma once
#include <fstream>
#include <list>
//#include <initializer_list>
#define g_MAXN 1100
#define MAX_CUS_NUM 1100
struct customer;
struct order;
struct g_data;
class Route;
using Riterator = std::list<Route>::iterator;
struct pthelm //路径上的节点 元素
{
	customer *pcus;
	order *porder;
	Riterator proute;
	//Route *proute;
	//int ind;//位置标号 < 0 1 2 3 4 5 6 7 ...n n+1>  0&n+1是仓库deplot	
	int q=0;//在此节点时的负载
	int isejected = 0;//是否已经被eject掉
	int eld=0;//离开时负载
	double st = 0, et = 0;//到达时间 离开时间
	double lpt = 0;//zi in paper  latest possible time 最晚离开时间
	double atime = 0;//到达时间 eject insert时更新
	double lcost=0;//离开时代价
	double disfrmlst=0;//从上一点到这一点的distan
	pthelm(customer* pc, order* po, Riterator pr=Riterator())
		:pcus(pc), porder(po), proute(pr)
	{	
	}
	pthelm() {}

};

struct customer
{

	//protected:
	int cusid;
	int x;
	int y;
	int demand;
	int etime;
	int ltime;
	int stime;
	int pid;
	int did;
	//bool p;//customer类型
	int p;
	int d;
	order *porder;
};
#define SN 20
struct order//订单包括pickup和delivery
{
	int id;
	int checked = 0;
	customer *pp;
	customer *pd;
	//pthelm* mat[SN][2];
	std::list<pthelm>::iterator pelmp[SN+13], pelmd[SN+13];

	//pthelm *pelmp[SN], *pelmd[SN];
};
struct g_data
{
	double pcross = 0.8;
	double plocal = 0.2;
	double ejmut = 1.5;//2*1.5 bad+rand
	int PolN=8;//种群个体数
	double leastp = 0.05;
	double mostp = 0.1;
	double goodcross =  2.0 / 3.0;
	int badmut =  1;
	int baseR = 0;//外部赋值
	//int testt;
	int pdd_index = 0;
	int vnum = -1;
	int cus_num = 0;
	int cap = -1;
	int speed = -1;
	double begin_time = 0;
	

	std::ofstream each_problem_output;
	order pdd[1000];
	customer allcustomer[MAX_CUS_NUM];
	double dist[g_MAXN][g_MAXN];
	//extern order* pd;//creat order
	int load(const char *openfile);
	int load1(const char *openfile);

	void creat_order();

};
class Route
{
public:
	//customer* c;
	int path_length = 0;//路径长度<0 1 2 3 ... n n+1>
	int cv = 0;
	int twv = 0;// time window violation
	int cap;
	int snum;
	int no;
	double route_cost=0;
	double route_dist=0;
	double twdiff=0, twwait=0, tvt=0;
	double dis=0;
	double(*dist)[MAX_CUS_NUM];
	std::list<pthelm> path;//组成一条路径的所有元素
	Route(g_data* g_d,int snum):cap(g_d->cap),snum(snum) ,dist(g_d->dist)
	{

	}

	int updata();
	void updata(int &violation);
	int updata_lcheck(double &diss)const;
	int updata_check(int violation)const;
	void lastpossible();
	//int HillClimbing(int thrHill);
	double cost() const;
	double cost(double diss, double twait) const;

	double getcostonly() const;
	//int insert_chck(std::list<pthelm>::iterator it_pthelmp, std::list<pthelm>::iterator it_pthelmd, int increment);

};


constexpr unsigned int New_block = 5 * 1024 * 1024;
struct MemPool
{
	typedef char** next_free;
	typedef char* ptr_d;
	MemPool() {
		for (int k = 0; k <= 31; k++)
			mem_buffer[k] = nullptr;
	}
	~MemPool()
	{
		while (pblock != nullptr)
		{
			ptr_d tmp = pblock;
			pblock = *(next_free)tmp;
			free(tmp);
		}
	}
	ptr_d mem_buffer[32];
	ptr_d pblock = nullptr;
	unsigned int block_num = New_block;
	ptr_d get_mem(size_t size) {
		ptr_d mptr;
		int round_up_demand = (size + 7) & ~7;//按照8字节对齐
		int k = (round_up_demand >> 3) - 1;
		if (mem_buffer[k] == nullptr)
		{
			if (block_num + round_up_demand > New_block)
			{
				ptr_d newblock = ((char *)malloc((New_block) * sizeof(char)));
				*(next_free)newblock = pblock;
				pblock = newblock;
				block_num = 8;
			}
			mptr = pblock + block_num;
			block_num += round_up_demand;
		}
		else
		{
			mptr = mem_buffer[k];
			mem_buffer[k] = *(next_free)mptr;
		}
		return mptr;
	}
	void giveback_mem(void *mptr, size_t size) {
		int k = ((size + 7) >> 3) - 1;
		*(next_free)mptr = mem_buffer[k];
		mem_buffer[k] = (ptr_d)mptr;
	}
};

enum EnumPdp
{
	pdp100 = 0, pdp200, pdp400, pdp600, pdp800, pdp1000,pdp1200,pdp1400,pdp1600,pdp1800
	// We only use the 100-600 scale example on Li and Lim’s data set [21], which is  available  at  http ://www.sintef.no/projectweb/top.

};
enum type1
{
	lc = 1, lr, lrc
};
enum type2 {
	narrow = 1, wide
};

#include <sys/timeb.h>
class util {
public:
	static double nowtime() {
		timeb rawtime;
		ftime(&rawtime);
		return static_cast<double>(rawtime.millitm + rawtime.time * 1000);
	}
};
#ifdef _WIN32
#define my_ERROR \
{printf("ERROR in File:%s, Line:%d\n",__FILE__,__LINE__);\
system("pause");}

#else
#define my_ERROR \
{printf("ERROR in File:%s, Line:%d\n",__FILE__,__LINE__);getchar();}
#endif



#define _assert(str) \
if(!(str)){printf(#str "\n");    \
my_ERROR \
}
