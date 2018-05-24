#pragma once
#include <cstddef>
#include <list>
#include "uti.h"
#include "Solution.h"
#include "tree.h"
#include "load.h"
//using Pcit=std::list<pthelm>::iterator ;


struct ejectpij_best
{
	int pin, din;//pickup_insert delivery_insert
	//din=0 没有进行insert操作
	std::list<pthelm>::iterator itjpn;
	int rindex;//第几条路径
	std::list<int> ejection;//记录被eject的迭代器
	std::list<std::list<pthelm>::iterator> it_ejection;//被eject的那个那个元素在ppj中的位置迭代器
	
};

class EjectionSearch
{
public:
	Paramenter para;
	int kmax=para.kmax;//最多eject kmax对request
	int jd=1;//delivery位置 [1,2*kmax+1]
	//order tempod;
	int rindex;
	int itjp_index;//pin的位置编号
	//struct Node;
	std::list<pthelm>::iterator itjp;
	Route* tmpr;
	Solution *pS0;
	std::vector<ejectpij_best> record_eject;//记录所有的ejectin&insertion组合，选psum最小的
	std::list<pthelm> *ptmp_path;
	std::list<pthelm> tmppath;
	removepool *temporder;
	g_data *g_d;
	EjectionSearch(g_data *g_d):g_d(g_d) {
		//int jd = 1; int Psum = 0; int Pbest = INT_MAX; int kmax = 2;
		kmax = para.kmax;
		record_eject.reserve(2 * kmax + 2); 
	}
	EjectionSearch(int kmax, g_data *g_d)
		:kmax(kmax),
		g_d(g_d)/*,Psum(Psum),Pbest(Pbest)*/{
		record_eject.reserve(2*kmax+2); //记录ejectin&insertion组合，最多2*kmax+1个元素 从1开始，则[1,2*kmx+2]
	}
// 	void insert_route(std::list<pthelm>& ptmp_path, 
// 		removepool & temporder, Route * pr_origin, std::vector<insertpij>& record_insert, int route_index);
	//void lexicographic_iterativebacktrack();
	void eject_route(Node& root/*std::list<pthelm> &ptmp_path, removepool &temporder*/);
	void lexicographic_iterativebacktrack(int n, Node &root);
	//int insert_chck(std::list<pthelm>::iterator it_pthelmp, std::list<pthelm>::iterator it_pthelmd, int increment);
	bool constraint(const Node &head);
	//return 0 仅仅此条组合错了
	//return -1 自己的子代不用算了
	//return -2 /其之后的兄弟 及 自己和兄弟的子代都不用算了
	//retun 1  正确
	int prune(Node& head, int n, bool increment );
	void traverse(Node& head, int level, int startnum, int endnum);
	void traverse_purn(Node& head, int level, int startnum, int endnum);
	void Search(Node& head, int level, int n);
};

//extern void lexicographic_iterativebacktrack();
