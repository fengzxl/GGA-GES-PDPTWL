

//#include "stdafx.h"
#pragma once
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <string.h>
#include <stack>
#include <set>
#include <float.h>
#include <limits.h>
#include "load.h"
using Pcit = std::list<pthelm>::iterator;
using Riterator = std::list<Route>::iterator;
//#define CHECK
#define PRINTF
#define WRITE
// #define GES_BASE
// //#define GES_ROUTE 
class Route;
struct Sol_var //更新Solution中的参数
{
	int cv_total, twv_total, route_num,  pathelm_totalnum;
	double twdiff_total, dis_total, cost_total;
	size_t hash_key;//记录大小的数据类型
	void clearall() { memset(this, 0, sizeof(Sol_var)); }
	void updataIter(Route* r)
	{
		cv_total += r->cv;
		twv_total += r->twv;
		twdiff_total += r->twdiff;
		dis_total += r->dis;
		cost_total += r->cost();
		pathelm_totalnum += r->path_length;
	}


};
struct odpij
{
	Route* pr;
	//Route pr_insert;
	//using Pcit = std::list<pthelm>::const_iterator;

	std::list<pthelm>::iterator itjp, itjd;
	std::list<pthelm>::iterator itip, itid;//sl.itip/d是pri的一对Pickup/delievery点，
	double delta;
	odpij() {}
	odpij(Pcit& i, Pcit j, Route*k):pr(k),itip(i), itid(j)
	{
		delta = DBL_MAX;
	}
	//bool operate()


};

struct removepool
{
	using iter = std::list<pthelm>::iterator;
	order* remove_order;//减少路径个数时 存储删掉那条路径上的order, 和 eject的order
	std::list<pthelm>::iterator it_pthelmp;
	std::list<pthelm>::iterator it_pthelmd;
	removepool(order* ro, iter& itp, iter& itd) :remove_order(ro), it_pthelmp(itp), it_pthelmd(itd) {

	}
};

struct Perturb
{
	std::list<pthelm>::iterator it_pthelmp;
	std::list<pthelm>::iterator it_pthelmd;
	Riterator itroute;//第几条路径
	//double ccost = 0;//放在这个位置时的cost
};
struct infro_remove
{
	std::list<pthelm>::iterator itjpn, itjdn;
	Riterator it_route;
};

struct Squeeze_Perturb
{
	std::list<pthelm>::iterator it_pthelmp, it_pthelmp_erase;
	std::list<pthelm>::iterator it_pthelmd, it_pthelmd_erase;
	Riterator itroute;//第几条路径
	int violation = 0;//不合法的解 的约束违反量
	double cost_delate = 0;//合法解 的cost增加量

};
struct Paramenter
{
	int Irand = 100;
	double Pex = 0.5;
	//double pex2 = 0.5;
	int kmax = 2;
	int i = 0;
	//int num[6] = {1,2,4,6,8,10};
	int ite_num[10] = { 100,100,100,100,100,100,100,100,100,100 };
	int TIME[10] = { 500,3600,7200,10800,6000,8000,500,500,500,500 };
};
struct insertpij   //记录有关信息
{
	int pindex;//插入pickup的位置标号
	int dindex;//插入delivery的位置标号
	int rindex;//第几条路径 从0开始
	std::list<pthelm>::iterator itjp, itjd;
	double dcost = 0;
};
class population {

};
class Solution
{
public:
	int snum;
	int Psum = 0;
	int Pbest = INT_MAX;
	int Irand;
	double total_cost = 0;
	double total_dist = 0;
	double minTime = 0;
	double total_time = 0;
	double fitness = 0;
	double _fitness = 0;
	//int minIter = 0;
	int removebad=0;
	int removerand=0;
	int tess=0;
	double Pex;
	double Pex2;
	order *od;
	g_data *g_d;
	Solution* pbest;
	std::list<Route> R;
	std::list<Route> tmpR;
	std::vector<int> p;//penalty counters惩罚计数器 p[order.id]
	std::vector<int> slifo;
	//std::vector<int> vresult;
	std::vector<removepool> removed;
	std::unordered_map<size_t, double> hashtable;
	 
