//#include "stdafx.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include "Solution.h"
#include "ejection.h"
#include "tree.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

using namespace std;
//double LTIME = 800.0;//
using std::advance;
double Solution::seg_perturb(int rand_order)
{
	//随机选一个request 除了removed之外的
	// simple relocate
	// extended exchange
	int pS = 0;
	


	std::list<pthelm>::iterator it_pthelmp, it_pthelmd;
	Riterator itroute;

	order* rorder;
	if (R.size() == 1)
	{
		Pex = 1.0;
		Pex2 = 1.0;
	}
		itroute = od[rand_order].pelmp[snum]->proute;//被选request所在路径
		rorder = &od[rand_order];
		it_pthelmp = rorder->pelmp[snum];
		it_pthelmd = rorder->pelmd[snum];
		Route tmp(g_d, snum);
		auto tmpn = next(it_pthelmp);
		auto tmdn = next(it_pthelmd);
		tmp.path.splice(tmp.path.begin(), itroute->path, it_pthelmp, tmdn);
		if (itroute->path.size()==2)
		{
			tmp.route_cost = 0;
		}
		else
		{
			tmp.route_cost = itroute->getcostonly();
		}
		


		vector<Perturb> record_perturb;
		record_perturb.reserve(100);
		double perR = rand() / double(RAND_MAX);
		if (perR < Pex)//// simple relocate   Pex
		{
			record_perturb.clear();
			for (auto rit = R.begin(); rit != R.end(); rit++)
			{

				if ((rit) == itroute) continue;

				seg_perturbPD(rit, &tmp, it_pthelmp, it_pthelmd, record_perturb);

			}
			if (!record_perturb.empty())
			{

// 				sort(record_perturb.begin(), record_perturb.end(),
// 					[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 				double q = rand() / (double)RAND_MAX;
// 				size_t randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));			
// 				if (randselct >= record_perturb.size())
// 				{
// 					randselct = record_perturb.size() - 1;
// 				}

				size_t randselct = rand() % record_perturb.size();
				Perturb r1 = record_perturb[randselct];
				(r1.itroute)->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp,next(it_pthelmd));
				
				//必须updata 不能注释
				int ret = updata(r1.itroute);
				if (ret == 0)//infeasible
				{
					g_d->each_problem_output << "route error13!!!\n";
					my_ERROR
				}
				ret = updata(itroute);
				if (ret == 0)//infeasible
				{
					g_d->each_problem_output << "route error13!!!\n";
					my_ERROR
				}
				if (itroute->path_length == 2) {
					R.erase(itroute);
				}
			}
			else
			{
				itroute->path.splice(tmdn, tmp.path, it_pthelmp, next(it_pthelmd));
			}
#ifdef CHECK
			checkR((removed.size()), 1);
#endif

		}
		else //if (Pex <= perR &&perR <= Pex2)
			 // extended exchange
		{//再随机选一个order
			record_perturb.clear();

			int rand_order2;
			while (1) {
				rand_order2 = rand() % g_d->pdd_index;
				if (rand_order2 == rand_order || rand_order2 == 0 ||
					&*od[rand_order2].pelmp[snum]->proute == &*itroute || removed.end() != find_if(removed.begin(), removed.end(),
						[&rand_order2](const removepool& rp) {return rp.remove_order->id == rand_order2; }))
					continue;//在removed里面找到了
				else break;
			}
			auto itroute2 = od[rand_order2].pelmp[snum]->proute;//被选request所在路径
			order* rorder2 = &od[rand_order2];

			auto it_pthelmp2 = rorder2->pelmp[snum];
			auto it_pthelmd2 = rorder2->pelmd[snum];
			Route tmp2(g_d, snum);
			auto tmpn2 = next(it_pthelmp2);
			auto tmdn2 = next(it_pthelmd2);
			tmp2.path.splice(tmp2.path.begin(), itroute2->path, it_pthelmp2, tmdn2);
			
			if (itroute2->path.size()==2 && itroute->path.size()==2)
			{
				itroute2->path.splice(tmdn2, tmp2.path, it_pthelmp2, next(it_pthelmd2));
				itroute->path.splice(tmdn, tmp.path, it_pthelmp, next(it_pthelmd));
				return 1;
			}
			if (itroute2->path.size()==2)
			{
				tmp2.route_cost = 0;
			}
			else
			{
				tmp2.route_cost = itroute2->getcostonly();//删除一段交换的后的cost
			}
			
			//将it_pthelmp 和 it_pthelmd insert到itroute2->path
			//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path


			seg_perturbPD(itroute2, &tmp, it_pthelmp, it_pthelmd, record_perturb);

			vector<Perturb> record_perturb2;
			if (!record_perturb.empty())//有合适的位置
			{//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path

				seg_perturbPD(itroute, &tmp2, it_pthelmp2, it_pthelmd2, record_perturb2);

				if (!record_perturb2.empty() && !record_perturb.empty())
				{//从record_perturb2 和 record_perturb 中 各选一个
					
// 					sort(record_perturb.begin(), record_perturb.end(),
// 						[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 					double q = rand() / (double)RAND_MAX;
// 
// 					size_t randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));
// 					if (randselct >= record_perturb.size())
// 					{
// 						randselct = record_perturb.size() - 1;
// 					}

					size_t randselct = rand() % record_perturb.size();
					Perturb r1 = record_perturb[randselct];

					itroute2->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp,next(it_pthelmd));


// 					sort(record_perturb2.begin(), record_perturb2.end(),
// 						[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 					double q2 = rand() / (double)RAND_MAX;
// 
// 					size_t randselct2 = static_cast<size_t>(floor(pow(q2, 5)*record_perturb2.size()));
// 					if (randselct2 >= record_perturb2.size())
// 					{
// 						randselct2 = record_perturb2.size() - 1;
// 					}
// 					Perturb r2 = record_perturb2[randselct2];

					Perturb r2 = record_perturb2[rand() % record_perturb2.size()];

					itroute->path.splice(r2.it_pthelmp, tmp2.path, it_pthelmp2,next(it_pthelmd2));
					//itroute->path.splice(r2.it_pthelmd, tmp2.path, it_pthelmd2);

// 					it_pthelmp2->proute = itroute;
// 					it_pthelmd2->proute = itroute;
					//必须updata 不可以注释
					int ret = updata(itroute);
					if (ret == 0)//infeasible
					{
						g_d->each_problem_output << "route error13!!!\n";
						my_ERROR
					}
					ret = updata(itroute2);
					if (ret == 0)//infeasible
					{
						g_d->each_problem_output << "route error13!!!\n";
						my_ERROR
					}

				}
				else
				{
					itroute2->path.splice(tmdn2, tmp2.path, it_pthelmp2, next(it_pthelmd2));
					itroute->path.splice(tmdn, tmp.path, it_pthelmp, next(it_pthelmd));
					
				}

			}
			else//没有合适的位置
			{
				itroute2->path.splice(tmdn2, tmp2.path, it_pthelmp2,next(it_pthelmd2));
				itroute->path.splice(tmdn, tmp.path, it_pthelmp,next(it_pthelmd));
				
			}
			//printf("exchange\n");
		}

		return 1;

	

}
void Solution::seg_perturbPD(Riterator rit, Route* itroute, std::list<pthelm>::iterator it_pthelmp,
	std::list<pthelm>::iterator it_pthelmd, vector<Perturb> &record_perturb)
{

	std::list<pthelm> *ppj = &(rit)->path;
	auto itdn = next(it_pthelmd);
	auto itjd = ppj->begin();

	double oricost = rit->route_cost + itroute->route_cost;
	ppj->splice(itjd, itroute->path, it_pthelmp, next(it_pthelmd));

	double itrouteCost = itroute->getcostonly();

	do {
		double upc = 0;
		if ((rit)->updata_lcheck(upc) == 1)//feasible
		{

			Perturb feasible;
			feasible.it_pthelmp = itjd;
			feasible.it_pthelmd = itjd;
			feasible.itroute = (rit);

			//			feasible.ccost = upc + itroute->route_cost - oricost;//计算solution的cost
			record_perturb.push_back(feasible);
		}

		itjd++;
		if (itjd == ppj->end()) { break; }
		ppj->splice(itjd, *ppj, it_pthelmp, next(it_pthelmd));

	} while (1);
	itroute->path.splice(itdn, *ppj, it_pthelmp, next(it_pthelmd));
}
double Solution::ord_perturb(int rand_order)
{
	//随机选一个request 除了removed之外的
	// simple relocate
	// extended exchange
	int pS = 0;
	//int pF = 0;
		bool perS = 0;
		std::list<pthelm>::iterator it_pthelmp, it_pthelmd;
		Riterator itroute;
		if (R.size() == 1)
		{
			Pex = 1.0;
			Pex2 = 1.0;
		}
		
		
		itroute = od[rand_order].pelmp[snum]->proute;//被选request所在路径
		order* rorder = &od[rand_order];

		it_pthelmp = rorder->pelmp[snum];
		it_pthelmd = rorder->pelmd[snum];
		Route tmp(g_d, snum);
		auto tmpn = next(it_pthelmp);
		auto tmdn = next(it_pthelmd);
		tmp.path.splice(tmp.path.begin(), itroute->path, it_pthelmp);
		tmp.path.splice(tmp.path.end(), itroute->path, it_pthelmd);
		if (itroute->path.size()==2)
		{
			tmp.route_cost = 0;
		}
		else
			tmp.route_cost = itroute->getcostonly();

		vector<Perturb> record_perturb;
		record_perturb.reserve(100);
		double perR = rand() / double(RAND_MAX);
		if (perR < Pex)//// simple relocate   Pex
		{
			record_perturb.clear();
			for (auto rit = R.begin(); rit != R.end(); rit++)
			{

				if ((rit) == itroute) continue;
				double oricost = rit->route_cost + itroute->route_cost;
				perturbPD(rit, &tmp, it_pthelmp, it_pthelmd, record_perturb/*, oricost*/);

			}
			if (!record_perturb.empty())
			{

// 				sort(record_perturb.begin(), record_perturb.end(),
// 					[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 				double q = rand() / (double)RAND_MAX;
// 
// 				size_t randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));
// 				if (randselct >= record_perturb.size())
// 				{
// 					randselct = record_perturb.size() - 1;
// 				}

				size_t randselct = rand() % record_perturb.size();
				Perturb r1 = record_perturb[randselct];
				(r1.itroute)->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp);
				(r1.itroute)->path.splice(r1.it_pthelmd, tmp.path, it_pthelmd);
				//必须updata 不能注释
				int ret = updata(r1.itroute);
				if (ret == 0)//infeasible
				{
					g_d->each_problem_output << "route error13!!!\n";
					my_ERROR
				}
				ret = updata(itroute);
				if (ret == 0)//infeasible
				{
					g_d->each_problem_output << "route error13!!!\n";
					my_ERROR
				}
				if (itroute->path_length == 2) {
					R.erase(itroute);
				}
			}
			else {
				itroute->path.splice(tmdn, tmp.path, it_pthelmd);
				itroute->path.splice(tmpn, tmp.path, it_pthelmp);
			}
#ifdef CHECK
			checkR((removed.size()), 1);
#endif

		}
		else //if (Pex <= perR &&perR <= Pex2)
			 // extended exchange
		{//再随机选一个order
			record_perturb.clear();

			int rand_order2;
			while (1) {
				rand_order2 = rand() % g_d->pdd_index;
				if (rand_order2 == rand_order || rand_order2 == 0 ||
					&*od[rand_order2].pelmp[snum]->proute == &*itroute || removed.end() != find_if(removed.begin(), removed.end(),
						[&rand_order2](const removepool& rp) {return rp.remove_order->id == rand_order2; }))
					continue;//在removed里面找到了
				else break;
			}
			auto itroute2 = od[rand_order2].pelmp[snum]->proute;//被选request所在路径

			if (itroute2->path_length==4 && itroute->path_length==4)
			{
				itroute->path.splice(tmdn, tmp.path, it_pthelmd);
				itroute->path.splice(tmpn, tmp.path, it_pthelmp);
				return 1;
			}

			order* rorder2 = &od[rand_order2];
			auto it_pthelmp2 = rorder2->pelmp[snum];
			auto it_pthelmd2 = rorder2->pelmd[snum];
			Route tmp2(g_d, snum);
			auto tmpn2 = next(it_pthelmp2);
			auto tmdn2 = next(it_pthelmd2);
			tmp2.path.splice(tmp2.path.begin(), itroute2->path, it_pthelmp2);
			tmp2.path.splice(tmp2.path.end(), itroute2->path, it_pthelmd2);
// 			if (itroute2->path.size()==2)
// 			{
// 				tmp2.route_cost = 0;
// 			}
// 			else
// 				tmp2.route_cost = itroute2->getcostonly();


			//将it_pthelmp 和 it_pthelmd insert到itroute2->path
			//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path

			double oricost = itroute->route_cost + itroute2->route_cost;
			perturbPD(itroute2, &tmp, it_pthelmp, it_pthelmd, record_perturb/*, oricost*/);

			vector<Perturb> record_perturb2;
			if (!record_perturb.empty())//有合适的位置
			{//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path

				perturbPD(itroute, &tmp2, it_pthelmp2, it_pthelmd2, record_perturb2/*, oricost*/);

				if (!record_perturb2.empty() && !record_perturb.empty())
				{//从record_perturb2 和 record_perturb 中 各选一个
					
// 					sort(record_perturb.begin(), record_perturb.end(),
// 						[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 					double q = rand() / (double)RAND_MAX;
// 
// 					size_t randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));
// 					if (randselct >= record_perturb.size())
// 					{
// 						randselct = record_perturb.size() - 1;
// 					}
// 					Perturb r1 = record_perturb[randselct];

					Perturb r1 = record_perturb[rand() % record_perturb.size()];

					itroute2->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp);
					itroute2->path.splice(r1.it_pthelmd, tmp.path, it_pthelmd);


// 					sort(record_perturb2.begin(), record_perturb2.end(),
// 						[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 					double q2 = rand() / (double)RAND_MAX;
// 
// 					size_t randselct2 = static_cast<size_t>(floor(pow(q2, 5)*record_perturb2.size()));
// 					if (randselct2 >= record_perturb2.size())
// 					{
// 						randselct2 = record_perturb2.size() - 1;
// 					}
// 					Perturb r2 = record_perturb2[randselct2];

					Perturb r2 = record_perturb2[rand() % record_perturb2.size()];

					itroute->path.splice(r2.it_pthelmp, tmp2.path, it_pthelmp2);
					itroute->path.splice(r2.it_pthelmd, tmp2.path, it_pthelmd2);

					//必须updata 不可以注释
					int ret = updata(itroute);
					if (ret == 0)//infeasible
					{
						g_d->each_problem_output << "route error13!!!\n";
						my_ERROR
					}
					ret = updata(itroute2);
					if (ret == 0)//infeasible
					{
						g_d->each_problem_output << "route error13!!!\n";
						my_ERROR
					}

				}
				else
				{
					itroute2->path.splice(tmdn2, tmp2.path, it_pthelmd2);
					itroute2->path.splice(tmpn2, tmp2.path, it_pthelmp2);

					itroute->path.splice(tmdn, tmp.path, it_pthelmd);
					itroute->path.splice(tmpn, tmp.path, it_pthelmp);
					
				}

			}
			else//没有合适的位置
			{
				itroute2->path.splice(tmdn2, tmp2.path, it_pthelmd2);
				itroute2->path.splice(tmpn2, tmp2.path, it_pthelmp2);

				itroute->path.splice(tmdn, tmp.path, it_pthelmd);
				itroute->path.splice(tmpn, tmp.path, it_pthelmp);
				
			}
			//printf("exchange\n");
		}
		return 1;

}
double Solution::perturb(int irand)
{
	for (int i=0;i<irand;++i)
	{
		 
		std::list<pthelm>::iterator it_pthelmp, it_pthelmd;
		Riterator itroute;

		order* rorder;
		
		int rand_order;
		while (1) {
			rand_order = rand() % g_d->pdd_index;
			if (rand_order == 0 || removed.end() != find_if(removed.begin(), removed.end(),
				[&rand_order](const removepool& rp) {return rp.remove_order->id == rand_order; }))
				continue;//在removed里面找到了
			else
			{
				break;
			}
		}
		itroute = od[rand_order].pelmp[snum]->proute;//被选request所在路径
		rorder = &od[rand_order];
		it_pthelmp = rorder->pelmp[snum];
		it_pthelmd = rorder->pelmd[snum];
		
// 		int len = 0;
// 		for (auto it = it_pthelmp; it != next(it_pthelmd); it++)
// 		{
// 			len++;
// 		}
// 		if (len > 4)
// 		{
// 			ord_perturb(rand_order);//rand_prder
// 		}
// 		else
// 			seg_perturb(rand_order);


		double perR = rand() / double(RAND_MAX);
		if (perR<0.7)
		{
			ord_perturb(rand_order);
		}
		else
		{
			seg_perturb(rand_order);
		}
	}
	return 1;
	
}