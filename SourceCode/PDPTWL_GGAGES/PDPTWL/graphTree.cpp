
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include "ejection.h"
#include "Solution.h"
#include "tree.h"
using namespace std;
#define  MAX(a,b) ((a)>(b)?(a):(b))



void EjectionSearch::traverse(Node& head, int level, int startnum, int endnum) {
	int nowlevel = level + 1;
	if (nowlevel == 2 * kmax + 2/*C + 1*/)
	{
		return;
	}

	for (int i = startnum; i <= endnum; ++i)
	{
// 		head.adj.emplace_back(Node(nowlevel, i, &head));
// 		Node& nextNode = head.adj.back();

		_assert(&head != nullptr);
		Node& nextNode = head.putpack_getlast(nowlevel, i, &head);
		bool increment = 0;
		if (i != startnum) { increment = 1; }
		auto tmppthelm_itdn = next(temporder->it_pthelmd);
		int charge = prune(nextNode, endnum, increment);
#ifdef CHECK
		double diss;
		int aaa = r->updata_lcheck(diss);
		if (aaa == 1 && charge != 1)
		{
				
			if ((head.adj.back().level != jd && head.adj.back().num == itjp_index) || head.adj.back().cont.psum > pS0->Pbest)
			{
			}
				else
				{
					my_ERROR
					auto it = head.adj.back().cont.itr;
					it++;
					if (head.adj.back().level == jd)
					{
						tmppthelm->splice(tmppthelm_itdn, r->path, temporder->it_pthelmd);//temporder.it_pthelmd
					}
					else
					{
						r->path.splice(it, head.adj.back().cont.did_eject, head.adj.back().cont.eject_itr);
					}


					charge = prune(head.adj.back(), endnum, increment);
				}
			

		}
#endif
		int bbb = 0;
		if (charge == -2)
		{
#ifdef CHECK
			if ((head.adj.back().level != jd && head.adj.back().num == itjp_index) || head.adj.back().cont.psum > pS0->Pbest)
			{
			}
			else
				traverse_purn(head.adj.back(), nowlevel, i + 1, endnum);
#endif
//#define tmppath tmppath
			auto it = next(nextNode.cont.itr);
			//it++;
			if (nextNode.level == jd)
			{
				ptmp_path->splice(tmppthelm_itdn, tmpr->path, temporder->it_pthelmd);//temporder.it_pthelmd
			}
			else
			{
				tmpr->path.splice(it, tmppath, nextNode.cont.eject_itr);
			}
			break;
		}
		else if (charge == -1)
		{
#ifdef CHECK
			if ((head.adj.back().level != jd && head.adj.back().num == itjp_index) || head.adj.back().cont.psum > pS0->Pbest)
			{
			}
			else
				traverse_purn(head.adj.back(), nowlevel, i + 1, endnum);
#endif

			auto it = next(nextNode.cont.itr);
			if (nextNode.level == jd)
			{
				ptmp_path->splice(tmppthelm_itdn, tmpr->path, temporder->it_pthelmd);//temporder.it_pthelmd
			}
			else
			{
				tmpr->path.splice(it, tmppath, nextNode.cont.eject_itr);
			}
			continue;
		}
		else if (charge == 1 && constraint(nextNode) == 1)
		{
			bbb = 1;
		}

#ifdef CHECK		
		if (aaa != bbb)
		{
			printf("gggg");
			my_ERROR
			r->updata_lcheck(diss);
			for (auto iy = r->path.begin(); iy != r->path.end(); iy++)
			{
				auto et = MAX(iy->st, iy->pcus->etime);
				if (et != iy->atime)
				{
					//printf("%d\n", yy);
					printf("gggg");
					my_ERROR
				}
			}
			constraint(head.adj.back());
			//	aaa = pr_origin->updata();
			my_ERROR
		}
#endif
		if (bbb == 1)
		{//判断是否最优解
#ifdef CHECK
			double diss = 0;
			if (r->updata_lcheck(diss) == 0)
				my_ERROR
#endif
			if (nextNode.cont.psum <= pS0->Pbest)
			{
				if (nextNode.cont.psum < pS0->Pbest)
				{
					pS0->Pbest = nextNode.cont.psum;
					//记录新的数据
					record_eject.clear();
				}
				ejectpij_best  eject;
				int summ = 0;
				auto tmp = &nextNode;
				while (tmp->father != nullptr) {
					if (tmp->level == jd)
					{
						eject.din = tmp->num;
						tmp = tmp->father; continue;
					}
					eject.ejection.push_back(tmp->num);
					eject.it_ejection.push_back(tmp->cont.eject_itr);

					_assert(tmp->cont.eject_itr->pcus->cusid == tmp->cont.pej->pcus->cusid);
					if (tmp->cont.eject_itr->pcus->p==1)
					{
						summ += tmp->cont.eject_itr->porder->id;
					}
					else
					{
						summ -= tmp->cont.eject_itr->porder->id;
					}
					tmp = tmp->father;
				}
				_assert(summ == 0);

				eject.rindex = rindex;
				if (nextNode.level < jd)
					eject.din = 0;
				eject.pin = itjp_index;
				eject.itjpn = itjp;
				record_eject.push_back(eject);
			}
		}

		traverse(nextNode, nowlevel, i + 1, endnum);


		auto it = nextNode.cont.itr;
		it++;
		if (nextNode.level == jd)
		{
			ptmp_path->splice(tmppthelm_itdn, tmpr->path, temporder->it_pthelmd);//temporder.it_pthelmd
		}
		else
		{
			tmpr->path.splice(it, tmppath, nextNode.cont.eject_itr);
		}

	}
}
//#undef tmppath