	Paramenter PARA;
	//Route tmproute;
	char buff[1000];
// 	Sol_var current;//当前解中包含的变量
// 	Sol_var record;
	
	Solution(g_data *g_d,int SolNum):g_d(g_d){
		_assert(g_d->cus_num != 0);
		p.resize((g_d->cus_num + 1) / 2);

		Irand = PARA.Irand;
		Pex = PARA.Pex;
	//	Pex2 = PARA.pex2;
		snum = SolNum;
		od = g_d->pdd;
		//sizeof(R);
	}
	Solution(const Solution& sol,int SolNum):slifo(20) {
		copy(sol, SolNum);
	}
	void copy(const Solution& sol, int SolNum) {
		*this = sol;
		this->snum = SolNum;
		this->pbest = sol.pbest;
		updataAll();
	}
	Solution(Solution&) = delete;
	~Solution() {
	}
	void updataAll() {
		for (auto it = R.begin(); it != R.end();++it)updata(it);
	}
// 	double getCost() const {
// 		return current.cost_total;//只是一条路径上的cost，需要迭代计算一个解所有路径的总coat
// 	}
// 	double getconstDist() const {
// 		double ii = 0;
// 		for (auto &i : R)ii += i->dis;
// 		return ii;
// 	}
	size_t gethashvalue() {
		const size_t _FNV_offset_basis = 14695981039346656037ULL;
		const size_t _FNV_prime = 1099511628211ULL;
		size_t _VAL = _FNV_offset_basis;
		for (auto &itR : R)
		{
			for (auto &itp : itR.path)
			{
				_VAL ^= (size_t)itp.pcus->cusid;
				_VAL *= _FNV_prime;
			}
		}
		return _VAL;
	}
	void add2hashtable(size_t code,double cost) {
		hashtable[code] = cost;
	}
	bool findinhashtable(size_t _val) const {
		return hashtable.find(_val) != hashtable.end();
	}
// 	void clear_hashtable() {
// 		hashtable.clear();
// 	}
// 	
	void construction_sequential();
	void ejection_insertion(int utime);
	void ejection_insertion();
	void ejection_cross(std::list<Route>::iterator se, int i);
	
	void insert_route(std::list<pthelm>& tmppthelm, removepool & temporder, Route * r, std::vector<insertpij>& record_insert, int route_index);
	double perturb(int irand);
	void order_perturb(int irand);
	double ord_perturb(int irand);
	double seg_perturb(int irand);
	void perturbD(Riterator rit, Route* itroute,
		std::list<pthelm>::iterator itjp, std::list<pthelm>::iterator it_pthelmd, std::list<pthelm>::iterator itdn, 
		std::vector<Perturb>& record_perturb/*, double oricost*/);
	void perturbPD(Riterator rit, Route* itroute,
		std::list<pthelm>::iterator it_pthelmp, std::list<pthelm>::iterator it_pthelmd, std::vector<Perturb>& record_perturb/*, double oricost*/);
	void seg_perturbPD(Riterator rit, Route* itroute,
		std::list<pthelm>::iterator it_pthelmp, std::list<pthelm>::iterator it_pthelmd, std::vector<Perturb>& record_perturb);
	void  localSearch(std::vector<int> &tabulist, std::list<Route> &R_best_child, int k, int &isrestart);
//	void UpdateSol();
 
	 
	 
// 	void Squeezing_Removal(const std::set<order*>& D, Route &tmproute1);
// 	void _f_i_in_k(Route *pri, Route *prj, odpij &sl);
// 	//void insert();
// 	void Regret_K(std::set<order*>& D, int k);
// 	void Squeezing_K(std::set<order*>& D, int k, double &violation,std::set<order*>& D_insert, Route &tmproute1);
// 	bool perturb_LNS(std::set<order*>& D, int k, Route &tmproute1, std::vector<infro_remove> D_record);
	void checkR(size_t removesize, bool istwv);
	void tabuList();
	int updata(Riterator itr);
	void updata(Riterator itr, int & violation);
public:

	int insert_chck(std::list<pthelm>::iterator it_pthelmp, std::list<pthelm>::iterator it_pthelmd, int increment);

};