#ifdef CHECK
void EjectionSearch::traverse_purn(Node& head, int level, int startnum, int endnum) {
	int nowlevel = level + 1;
	if (nowlevel == 2 * kmax + 2/*C + 1*/)
	{
		return;
	}

	for (int i = startnum; i <= endnum; ++i)
	{
		head.adj.emplace_back(Node(nowlevel, i, &head));

		bool increment = 0;
		if (i != startnum) { increment = 1; }
		auto tmppthelm_itdn = next(temporder->it_pthelmd);
		int charge = prune(head.adj.back(), endnum, increment);
		
		if (charge==1)
		{
			charge = constraint(head.adj.back());
		}

		double diss;
		int aaa = tmpr->updata_lcheck(diss);
		if (aaa == 1 )
		{
			my_ERROR
				auto it = head.adj.back().cont.itr;
			it++;
			if (head.adj.back().level == jd)
			{
				ptmp_path->splice(tmppthelm_itdn, tmpr->path, temporder->it_pthelmd);//temporder.it_pthelmd
			}
			else
			{
				tmpr->path.splice(it, qk_pth, head.adj.back().cont.eject_itr);
			}


			charge = prune(head.adj.back(), endnum, increment);
		}


	
		if ((head.adj.back().level != jd && head.adj.back().num == itjp_index) || head.adj.back().cont.psum > pS0->Pbest)
		{
		}
		else
			traverse_purn(head.adj.back(), nowlevel, i + 1, endnum);


		auto it = head.adj.back().cont.itr;
		it++;
		if (head.adj.back().level == jd)
		{
			ptmp_path->splice(tmppthelm_itdn, tmpr->path, temporder->it_pthelmd);//temporder.it_pthelmd
		}
		else
		{
			tmpr->path.splice(it, qk_pth, head.adj.back().cont.eject_itr);
		}

	}
}
#endif // CHECK



void EjectionSearch::Search(Node& head, int level, int n) {

	if (level < jd - 2)
	{
		//if (head.adj.empty()) {
		if (head.empty()) {
			return;
		}
	}
	else if (level == jd - 2)
	{
		if (head.cont.isprune == 1)
		{
			//_assert(head.adj.empty());
			_assert(head.empty());
			return;
		}
		(head).cont.itr->atime = head.cont.atime_new;
		head.cont.itr->q = head.cont.q_new;
		//head.adj.clear();
		head.clear();
		traverse(head, level, head.num + 1, n);
		return;
	}

	int nowlevel = level + 1;

	//for (auto it = head.adj.begin(); it != head.adj.end(); it++)
	for (auto it = head.adj.begin(); it != head.end(); it++)
	{
		if (nowlevel <= jd - 2)
		{
			std::list<pthelm> *ppj = &tmpr->path;//取址
			#ifdef _CHK
auto iuu = ppj->end();
			advance(iuu, -(n - (*it).num + 3));//eject 前一个
			if (iuu != (*it).cont.itr)
			{
				my_ERROR
			}
#endif // _CHK
//#define  tmppath tmppath
			tmppath.splice(tmppath.end(), *ppj, (*it).cont.eject_itr);
			Search(*it, nowlevel, n);
			tmpr->path.splice(next((*it).cont.itr), tmppath, (*it).cont.eject_itr);
//#undef tmppath
		}
	}
}



void EjectionSearch::lexicographic_iterativebacktrack(int n, Node &root) {

	if (jd == 1 || jd == 2)
	{
		//root.adj.clear();
		root.clear();
		traverse(root, root.level, root.num + 1, n);
	}
	else {//上层一样
		Search(root, root.level, n);
	}

// 	root.clear();
// 	traverse(root, root.level, root.num + 1, n);

}

int Solution::insert_chck(std::list<pthelm>::iterator it_pthelmp, list<pthelm>::iterator it_pthelmd, int increment)
{
	
	auto it_pre = prev(it_pthelmd, 1);
	auto it_adv = prev(it_pthelmd, -1);
	double ab;
	if (increment == 1)
	{//检查i(jd)
		auto itt = prev(it_pthelmd, 1);//i(jd)
		auto it = prev(itt, 1);//i(jd)-1
		ab = (*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid];
		(*itt).atime = MAX(ab,(*itt).pcus->etime);

		//(*itt).atime = MAX((*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid], (*itt).pcus->etime);
		itt->q = it->q + itt->pcus->demand;
	}

	auto it = prev(it_pthelmd, 1);//it=i[jd]
	auto itt = it_pthelmd;////insert 进去的那个  dilivery插在x[jd]之后 
						  //i[jd]+1   
	for (int i = 0; i < 2; i++)
	{
		ab= (*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid];
		(*itt).atime = MAX(ab, (*itt).pcus->etime);
		//(*itt).atime = MAX((*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid], (*itt).pcus->etime);
		itt->q = it->q + itt->pcus->demand;
		it++;
		itt++;
	}
	itt--;//i[jd]+1 

		  // return -1 表示子代不用算了，往后挪，continue
		  // return -2 其后面的兄弟姐妹都不用算了，break
		  // pin的条件下 return 1， 寻找其din
		  // din的条件下 return 1，合法解
	if (it_pre->atime > it_pre->pcus->ltime || it_pre->q > g_d->cap)
	{////判断i(jd)
		return -2;//其之后的兄弟 及 自己和兄弟的子代都不用算了
	}

	if (it_pthelmd->atime > it_pthelmd->pcus->ltime)
	{
		return -2;//插进i[jd]后面的那个insert不满足时间窗  -2
	}

	if (it_pthelmd->q > g_d->cap)
	{
		return -1;
	}
	if (it_pthelmd->pcus->d == 1)//din
	{
		if (it_pre->pcus->p == 1 && it_pre->pcus->cusid != it_pthelmd->porder->pp->cusid)//i[jd]是pickup,即pickup后面插入另一个元素的delivery
		{// i[jd]是pickup, 即pickup后面插入另一个元素的delivery
			return -1;//其子代不用算了
		}
	}
	if (it_adv->atime > itt->lpt || it_adv->q > g_d->cap)
	{
		return -1;
	}
	if (it_pthelmp->pcus->cusid != it_pthelmd->pcus->cusid)
	{
		//slifo.clear();
		//std::stack<int> slifo;
		/*vector<int> */slifo.clear();
		//slifo.reserve(20);
		for (auto it = it_pthelmp; it != next(it_pthelmd); it++)//pin 到 din
		{
			if (it->pcus->p == 1 && it->pcus->cusid != 0)//pickup 入栈
			{
				//slifo.push(it->pcus->cusid);
				slifo.push_back(it->pcus->cusid);
			}
			if (it->pcus->d == 1 && it->pcus->cusid != 0)//delivery 出栈
			{
				_assert(!slifo.empty());
// 				if (slifo.top() == it->pcus->pid)
// 				{
// 					slifo.pop();//删除栈顶
// 				}
				if (slifo.back() == it->pcus->pid)
				{
					slifo.pop_back();//删除栈顶
				}
				else
				{
					return -1;//infeasible
				}

			}
		}
		if (!slifo.empty())
		{
			return -1;
		}
	}

	return true;

}
