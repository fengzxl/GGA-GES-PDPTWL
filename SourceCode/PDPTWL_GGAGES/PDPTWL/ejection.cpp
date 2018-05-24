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
void Solution::ejection_cross(list<Route>::iterator se, int bad)
{
	if (bad == 1)
	{
		cout << "bad\t";
		sprintf(buff, "bad   \t");
		g_d->each_problem_output << buff;
	}
	else
	{
		cout << "rand\t";
		sprintf(buff, "rand   \t");
		g_d->each_problem_output << buff;
	}
	
	int croute = 0;
	double fa_tcost = 0;
	Pbest = INT_MAX;
	int fa = 0;
	Node root(0, 0, nullptr);
		tmpR = R;
		fa_tcost = total_cost;
		//checkR(0,1);
		
		std::list<Route> EP;
		{//1
		 //removed的order
		
			EP.splice(EP.begin(), R, se);
		}

		std::list<pthelm> &tmppath = EP.front().path;
		{//2
			removed.clear();
			removed.reserve(20);
			//for (auto p1 = (*pr_origin)->path.begin(); p1 != (*pr_origin)->path.end(); ++p1) {
			for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
				if (p1->pcus->p == 1)//pickup
				{
					removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
				}
			}

		}
		int orignsize = static_cast<int>(removed.size());

		int order_num = (g_d->cus_num + 1) / 2;
		int c = 0;//迭代次数
		int c_max = (order_num*order_num) / static_cast<int>(R.size());//size(R)+1 remove之前 or 之后

		p.assign(p.size(), 1);//3

		Pbest = INT_MAX;

		//double c_time = nowtime();
		while (!removed.empty() && c < c_max)
		{//对每个需要插入的order进行遍历
			c++;
			int temp_index = rand() % removed.size();
			removepool temp = removed[temp_index];

			vector<insertpij> record_insert;//记录有几种插入方案 随机选一个
			record_insert.reserve(R.size());

			//double insert_start = nowtime();
			int route_index = 0;
			for (auto itr = R.begin(); itr != R.end(); itr++, route_index++)//对每条路径进行遍历
			{

				insert_route(tmppath, temp, &*itr, record_insert, route_index);//把一个order插入到一条路径

				temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
				//= EP.begin();
			}



			if (!record_insert.empty())//随机选一个
			{
				size_t randselct = rand() % record_insert.size();
				
				int numth = record_insert[randselct].rindex;//第几条路径 rindex从0开始
				auto itr = next(R.begin(), numth);

				(itr)->path.splice(record_insert[randselct].itjp, tmppath, temp.it_pthelmp);
				(itr)->path.splice(record_insert[randselct].itjd, tmppath, temp.it_pthelmd);
				temp.it_pthelmd->proute = temp.it_pthelmp->proute = itr;

				//不进行check注释 必须update
				int ret = updata(itr);
				_assert(ret != 0);

				/*********************************随机选择一个order删除************************************************/
				auto itremo = next(removed.begin(), temp_index);
				removed.erase(itremo);
				//cperturb=perturb(0.3*Irand);//迭代30次
			}

			// 			if (!removed.empty() && removed.end() != find_if(removed.begin(), removed.end(),
			// 				[&temp](const removepool& rp) {return rp.remove_order->id == temp.remove_order->id; }))// temp 没有被re_insert 能找到
			else
			{//ejection if hin can not be insert
			 //double eject_start = nowtime();
				p[temp.remove_order->id]++;//penalty + 1
				EjectionSearch ejection1(g_d);//初始化 kmax=2  每次新建psum pbest会被清空 赋初始值
											  //每插入一个新的点 重新建一个对象 即之前的值全都清零 重新初始化
				ejection1.pS0 = this;
				route_index = 0;
				
				
				//随机选一个路径
				int autoint = rand() % R.size();
				route_index = autoint;
				list<Route>::iterator itr = next(R.begin(), route_index);
				for (/*auto itr = R.begin()*/; itr != R.end(); itr++, route_index++)//对每条路径进行遍历
				{
					ejection1.tmpr = &*itr;
					ejection1.rindex = route_index;
					ejection1.ptmp_path = &tmppath;
					ejection1.temporder = &temp;
					ejection1.eject_route(root);//把一个order插入到一条路径
					temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
					if (!ejection1.record_eject.empty())
					{
						break;
					}
				}


				ejectpij_best  eject;
				ejection1.record_eject.push_back(eject);
				if (!ejection1.record_eject.empty())
				{
					int randselct = rand() % ((ejection1.record_eject.size()));

					if (randselct == ((ejection1.record_eject.size()) - 1))
					{
						double rd = rand() / double(RAND_MAX);
						if (rd >= 0.5) {
							randselct = rand() % (ejection1.record_eject.size());
						}
					}
					if (randselct == ((ejection1.record_eject).size() - 1))//以较小概率选择 50%
					{//原路径 不作任何变化 没有insert 没有eject
					 //
					 //printf("removed_size:%d; select_order:%d; record_eject %d 选择原路径 \n", removed.size(), temp.remove_order->id,size(ejection1.record_eject));
					}
					else {
						auto& pijchuz = ejection1.record_eject[randselct];
						int numth = pijchuz.rindex;//第几条路径 rindex从0开始
						auto itr = next(R.begin(), numth);

						auto ret = updata(itr);
						_assert(ret != 0);

						itr->path.splice(pijchuz.itjpn, tmppath, temp.it_pthelmp);
						temp.it_pthelmp->proute = itr;
						auto itjdn = next((itr)->path.begin(), pijchuz.din + 1);
						(itr)->path.splice(itjdn, tmppath, temp.it_pthelmd);//insert
						temp.it_pthelmd->proute = itr;

						for (auto it = pijchuz.it_ejection.rbegin(); it != pijchuz.it_ejection.rend(); ++it)
						{
							tmppath.splice(prev(tmppath.end()), (itr)->path, *it);
							(*it)->proute = EP.begin();
						}
						ret = updata(itr);
						_assert(ret != 0);
						removed.clear();

						for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
							if (p1->pcus->p == 1)//pickup
							{
								removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
							}
						}
#ifdef CHECK
						checkR(removed.size(), 1);
#endif //
					}
				}//end updata solution
				 //double perturb_start = nowtime();
#ifdef CHECK
				checkR(removed.size(), 1);
#endif
				//tabuList();
				order_perturb(Irand);//迭代100次
									 //cperturb = perturb(Irand);//迭代100次
			}//endif ejection



		}//endfor ( removed.size()!=0 )


		if (!removed.empty())
		{

			if (g_d->baseR == 0)//base
			{
				//恢复路径 拷贝
				//恢复路径 后 perturb
				R = tmpR;
				
				//不可以注释
				checkR(0, 1);
				//order_perturb(Irand);
				return;

			}
			else
			{
				
				//不恢复removed自成一条路径 ifremoved一条路径则用 否则不用（即恢复）
				Route tmp(g_d, snum);
				tmp.no = se->no;
				//radd = &rAdd[++raddindex];

				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmd);
				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmp);

				removed.erase(removed.begin());
				tmp.path.push_front(pthelm(od[0].pp, od));
				tmp.path.push_back(pthelm(od[0].pd, od));

				//tmp.updata();
				//insert
				R.emplace_back(tmp);
				auto& radd = R.back();
				auto Rend = prev(R.end());
				updata(Rend);
				//radd.updata();
				int s = removed.size();
				for (int h = 1; h < max(50, s*s); h++)
				{
					if (removed.size() == 0)
					{
						break;
					}
					int temp_index = rand() % removed.size();
					removepool temp = removed[temp_index];
					std::vector<insertpij> rinsert;//record_insert
					insert_route(tmppath, temp, &radd, rinsert, int(R.size()));

					if (!rinsert.empty())
					{
						int randselct = rand() % rinsert.size();
						(radd).path.splice(rinsert[randselct].itjp, tmppath, temp.it_pthelmp);
						(radd).path.splice(rinsert[randselct].itjd, tmppath, temp.it_pthelmd);
						temp.it_pthelmp->proute = Rend;
						temp.it_pthelmd->proute = Rend;
						int ret = updata(Rend);
						if (ret == 0)//fausle infeasible
						{
							printf("error");
						}
						/*********************************随机选择一个order删除************************************************/
						auto itremo = removed.begin();
						advance(itremo, temp_index);
						removed.erase(itremo);

					}
					else {
						continue;
					}

				 

					}
				if (!removed.empty())
				{//fail，恢复

				 //恢复路径 拷贝
				 //恢复路径 后 perturb
					R = tmpR;
					checkR(0, 1);//此处check不可被注释

								 //order_perturb(Irand);
					cout << "\n";
					sprintf(buff, "\n");
					g_d->each_problem_output << buff;
					return;
				}
				//此处check不可被注释
				checkR(0, 1);

				if (bad == 1)
				{
					pbest->removebad++;
				}
				else
					pbest->removerand++;
				//即使fail也记录这个解 此次循环得到的解是否是新解
				//Sobest 与 S[0]->minTime
				vector<pair<pair<pair<int, double>, double>, int>> sb;
				sb.reserve(2);
				sb.clear();
				sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
				sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
				sort(sb.begin(), sb.end());
				if (sb.begin()->second == 1)//S[0]更好
				{
					pbest->R = R;
					pbest->total_time = total_time;
					pbest->total_cost = total_cost;
					pbest->total_dist = total_dist;
				}

				//添加到hash
				bool news = 0;
				size_t code = gethashvalue();
				if (pbest->findinhashtable(code))
					news = 0;
				else
				{
					news = 1;
					pbest->add2hashtable(code, total_cost);
				}

				}





			
			double td = util::nowtime();
#ifdef PRINTF

#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#endif // _WIN32
			printf("fail");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms cross\n",
				 total_dist,total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#endif//PRINTF

#ifdef WRITE
			sprintf(buff, "fail\t\t%-10.2f%-10.2f%-8zd%-8.1f ms cross\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif

			}
		else
		{
			if (bad == 1)
			{
				pbest->removebad++;
			}
			else
				pbest->removerand++;
			//scueess的时候clear hash 然后再添加
			//clear_hashtable();
			total_cost = 0;
			total_dist = 0;
			total_time= total_time = util::nowtime() - g_d->begin_time;
			for (auto itt=R.begin();itt!=R.end();itt++)
			{
				total_cost += itt->route_cost;
				total_dist += itt->route_dist;
			}

			size_t code = gethashvalue();
			if (pbest->findinhashtable(code))
				;
			else
			{
				pbest->add2hashtable(code, total_cost);
			}


			croute++;

			double td = util::nowtime();



#ifdef PRINTF
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif // _WIN32
			printf("success");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32

			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms cross\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
#endif // PRINTF

#ifdef WRITE
			sprintf(buff, "success\t\t%-10.2f%-10.2f%-8zd%-8.1f ms cross\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif
//跟踪最新解是否是更好的
// 			auto tmpht = pbest->hashtable;
// 			pbest->copy(*this, pbest->snum);
// 			pbest->hashtable = tmpht;

			if (R.size() < pbest->R.size())
			{
				pbest->minTime = total_time;
			}
			vector<pair<pair<pair<int, double>, double>, int>> sb;
			sb.reserve(2);
			sb.clear();
			sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
			sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
			sort(sb.begin(), sb.end());
			if (sb.begin()->second == 1)//S[0]更好
			{
				pbest->R = R;
				pbest->total_time = total_time;
				pbest->total_cost = total_cost;
				pbest->total_dist = total_dist;
			}

				
			
		}

#ifdef CHECK
	checkR(0, 1);
#endif
	}

void Solution::ejection_insertion(int utime)
{
	int croute = 0;
	double fa_tcost = 0;
	Pbest = INT_MAX;
	int fa = 0;
	Node root(0, 0, nullptr);
	for (int u = 0; u <= utime; u++)//参数
	{//procedure_delete_route
		if (util::nowtime() - g_d->begin_time > PARA.TIME[PARA.i]*1000)
		{
			break;
		}


		tmpR = R;
		fa_tcost = total_cost;
		//checkR(0,1);
		int randroute = rand() % R.size();
		std::list<Route> EP;
		{//1
		 //removed的order
			auto iter_delete = next(R.begin(), randroute);
			EP.splice(EP.begin(), R, iter_delete);
		}

		std::list<pthelm> &tmppath = EP.front().path;
		{//2
			removed.clear();
			removed.reserve(20);
			//for (auto p1 = (*pr_origin)->path.begin(); p1 != (*pr_origin)->path.end(); ++p1) {
			for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
				if (p1->pcus->p == 1)//pickup
				{
					removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
				}
			}

		}
		int orignsize = static_cast<int>(removed.size());

		int order_num = (g_d->cus_num + 1) / 2;
		int c = 0;//迭代次数
		int c_max = (order_num*order_num) / static_cast<int>(R.size());//size(R)+1 remove之前 or 之后


		p.assign(p.size(), 1);//3

		Pbest = INT_MAX;

		//double c_time = nowtime();
		while (!removed.empty() && c < c_max)
		{//对每个需要插入的order进行遍历
			c++;
			int temp_index = rand() % removed.size();
			removepool temp = removed[temp_index];

			vector<insertpij> record_insert;//记录有几种插入方案 随机选一个
			record_insert.reserve(R.size());

			//double insert_start = nowtime();
			int route_index = 0;
			for (auto itr = R.begin(); itr != R.end(); itr++, route_index++)//对每条路径进行遍历
			{

				insert_route(tmppath, temp, &*itr, record_insert, route_index);//把一个order插入到一条路径

				temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
				//= EP.begin();
			}



			if (!record_insert.empty())//随机选一个
			{
				size_t randselct = rand() % record_insert.size();



				
				int numth = record_insert[randselct].rindex;//第几条路径 rindex从0开始
				auto itr = next(R.begin(), numth);

				(itr)->path.splice(record_insert[randselct].itjp, tmppath, temp.it_pthelmp);
				(itr)->path.splice(record_insert[randselct].itjd, tmppath, temp.it_pthelmd);
				temp.it_pthelmd->proute = temp.it_pthelmp->proute = itr;

				//不进行check注释 必须update
				int ret = updata(itr);
				_assert(ret != 0);

				/*********************************随机选择一个order删除************************************************/
				auto itremo = next(removed.begin(), temp_index);
				removed.erase(itremo);
				//cperturb=perturb(0.3*Irand);//迭代30次
			}

			// 			if (!removed.empty() && removed.end() != find_if(removed.begin(), removed.end(),
			// 				[&temp](const removepool& rp) {return rp.remove_order->id == temp.remove_order->id; }))// temp 没有被re_insert 能找到
			else
			{//ejection if hin can not be insert
			 //double eject_start = nowtime();
				p[temp.remove_order->id]++;//penalty + 1
				EjectionSearch ejection1(g_d);//初始化 kmax=2  每次新建psum pbest会被清空 赋初始值
											  //每插入一个新的点 重新建一个对象 即之前的值全都清零 重新初始化
				ejection1.pS0 = this;
				route_index = 0;



				int autoint = rand() % R.size();
				route_index = autoint;
				list<Route>::iterator itr = next(R.begin(), route_index);
				for (/*auto itr = R.begin()*/; itr != R.end(); itr++, route_index++)//对每条路径进行遍历
				{
					ejection1.tmpr = &*itr;
					ejection1.rindex = route_index;
					ejection1.ptmp_path = &tmppath;
					ejection1.temporder = &temp;
					ejection1.eject_route(root);//把一个order插入到一条路径
					temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
					if (!ejection1.record_eject.empty())
					{
						break;
					}
				}

				
				ejectpij_best  eject;
				ejection1.record_eject.push_back(eject);
				if (!ejection1.record_eject.empty())
				{
					int randselct = rand() % ((ejection1.record_eject.size()));

					if (randselct == ((ejection1.record_eject.size()) - 1))
					{
						double rd = rand() / double(RAND_MAX);
						if (rd >= 0.5) {
							randselct = rand() % (ejection1.record_eject.size());
						}
					}
					if (randselct == ((ejection1.record_eject).size() - 1))//以较小概率选择 50%
					{//原路径 不作任何变化 没有insert 没有eject
					 //
					 //printf("removed_size:%d; select_order:%d; record_eject %d 选择原路径 \n", removed.size(), temp.remove_order->id,size(ejection1.record_eject));
					}
					else {
						auto& pijchuz = ejection1.record_eject[randselct];
						int numth = pijchuz.rindex;//第几条路径 rindex从0开始
						auto itr = next(R.begin(), numth);

						auto ret = updata(itr);
						_assert(ret != 0);

						itr->path.splice(pijchuz.itjpn, tmppath, temp.it_pthelmp);
						temp.it_pthelmp->proute = itr;
						auto itjdn = next((itr)->path.begin(), pijchuz.din + 1);
						(itr)->path.splice(itjdn, tmppath, temp.it_pthelmd);//insert
						temp.it_pthelmd->proute = itr;

						for (auto it = pijchuz.it_ejection.rbegin(); it != pijchuz.it_ejection.rend(); ++it)
						{
							tmppath.splice(prev(tmppath.end()), (itr)->path, *it);
							(*it)->proute = EP.begin();
						}
						ret = updata(itr);
						_assert(ret != 0);
						removed.clear();

						for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
							if (p1->pcus->p == 1)//pickup
							{
								removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
							}
						}
#ifdef CHECK
						checkR(removed.size(), 1);
#endif //
					}
				}//end updata solution
				 //double perturb_start = nowtime();
#ifdef CHECK
				checkR(removed.size(), 1);
#endif
				//tabuList();
				order_perturb(Irand);//迭代100次
				//cperturb = perturb(Irand);//迭代100次
			}//endif ejection



		}//endfor ( removed.size()!=0 )


		if (!removed.empty())
		{

			if (g_d->baseR==0)//base
			{
				//恢复路径 拷贝
				//恢复路径 后 perturb
				R = tmpR;
				//不可以注释
				checkR(0, 1);
				//order_perturb(Irand);

				
			}
			else
			{
				//不恢复removed自成一条路径 ifremoved一条路径则用 否则不用（即恢复）
				Route tmp(g_d, snum);

				//radd = &rAdd[++raddindex];

				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmd);
				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmp);

				removed.erase(removed.begin());
				tmp.path.push_front(pthelm(od[0].pp, od));
				tmp.path.push_back(pthelm(od[0].pd, od));

				//tmp.updata();
				//insert
				R.emplace_back(tmp);
				auto& radd = R.back();
				auto Rend = prev(R.end());
				updata(Rend);
				//radd.updata();
				int s = removed.size();
				for (int h = 1; h < max(50,s*s); h++)
				{
					if (removed.size() == 0)
					{
						break;
					}
					int temp_index = rand() % removed.size();
					removepool temp = removed[temp_index];
					std::vector<insertpij> rinsert;//record_insert
					insert_route(tmppath, temp, &radd, rinsert, int(R.size()));

					if (!rinsert.empty())
					{
						int randselct = rand() % rinsert.size();
						(radd).path.splice(rinsert[randselct].itjp, tmppath, temp.it_pthelmp);
						(radd).path.splice(rinsert[randselct].itjd, tmppath, temp.it_pthelmd);
						temp.it_pthelmp->proute = Rend;
						temp.it_pthelmd->proute = Rend;
						int ret = updata(Rend);
						if (ret == 0)//fausle infeasible
						{
							printf("error");
						}
						/*********************************随机选择一个order删除************************************************/
						auto itremo = removed.begin();
						advance(itremo, temp_index);
						removed.erase(itremo);

					}
					else {
						continue;
					}

					

				}
				if (!removed.empty())
				{//fail，恢复
			
					//恢复路径 拷贝
					//恢复路径 后 perturb
					R = tmpR;
					checkR(0, 1);//此处check不可被注释

					//order_perturb(Irand);
				}
				//此处check不可被注释
				//即使fail也记录这个解 此次循环得到的解是否是新解
				//Sobest 与 S[0]->minTime
				vector<pair<pair<pair<int, double>, double>, int>> sb;
				sb.reserve(2);
				sb.clear();
				sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
				sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
				sort(sb.begin(), sb.end());
				if (sb.begin()->second == 1)//S[0]更好
				{
					pbest->R = R;
					pbest->total_time = total_time;
					pbest->total_cost = total_cost;
					pbest->total_dist = total_dist;
				}
				//添加到hash
				bool news = 0;
				size_t code = gethashvalue();
				if (pbest->findinhashtable(code))
					news = 0;
				else
				{
					news = 1;
					pbest->add2hashtable(code, total_cost);
				}
				checkR(0, 1);

			}
			





			double td = util::nowtime();
#ifdef PRINTF

#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#endif // _WIN32
			printf("fail");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				 total_dist, total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#endif//PRINTF
			
#ifdef WRITE
			sprintf(buff, "fail\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif

		}
		else
		{
			total_cost = 0;
			total_dist = 0;
			for (auto it = R.begin(); it != R.end(); it++)
			{
				total_cost += (*it).route_cost;
				total_dist += (*it).route_dist;
			}
			total_time = util::nowtime() - g_d->begin_time;

			//scueess的时候clear hash 然后再添加
			//clear_hashtable();

			size_t code = gethashvalue();
			if (pbest->findinhashtable(code))
				;
			else
			{
				pbest->add2hashtable(code, total_cost);
			}


			croute++;

			double td = util::nowtime();
			
	

#ifdef PRINTF
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif // _WIN32
			printf("success");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32

			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
#endif // PRINTF
			
#ifdef WRITE
			sprintf(buff, "success\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif
			
			
			if (R.size() < pbest->R.size())
			{
				pbest->minTime = total_time;
			}
			vector<pair<pair<pair<int, double>, double>, int>> sb;
			sb.reserve(2);
			sb.clear();
			sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
			sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
			sort(sb.begin(), sb.end());
			if (sb.begin()->second == 1)//S[0]更好
			{
				pbest->R = R;
				pbest->total_time = total_time;
				pbest->total_cost = total_cost;
				pbest->total_dist = total_dist;
			}

			


		}

	}/* while (1);*///endwhile(not finish)



#ifdef CHECK
	checkR(0, 1);
#endif
}

void Solution::ejection_insertion( )
{
	int croute = 0;
	double fa_tcost = 0;
	Pbest = INT_MAX;
	int fa = 0;
	Node root(0, 0, nullptr);
	for (int u = 0; ; u++)//参数
	{//procedure_delete_route
		if (util::nowtime() - g_d->begin_time > PARA.TIME[PARA.i] * 1000)
		{
			break;
		}

		if (R.size() == 1)
		{
			system("pause");
		}
		tmpR = R;
		fa_tcost = total_cost;
		//checkR(0,1);
		int randroute = rand() % R.size();
		std::list<Route> EP;
		{//1
		 //removed的order
			auto iter_delete = next(R.begin(), randroute);
			EP.splice(EP.begin(), R, iter_delete);
		}

		std::list<pthelm> &tmppath = EP.front().path;
		{//2
			removed.clear();
			removed.reserve(20);
			//for (auto p1 = (*pr_origin)->path.begin(); p1 != (*pr_origin)->path.end(); ++p1) {
			for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
				if (p1->pcus->p == 1)//pickup
				{
					removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
				}
			}

		}
		int orignsize = static_cast<int>(removed.size());

		int order_num = (g_d->cus_num + 1) / 2;
		int c = 0;//迭代次数
		
		int c_max = (order_num*order_num) / static_cast<int>(R.size());//size(R)+1 remove之前 or 之后


		p.assign(p.size(), 1);//3

		Pbest = INT_MAX;

		//double c_time = nowtime();
		while (!removed.empty() && c < c_max)
		{//对每个需要插入的order进行遍历
			c++;
			int temp_index = rand() % removed.size();
			removepool temp = removed[temp_index];

			vector<insertpij> record_insert;//记录有几种插入方案 随机选一个
			record_insert.reserve(R.size());

			//double insert_start = nowtime();
			int route_index = 0;
			for (auto itr = R.begin(); itr != R.end(); itr++, route_index++)//对每条路径进行遍历
			{

				insert_route(tmppath, temp, &*itr, record_insert, route_index);//把一个order插入到一条路径

				temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
				//= EP.begin();
			}



			if (!record_insert.empty())//随机选一个
			{
				size_t randselct = rand() % record_insert.size();


				int numth = record_insert[randselct].rindex;//第几条路径 rindex从0开始
				auto itr = next(R.begin(), numth);

				(itr)->path.splice(record_insert[randselct].itjp, tmppath, temp.it_pthelmp);
				(itr)->path.splice(record_insert[randselct].itjd, tmppath, temp.it_pthelmd);
				temp.it_pthelmd->proute = temp.it_pthelmp->proute = itr;

				//不进行check注释 必须update
				int ret = updata(itr);
				_assert(ret != 0);

				/*********************************随机选择一个order删除************************************************/
				auto itremo = next(removed.begin(), temp_index);
				removed.erase(itremo);
				//cperturb=perturb(0.3*Irand);//迭代30次
			}

		 
			else
			{//ejection if hin can not be insert
			 //double eject_start = nowtime();
				p[temp.remove_order->id]++;//penalty + 1
				EjectionSearch ejection1(g_d);//初始化 kmax=2  每次新建psum pbest会被清空 赋初始值
											  //每插入一个新的点 重新建一个对象 即之前的值全都清零 重新初始化
				ejection1.pS0 = this;
				route_index = 0;



				int autoint = rand() % R.size();
				route_index = autoint;
				list<Route>::iterator itr = next(R.begin(), route_index);
				for (/*auto itr = R.begin()*/; itr != R.end(); itr++, route_index++)//对每条路径进行遍历
				{
					ejection1.tmpr = &*itr;
					ejection1.rindex = route_index;
					ejection1.ptmp_path = &tmppath;
					ejection1.temporder = &temp;
					ejection1.eject_route(root);//把一个order插入到一条路径
					temp.it_pthelmd->proute = temp.it_pthelmp->proute = EP.begin();
					if (!ejection1.record_eject.empty())
					{
						break;
					}
				}


				ejectpij_best  eject;
				ejection1.record_eject.push_back(eject);
				if (!ejection1.record_eject.empty())
				{
					int randselct = rand() % ((ejection1.record_eject.size()));

					if (randselct == ((ejection1.record_eject.size()) - 1))
					{
						double rd = rand() / double(RAND_MAX);
						if (rd >= 0.5) {
							randselct = rand() % (ejection1.record_eject.size());
						}
					}
					if (randselct == ((ejection1.record_eject).size() - 1))//以较小概率选择 50%
					{//原路径 不作任何变化 没有insert 没有eject
					 //
					 //printf("removed_size:%d; select_order:%d; record_eject %d 选择原路径 \n", removed.size(), temp.remove_order->id,size(ejection1.record_eject));
					}
					else {
						auto& pijchuz = ejection1.record_eject[randselct];
						int numth = pijchuz.rindex;//第几条路径 rindex从0开始
						auto itr = next(R.begin(), numth);

						auto ret = updata(itr);
						_assert(ret != 0);

						itr->path.splice(pijchuz.itjpn, tmppath, temp.it_pthelmp);
						temp.it_pthelmp->proute = itr;
						auto itjdn = next((itr)->path.begin(), pijchuz.din + 1);
						(itr)->path.splice(itjdn, tmppath, temp.it_pthelmd);//insert
						temp.it_pthelmd->proute = itr;

						for (auto it = pijchuz.it_ejection.rbegin(); it != pijchuz.it_ejection.rend(); ++it)
						{
							tmppath.splice(prev(tmppath.end()), (itr)->path, *it);
							(*it)->proute = EP.begin();
						}
						ret = updata(itr);
						_assert(ret != 0);
						removed.clear();

						for (auto p1 = tmppath.begin(); p1 != tmppath.end(); ++p1) {
							if (p1->pcus->p == 1)//pickup
							{
								removed.emplace_back(p1->porder, p1, p1->porder->pelmd[snum]);
							}
						}
#ifdef CHECK
						checkR(removed.size(), 1);
#endif //
					}
				}//end updata solution
				 //double perturb_start = nowtime();
#ifdef CHECK
				checkR(removed.size(), 1);
#endif
				//tabuList();
				order_perturb(Irand);//迭代100次
									 //cperturb = perturb(Irand);//迭代100次
			}//endif ejection

		}//endfor ( removed.size()!=0 )


		if (!removed.empty())
		{//初始化时用传统GES
			R = tmpR;
			//不可以注释
			checkR(0, 1);
			break;
			if (g_d->baseR == 0)//base
			{
				R = tmpR;
				//不可以注释
				checkR(0, 1);
			    break;
			}
			else
			{
				//不恢复removed自成一条路径 ifremoved一条路径则用 否则不用（即恢复）
				Route tmp(g_d, snum);

				//radd = &rAdd[++raddindex];

				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmd);
				tmp.path.splice(tmp.path.begin(), tmppath, removed[0].it_pthelmp);

				removed.erase(removed.begin());
				tmp.path.push_front(pthelm(od[0].pp, od));
				tmp.path.push_back(pthelm(od[0].pd, od));

				//tmp.updata();
				//insert
				R.emplace_back(tmp);
				auto& radd = R.back();
				auto Rend = prev(R.end());
				updata(Rend);
				//radd.updata();
				int s = removed.size();
				for (int h = 1; h < max(50, s*s); h++)
				{
					if (removed.size() == 0)
					{
						break;
					}
					int temp_index = rand() % removed.size();
					removepool temp = removed[temp_index];
					std::vector<insertpij> rinsert;//record_insert
					insert_route(tmppath, temp, &radd, rinsert, int(R.size()));

					if (!rinsert.empty())
					{
						int randselct = rand() % rinsert.size();
						(radd).path.splice(rinsert[randselct].itjp, tmppath, temp.it_pthelmp);
						(radd).path.splice(rinsert[randselct].itjd, tmppath, temp.it_pthelmd);
						temp.it_pthelmp->proute = Rend;
						temp.it_pthelmd->proute = Rend;
						int ret = updata(Rend);
						if (ret == 0)//fausle infeasible
						{
							printf("error");
						}
						/*********************************随机选择一个order删除************************************************/
						auto itremo = removed.begin();
						advance(itremo, temp_index);
						removed.erase(itremo);

					}
					else {
						continue;
					}


				}
				if (!removed.empty())
				{//fail，恢复

					R = tmpR;
					checkR(0, 1);//此处check不可被注释
					break;;
				}
				//此处check不可被注释
				checkR(0, 1);
				//即使fail也记录这个解 此次循环得到的解是否是新解

				//Sobest 与 S[0]->minTime
				vector<pair<pair<pair<int, double>, double>, int>> sb;
				sb.reserve(2);
				sb.clear();
				sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
				sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
				sort(sb.begin(), sb.end());
				if (sb.begin()->second == 1)//S[0]更好
				{
					//pbest->copy(*this, pbest->snum);
					pbest->R = R;
					pbest->total_time = total_time;
					pbest->total_cost = total_cost;
					pbest->total_dist = total_dist;
				}

				//添加到hash
				bool news = 0;
				size_t code = gethashvalue();
				if (pbest->findinhashtable(code))
					news = 0;
				else
				{
					news = 1;
					pbest->add2hashtable(code, total_cost);
				}
			
			}


			double td = util::nowtime();
#ifdef PRINTF

#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#endif // _WIN32
			printf("fail");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				 total_dist,total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#endif//PRINTF

#ifdef WRITE
			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				total_dist, total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif
			//break;////////////////////////////////////////////////////////////////////////////////////////

		}
		else
		{
			total_cost = 0;
			total_dist = 0;
			for (auto it = R.begin(); it != R.end(); it++)
			{
				total_cost += (*it).route_cost;
				total_dist += (*it).route_dist;
			}
			total_time = util::nowtime() - g_d->begin_time;


			if (R.size() < pbest->R.size())
			{
				pbest->minTime = total_time;
			}
			vector<pair<pair<pair<int, double>, double>, int>> sb;
			sb.reserve(2);
			sb.clear();
			sb.emplace_back(make_pair(make_pair(make_pair(pbest->R.size(), pbest->total_dist), pbest->total_time), 0));
			sb.emplace_back(make_pair(make_pair(make_pair(R.size(), total_dist), total_time), 1));
			sort(sb.begin(), sb.end());
			if (sb.begin()->second == 1)//S[0]更好
			{
				pbest->R = R;
				pbest->total_time = total_time;
				pbest->total_cost = total_cost;
				pbest->total_dist = total_dist;
			}

			//scueess的时候clear hash 然后再添加
			//clear_hashtable();

			size_t code = gethashvalue();
			if (pbest->findinhashtable(code))
				;
			else
			{
				pbest->add2hashtable(code, total_cost);
			}


			croute++;

			double td = util::nowtime();

#ifdef PRINTF
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif // _WIN32
			printf("success");
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32

			sprintf(buff, "\t\t%-10.2f%-10.2f%-8zd%-8.1f ms\n",
				 total_dist,total_cost, R.size(), (td - g_d->begin_time));
			printf(buff);
#ifdef _WIN32
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
#endif // _WIN32
#endif // PRINTF

#ifdef WRITE
			sprintf(buff, "success\t\t%-8d%-8d%-8d%-8d%-8d%-8zd%-10.2f%-8zd%-8.1f ms\n",
				1, u, c, randroute, orignsize, removed.size(), total_cost, R.size(), (td - g_d->begin_time));
			g_d->each_problem_output << buff;
#endif
			
		}

	}/* while (1);*///endwhile(not finish)



#ifdef CHECK
	checkR(0, 1);
#endif
}
void Solution::insert_route(std::list<pthelm> &tmppthelm, removepool &temporder,
	Route *rt, vector<insertpij> &record_insert, int route_index)
{
	//Route r1 = *rt;
	std::list<pthelm> *ppj = &rt->path;//取址
	list<pthelm>::iterator itjp = next(ppj->begin());
	int pthelm_pindex = 1;//从1开始
	int pthelm_dindex;
	//返回可行解集合
	double origincost = rt->route_cost;
	list<pthelm>::iterator itjpp, itjdd;

	//ppj->insert(itjp, *temporder.pelmp);
	auto tmppthelm_itpn = next(temporder.it_pthelmp);
	auto tmppthelm_itdn = next(temporder.it_pthelmd);
	//计算lpt
	rt->lastpossible();
   int increment = 0;
	ppj->splice(itjp, tmppthelm, temporder.it_pthelmp);

	do {


		int charge = insert_chck(temporder.it_pthelmp, temporder.it_pthelmp, increment);
#ifdef CHECK
		int vio = 0;
		pr_origin->updata(vio);//
		if (pr_origin->twv > 0 && charge == 1)
		{
			my_ERROR
		}
		if (pr_origin->twv == 0 && charge != 1)
		{
			my_ERROR
			insert_chck(temporder.it_pthelmp, temporder.it_pthelmp, increment);
			pr_origin->updata(vio);
		}
#endif
		if (charge == -2)
		{
			break;
		}
		if (charge == -1)
		{
			itjp++;
			pthelm_pindex++;
			if (itjp == ppj->end()) { break; }
			ppj->splice(itjp, *ppj, temporder.it_pthelmp);
			increment = 1;
			continue;

		}//charge==1继续往后算

		list<pthelm>::iterator itjd = itjp;
		pthelm_dindex = pthelm_pindex;
		ppj->splice(itjd, tmppthelm, temporder.it_pthelmd);
	//	temporder.it_pthelmd->proute = this;
		int incrementd = 0;
		do {


			int charge = insert_chck(temporder.it_pthelmp, temporder.it_pthelmd, incrementd);
#ifdef CHECK
			auto aaa = pr_origin->updata();
			if (aaa == 1 && charge != 1)
			{
				my_ERROR
			}
			if (aaa == 0 && charge == 1)
			{
				my_ERROR
			}
#endif
			if (charge == -2)
			{
				break;
			}
			if (charge == 1)
			{
				insertpij pij;//每次都要重新创建一个 然后将新的pij push_back
				const std::list<pthelm>::iterator itjp1 = itjp, itjd1 = itjd;
				pij.itjd = itjd1;
				pij.itjp = itjp1;
				pij.rindex = route_index;
				pij.dindex = pthelm_dindex;
				pij.pindex = pthelm_pindex;

//				pij.dcost = rt->getcostonly() - origincost;
				record_insert.emplace_back(pij);
			}//charge==-1 || charge==1都是往后挪


			itjdd = prev(itjd);
			//--itjdd;
			itjd++;
			pthelm_dindex++;
			if (itjd == ppj->end()) { break; }
			ppj->splice(itjd, *ppj, temporder.it_pthelmd);//itjdd
			incrementd = 1;
		} while (1);
		tmppthelm.splice(tmppthelm_itdn, *ppj, temporder.it_pthelmd);

		itjp++;
		pthelm_pindex++;
		if (itjp == ppj->end()) { break; }
		ppj->splice(itjp, *ppj, temporder.it_pthelmp);
		increment = 1;
	} while (1);

	tmppthelm.splice(tmppthelm_itpn, *ppj, temporder.it_pthelmp);//itjpp


}


void EjectionSearch::eject_route(Node& root)
{
	std::list<pthelm> *ppj = &tmpr->path;//取址
	itjp = ppj->begin();
	itjp++;
	itjp_index = 1;//从1开始编号 第一个 插入进去之后就在第一个的位置
				   //返回可行解集合
				   //list<pthelm>::iterator itjpp, itj;
	auto tmppthelm_itpn = next(temporder->it_pthelmp);


	ppj->splice(itjp, *ptmp_path, temporder->it_pthelmp);
	//Node root(0, 0, nullptr);
	do {
		//ppj->insert(itjp, *temporder.pelmp);//插入pickup 插入在itjp之前   eject & divelary 只能在第一个后面 第n个后面						//插入pickup后 不需要判断
		tmpr->lastpossible();//
		root.init(0, 0, nullptr);
		//root.adj.clear();

		root.cont.itr = tmpr->path.begin();
		root.cont.atime_new = max(0, (*root.cont.itr).pcus->etime);
		for (jd = 1; jd <= 2 * kmax + 1; jd++)
		{//词典搜索 从1开始编号 d loop
			lexicographic_iterativebacktrack(int((*ppj).size()) - 2, root);//一次就把所有可能都搜索出来了 第几个后面
		}// <0 1 2 3 4 n n+1>

		itjp++;
		itjp_index++;
		if (itjp == ppj->end()) {
			break;
		}
		_assert(itjp != ppj->end() || itjp_index == (*ppj).size() - 1);
// 		if (itjp == ppj->end() && itjp_index != (*ppj).size() - 1)
// 		{
// 			printf("itjp == ppj->end() && pin!= size(*ppj) - 2 \n");
// 			my_ERROR
// 		}
		ppj->splice(itjp, *ppj, temporder->it_pthelmp);


	} while (1);
	ptmp_path->splice(tmppthelm_itpn, *ppj, temporder->it_pthelmp);
}


//return 0 仅仅此条组合错了
//return -1 自己的子代不用算了
//return -2 /其之后的兄弟 及 自己和兄弟的子代都不用算了
//retun 1  正确
int EjectionSearch::prune(Node& head, int n, bool increment)
{//一眼就能看出来错的

	std::list<pthelm> *ppj = &tmpr->path;//取址
	head.cont.itr = ppj->end();

	if (head.level != jd)
	{//eject eject后 后面的atime会变化o
		advance(head.cont.itr, -(n - head.num + 3));//eject 前一个

		_assert(head.cont.itr != ppj->end());

		head.cont.eject_itr = head.cont.itr;//itretor指向eject的那个
		advance(head.cont.eject_itr, +1);
		head.cont.pej = &(*head.cont.eject_itr);//eject的那个pthelement
												//head.cont.did_eject_cusid.push_back(head.cont.pej->pcus->cusid);
		head.cont.cus_id = head.cont.pej->pcus->cusid;
		tmppath.splice(tmppath.end(), *ppj, head.cont.eject_itr);
		//_assert(tmppath.size()<=1);
		if (increment == 1)
		{//检查i(j)-1
			auto itt = head.cont.itr;//当前i(j)的前一个 i[j]-1
			auto it = prev(itt, 1);//上一次i(j)的前一个
			(*itt).atime = max((*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid],
				(*itt).pcus->etime);
			itt->q = it->q + itt->pcus->demand;
		}
		head.cont.atime_new = (*head.cont.itr).atime;
		head.cont.q_new = head.cont.itr->q;
		//检查i(j)+1
		auto itt = next(head.cont.itr);//i(j)+1
		(*itt).atime = max((*head.cont.itr).atime + (*head.cont.itr).pcus->stime +
			g_d->dist[(*head.cont.itr).pcus->cusid][(*itt).pcus->cusid], (*itt).pcus->etime);
		itt->q = head.cont.itr->q + itt->pcus->demand;

		if (head.cont.pej->pcus->p == 1)
		{
			head.cont.countp++;
		}
		else
		{
			head.cont.countd++;
		}
		head.cont.psum += pS0->p[head.cont.pej->porder->id];
		if (increment == 1)
		{
			if (head.cont.itr->atime > head.cont.itr->pcus->ltime || head.cont.itr->q > g_d->cap)
			{
				head.cont.isprune = 1;
				return -2;//剪枝 {i[1]...i[j-1],v...}  v>i[j]-1    break 循环 其兄弟都不用看了
			}
			if (head.cont.itr->pcus->d == 1)//i[j]-1是delivery 其pickup被eject 则剪枝
			{
				auto tmp = &head;
				while (tmp->father != nullptr) {
					if (tmp->level == jd)
					{
						tmp = tmp->father; continue;
					}
					if (tmp->cont.cus_id == head.cont.itr->porder->pp->cusid)
					{
						head.cont.isprune = 1;
						return -2;//剪枝 {i[1]...i[j-1],v...}
					}

					tmp = tmp->father;
				}
			}
		}
		_assert(head.cont.psum != 0);

		if (head.cont.psum > pS0->Pbest)
		{
			head.cont.isprune = 1;
			return -1;//剪枝 {i[1]...i[j]...} i[j]的所有儿子都不用搜索了
		}


		if (head.cont.pej->pcus->p == 1)
		{
			//head.cont.countp++;
			if (head.cont.countp > kmax)
			{
				head.cont.isprune = 1;
				return -1;
			}
		}

		if (head.cont.pej->pcus->d == 1)
		{//x[i]是delivery 且 已经eject 相对应的pickup还没有eject
		 //
			bool found = 0;
			auto tmp = &head;
			while (tmp->father != nullptr) {
				if (tmp->level == jd)
				{
					tmp = tmp->father; continue;
				}
				if (tmp->cont.cus_id == head.cont.pej->porder->pp->cusid)
				{
					found = 1;
					break;
				}

				tmp = tmp->father;
			}
			if (found == 0) {
				head.cont.isprune = 1;
				return -1;
			}

			// 			if ((find(head.cont.did_eject_cusid.begin(), head.cont.did_eject_cusid.end(), head.cont.pej->porder->pp->cusid))
			// 				== head.cont.did_eject_cusid.end())
			// 			{
			// 				head.cont.isprune = 1;
			// 				return -1;
			// 			}
		}

		if (head.num == itjp_index)//把pin eject掉了
		{
			head.cont.isprune = 1;
			return -1;//子代不用算了
		}
		if (itt->atime > itt->lpt || itt->q > g_d->cap)
		{
			return 0;//只是此条组合不合法
		}
	}
	else
	{//dilivery插在x[jd]之后

		advance(head.cont.itr, -(n - head.num + 1));
		ppj->splice(head.cont.itr, *ptmp_path, temporder->it_pthelmd);
		advance(head.cont.itr, -2);;//insert 进去的前一个 即 i(jd)
									//head.cont.pej = *tempod.pelmd;//insert的那个 dilivery插在x[jd]之后
		head.cont.pej = &*temporder->it_pthelmd;
		if (increment == 1)
		{//检查i(jd)
			auto itt = head.cont.itr;//i(jd)
			auto it = prev(itt, 1);//i(jd)-1
			(*itt).atime = max((*it).atime + (*it).pcus->stime + g_d->dist[(*it).pcus->cusid][(*itt).pcus->cusid], (*itt).pcus->etime);
			itt->q = it->q + itt->pcus->demand;
		}

		//insert 检查i(jd)+1
		/*重复算了一次*/
		auto itpr = prev(head.cont.itr, 1);//it=i[jd]-1
		auto itt = head.cont.itr;//循环三次 分别是
								 //i[jd]
								 //insert 进去的那个  dilivery插在x[jd]之后
								 //i[jd]+1
		for (int i = 0; i < 3; i++)
		{
			(*itt).atime = max((*itpr).atime + (*itpr).pcus->stime + g_d->dist[(*itpr).pcus->cusid][(*itt).pcus->cusid],
				(*itt).pcus->etime);
			itt->q = itpr->q + itt->pcus->demand;
			itpr++;
			itt++;
		}
		itt--;//i[jd]+1


			  // 		if (increment == 1)
			  // 		{
		if (head.cont.itr->atime > head.cont.itr->pcus->ltime || head.cont.itr->q > g_d->cap)
		{////判断i(jd)
			head.cont.isprune = 1;
			return -2;//其之后的兄弟 及 自己和兄弟的子代都不用算了
		}
		//		}
		if (temporder->it_pthelmd->atime > temporder->it_pthelmd->pcus->ltime)
		{
			head.cont.isprune = 1;
			return -2;//插进i[jd]后面的那个insert不满足时间窗  -2
		}

		if (head.num < itjp_index)//head.level == jd && head.num < itjp_index
		{
			head.cont.isprune = 1;
			return -1;//其子代不用算了
		}
		if (head.cont.itr->pcus->p == 1 && head.cont.itr->pcus->cusid != head.cont.pej->porder->pp->cusid)//i[jd]是pickup,即pickup后面插入另一个元素的delivery
		{// i[jd]是pickup, 即pickup后面插入另一个元素的delivery
			head.cont.isprune = 1;
			return -1;//其子代不用算了
		}

		if (temporder->it_pthelmd->q > g_d->cap)
		{
			head.cont.isprune = 1;
			return -1;//插进i[jd]后面的那个insert不满足时间窗  -2
		}

		stack<int> slifo;
		for (auto it = temporder->it_pthelmp; it != next(temporder->it_pthelmd); it++)//pin 到 din
		{
			if (it->pcus->p == 1 && it->pcus->cusid != 0)//pickup 入栈
			{
				slifo.push(it->pcus->cusid);
			}
			if (it->pcus->d == 1 && it->pcus->cusid != 0)//delivery 出栈
			{
				if (slifo.top() == it->pcus->pid)
				{
					slifo.pop();//删除栈顶
				}
				else
				{
					return -1;//infeasible 以后的子代不用算了
				}
			}
		}
		if (!slifo.empty())
		{
			return -1;//以后的子代不用算了
		}


		if (itt->atime > itt->lpt || itt->q > g_d->cap)
		{
			return 0;
		}

	}

	return 1;
}

bool EjectionSearch::constraint(const Node &head)
{

	if (head.cont.countp != head.cont.countd) return false;
	if (head.level < jd) return false;
	else {
		if (head.num < itjp_index) return false;//不满足前后顺序
	}

	//判断did_eject是否成对
	// 	for (auto it = head.cont.did_eject_cusid.begin(); it != head.cont.did_eject_cusid.end(); it++)
	// 	{
	// 		if (g_d->allcustomer[*it].p == 1)
	// 		{
	// 			if ((find(head.cont.did_eject_cusid.begin(), head.cont.did_eject_cusid.end(), g_d->allcustomer[*it].did))
	// 				== head.cont.did_eject_cusid.end())
	// 			{
	// 				return 0;
	// 			}
	// 		}
	// 		if (g_d->allcustomer[*it].d == 1)
	// 		{
	// 			if ((find(head.cont.did_eject_cusid.begin(), head.cont.did_eject_cusid.end(), g_d->allcustomer[*it].pid))
	// 				== head.cont.did_eject_cusid.end())
	// 			{
	// 				return 0;
	// 			}
	// 		}
	// 	}


	//	int summ = 0;
	//summ = 0;
	// 	auto tmp = &head;
	// 	while (tmp->father != nullptr) {
	// 		if (tmp->level == jd)
	// 		{
	// 			tmp = tmp->father; 
	// 			continue;
	// 		}
	// 		if (tmp->cont.eject_itr->pcus->p == 1)
	// 		{
	// 			summ += tmp->cont.eject_itr->porder->id;
	// 		}
	// 		else
	// 		{
	// 			summ -= tmp->cont.eject_itr->porder->id;
	// 		}
	// 		tmp = tmp->father;
	// 	}
	// 	if (summ != 0)
	// 	{
	// 		return 0;
	// 	}
	//LIFO 判别
	//eject 如果成对，则肯定满足LIFO
	//pin din中间如果成对（包括0），则满足LIFO
	//定位到pin的位置  temporder->it_pthelmp

	return true;
}

void Solution::order_perturb(int irand)
{
	//随机选一个request 除了removed之外的
	// simple relocate
	// extended exchange
	
	
	for (int i = 1; i <= irand; i++)
	{
		
		std::list<pthelm>::iterator it_pthelmp, it_pthelmd;
		Riterator itroute;
		if (R.size() == 1)
		{
			Pex = 1.0;
			Pex2 = 1.0;
		}
		int rand_order;
		while (1) {
			rand_order = rand() % g_d->pdd_index;
			if (rand_order == 0 || removed.end() != find_if(removed.begin(), removed.end(),
				[&rand_order](const removepool& rp) {return rp.remove_order->id == rand_order; }))
				continue;//在removed里面找到了
			else break;
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
// 		if (itroute->path.size()==2)
// 		{
// 			tmp.route_cost = 0;
// 		}
// 		else
// 			tmp.route_cost = itroute->getcostonly();//变化后的cost
		
		vector<Perturb> record_perturb;
		record_perturb.reserve(100);
		double perR = rand() / double(RAND_MAX);
		if (perR <= Pex)//// simple relocate   Pex
		{
			record_perturb.clear();
			for (auto rit = R.begin(); rit != R.end(); rit++)
			{

				if ((rit) == itroute) continue;
	//			double oricost = rit->route_cost +itroute->route_cost;//变化前的总cost
				perturbPD(rit, &tmp, it_pthelmp, it_pthelmd, record_perturb/*, oricost*/);
				
			}
			if (!record_perturb.empty())
			{
				
				size_t randselct = rand() % record_perturb.size();
// 				if (g_d->testt==1)
// 				{
// 					sort(record_perturb.begin(), record_perturb.end(),
// 						[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 					double q = rand() / (double)RAND_MAX;
// 
// 					randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));
// 					if (randselct >= record_perturb.size())
// 					{
// 						randselct = record_perturb.size() - 1;
// 					}
// 					
// 				}
				Perturb r1 = record_perturb[randselct];

				
				(r1.itroute)->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp);
				(r1.itroute)->path.splice(r1.it_pthelmd, tmp.path, it_pthelmd);

				//必须updata 不能注释
				int ret = updata(r1.itroute);
// 				if (ret == 0)//infeasible
// 				{
// 					g_d->each_problem_output << "route error13!!!\n";
// 					my_ERROR
// 				}
				ret = updata(itroute);
// 				if (ret == 0)//infeasible
// 				{
// 					g_d->each_problem_output << "route error13!!!\n";
// 					my_ERROR
// 				}
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
			order* rorder2 = &od[rand_order2];

			auto it_pthelmp2 = rorder2->pelmp[snum];
			auto it_pthelmd2 = rorder2->pelmd[snum];
			Route tmp2(g_d, snum);
			auto tmpn2 = next(it_pthelmp2);
			auto tmdn2 = next(it_pthelmd2);
			tmp2.path.splice(tmp2.path.begin(), itroute2->path, it_pthelmp2);
			tmp2.path.splice(tmp2.path.end(), itroute2->path, it_pthelmd2);
			
			if (itroute2->path.size()==2 && itroute->path.size()==2)
			{
				itroute2->path.splice(tmdn2, tmp2.path, it_pthelmd2);
				itroute2->path.splice(tmpn2, tmp2.path, it_pthelmp2);
				itroute->path.splice(tmdn, tmp.path, it_pthelmd);
				itroute->path.splice(tmpn, tmp.path, it_pthelmp);
				return ;
			}
// 			if (itroute2->path.size() == 2)
// 			{
// 				tmp2.route_cost = 0;
// 			}
// 			else
// 				tmp2.route_cost = itroute2->getcostonly();//变化后的cost

			//将it_pthelmp 和 it_pthelmd insert到itroute2->path
			//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path
		
		//	double oricost = itroute->route_cost + itroute2->route_cost;
			perturbPD(itroute2, &tmp, it_pthelmp, it_pthelmd, record_perturb/*, oricost*/);

			vector<Perturb> record_perturb2;
			if (!record_perturb.empty())//有合适的位置
			{//再将tmp2 的 it_pthelmp2 和 it_pthelmd2 splice 到  itroute1->path

				perturbPD(itroute, &tmp2, it_pthelmp2, it_pthelmd2, record_perturb2/*, oricost*/);

				if (!record_perturb2.empty() && !record_perturb.empty())
				{//从record_perturb2 和 record_perturb 中 各选一个
			
					size_t randselct = rand() % record_perturb.size();
// 					if (g_d->testt==1)
// 					{
// 						sort(record_perturb.begin(), record_perturb.end(),
// 							[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 						double q = rand() / (double)RAND_MAX;
// 
// 						randselct = static_cast<size_t>(floor(pow(q, 5)*record_perturb.size()));
// 						if (randselct >= record_perturb.size())
// 						{
// 							randselct = record_perturb.size() - 1;
// 						}
// 					}


					Perturb r1 = record_perturb[randselct];

					itroute2->path.splice(r1.it_pthelmp, tmp.path, it_pthelmp);
					itroute2->path.splice(r1.it_pthelmd, tmp.path, it_pthelmd);

					size_t randselct2 = rand() % record_perturb2.size();
// 					if (g_d->testt==1)
// 					{
// 						sort(record_perturb2.begin(), record_perturb2.end(),
// 							[](Perturb &i, Perturb &j) {return i.ccost < j.ccost; });
// 						double q2 = rand() / (double)RAND_MAX;
// 
// 						size_t randselct2 = static_cast<size_t>(floor(pow(q2, 5)*record_perturb2.size()));
// 						if (randselct2 >= record_perturb2.size())
// 						{
// 							randselct2 = record_perturb2.size() - 1;
// 						}
// 					}
					

					Perturb r2 = record_perturb2[randselct2];

					itroute->path.splice(r2.it_pthelmp, tmp2.path, it_pthelmp2);
					itroute->path.splice(r2.it_pthelmd, tmp2.path, it_pthelmd2);


					//必须updata 不可以注释
					int ret = updata(itroute);
// 					if (ret == 0)//infeasible
// 					{
// 						g_d->each_problem_output << "route error13!!!\n";
// 						my_ERROR
// 					}
					ret = updata(itroute2);
// 					if (ret == 0)//infeasible
// 					{
// 						g_d->each_problem_output << "route error13!!!\n";
// 						my_ERROR
// 					}

				}
				else
				{
					itroute2->path.splice(tmdn2, tmp2.path, it_pthelmd2);
					itroute2->path.splice(tmpn2, tmp2.path, it_pthelmp2);

					itroute->path.splice(tmdn, tmp.path, it_pthelmd);
					itroute->path.splice(tmpn, tmp.path, it_pthelmp);

					continue;
				}

			}
			else//没有合适的位置
			{
				itroute2->path.splice(tmdn2, tmp2.path, it_pthelmd2);
				itroute2->path.splice(tmpn2, tmp2.path, it_pthelmp2);

				itroute->path.splice(tmdn, tmp.path, it_pthelmd);
				itroute->path.splice(tmpn, tmp.path, it_pthelmp);

				continue;
			}
			//printf("exchange\n");
		}


		

	}
	//return pS / (double)irand;

}
// void Solution::perturbD(Riterator rit, Route* itroute, std::list<pthelm>::iterator itjp,
// 	std::list<pthelm>::iterator it_pthelmd, std::list<pthelm>::iterator itdn, vector<Perturb> &record_perturb)
// {
// 	std::list<pthelm> *ppj = &(rit)->path;
// 	auto it_pthelmp = prev(itjp, 1);
// 	auto itjd = itjp;
// 
// 	if (itjd == ppj->end())
// 	{
// 		return;
// 	}
// 	double oricost = rit->route_cost + itroute->route_cost;
// 	ppj->splice(itjd, itroute->path, it_pthelmd);
// 	
// 
// 	do {
// 		double upc = 0;
// 		if((rit)->updata_lcheck(upc)==1)//feasible
// 		{
// 		
// 			Perturb feasible;
// 			feasible.it_pthelmp = itjp;
// 			feasible.it_pthelmd = itjd;
// 			feasible.itroute = (rit);
// 
// 			feasible.ccost = upc + itroute->route_cost - oricost;//计算solution的cost
// 			record_perturb.push_back(feasible);
// 		}
// 
// 		itjd++;
// 		if (itjd == ppj->end()) { break; }
// 		ppj->splice(itjd, *ppj, it_pthelmd);
// 		
// 
// 	} while (1);
// 	itroute->path.splice(itdn, *ppj, it_pthelmd);
// 
// }
// void Solution::perturbPD(Riterator rit, Route* itroute, std::list<pthelm>::iterator it_pthelmp,
// 	std::list<pthelm>::iterator it_pthelmd, vector<Perturb> &record_perturb)
// {
// 	std::list<pthelm> *ppj = &(rit)->path;//取址
// 	auto itjp = ppj->begin();
// 	itjp++;
// 	int itjp_index = 1;//从1开始编号 第一个 插入进去之后就在第一个的位置
// 					   //返回可行解集合
// 
// 	auto itpn = next(it_pthelmp);//最后返回原位置
// 	auto itdn = next(it_pthelmd);//最后返回原位置
// 	(rit)->lastpossible();
// 	ppj->splice(itjp, itroute->path, it_pthelmp);//一次插入一整段
// 	int increment = 0;
// 
// 	do {
// 
// 		int vio = rit->updata_check(1);//
// 		if (vio == 0) {//不提前break
// 			itjp++;
// 			itjp_index++;
// 			if (itjp == ppj->end()) { break; }
// 			ppj->splice(itjp, *ppj, it_pthelmp);
// 			increment = 1;
// 			continue;
// 		}
// 
// // 		int charge = insert_chck(it_pthelmp, it_pthelmp, increment);
// // #ifdef CHECK
// // 		int vio = 0;
// // 		rit->updata(vio);//
// // 		if (rit->twv > 0 && charge == 1)
// // 		{
// // 			my_ERROR
// // 				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
// // 				{
// // 					auto et = max(iy->st, iy->pcus->etime);
// // 					if (et != iy->atime)
// // 					{
// // 						my_ERROR
// // 					}
// // 				}
// // 		}
// // 		if (rit->twv == 0 && charge != 1)
// // 		{
// // 			my_ERROR
// // 				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
// // 				{
// // 					auto et = max(iy->st, iy->pcus->etime);
// // 					if (et != iy->atime)
// // 					{
// // 						my_ERROR
// // 					}
// // 				}
// // 			insert_chck(it_pthelmp, it_pthelmp, increment);
// // 			rit->updata(vio);
// // 		}
// // #endif
// // 		if (charge == -2)
// // 		{
// // 			break;
// // 		}
// // 		if (charge == -1)
// // 		{
// // 			itjp++;
// // 			itjp_index++;
// // 			if (itjp == ppj->end()) { break; }
// // 			ppj->splice(itjp, *ppj, it_pthelmp);
// // 			increment = 1;
// // 			continue;
// // 
// // 		}//charge==1继续往后算
// 
// 
// 		perturbD(rit, itroute, itjp, it_pthelmd, itdn, record_perturb);
// 
// 		itjp++;
// 		itjp_index++;
// 		if (itjp == ppj->end()) { break; }
// 		ppj->splice(itjp, *ppj, it_pthelmp);
// 		increment = 1;
// 	} while (1);
// 	itroute->path.splice(itpn, *ppj, it_pthelmp);
// 	//it_pthelmp->proute = itroute;
// }


void Solution::perturbD(Riterator rit, Route* itroute, std::list<pthelm>::iterator itjp,
	std::list<pthelm>::iterator it_pthelmd, std::list<pthelm>::iterator itdn, vector<Perturb> &record_perturb/*, double oricost*/)
{
	std::list<pthelm> *ppj = &(rit)->path;
	auto it_pthelmp = prev(itjp, 1);
	auto itjd = itjp;

	if (itjd == ppj->end())
	{
		return;
	}
	//double oricost = rit->route_cost + itroute->route_cost;
	ppj->splice(itjd, itroute->path, it_pthelmd);
	int incrementd = 0;
	//double itrouteCost = itroute->getcostonly();

	do {
		// 		if((rit)->updata()==1)//feasible
		// 		{
		// 			int rr=itroute->updata();
		// 			Perturb feasible;
		// 			feasible.it_pthelmp = itjp;
		// 			feasible.it_pthelmd = itjd;
		// 			feasible.itroute = (rit);
		//
		// 			feasible.ccost = rit->route_cost+itroute->route_cost - oricost;//计算solution的cost
		// 			record_perturb.push_back(feasible);
		// 		}

		int charge = insert_chck(it_pthelmp, it_pthelmd, incrementd);
#ifdef CHECK
		auto aaa = rit->updata();
		if (aaa == 1 && charge != 1)
		{
			my_ERROR
				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
				{
					auto et = max(iy->st, iy->pcus->etime);
					if (et != iy->atime)
					{
						my_ERROR
					}
				}
			charge = insert_chck(it_pthelmp, it_pthelmd, incrementd);
		}
		if (aaa == 0 && charge == 1)
		{
			my_ERROR
				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
				{
					auto et = max(iy->st, iy->pcus->etime);
					if (et != iy->atime)
					{
						my_ERROR
					}
				}
			charge = insert_chck(it_pthelmp, it_pthelmd, incrementd);
		}
#endif
		if (charge == -2)
		{
			break;
		}
		if (charge == 1)
		{

			Perturb feasible;
			feasible.it_pthelmp = itjp;
			feasible.it_pthelmd = itjd;
			feasible.itroute = (rit);

			//feasible.ccost = rit->getcostonly() + itroute->route_cost - oricost;//计算solution的cost
			record_perturb.push_back(feasible);
		}//charge==-1 || charge==1都是往后挪


		itjd++;
		if (itjd == ppj->end()) { break; }
		ppj->splice(itjd, *ppj, it_pthelmd);
		incrementd = 1;

	} while (1);
	itroute->path.splice(itdn, *ppj, it_pthelmd);

}
void Solution::perturbPD(Riterator rit, Route* itroute, std::list<pthelm>::iterator it_pthelmp,
	std::list<pthelm>::iterator it_pthelmd, vector<Perturb> &record_perturb/*, double oricost*/)
{
	std::list<pthelm> *ppj = &(rit)->path;//取址
	auto itjp = ppj->begin();
	itjp++;
	int itjp_index = 1;//从1开始编号 第一个 插入进去之后就在第一个的位置
					   //返回可行解集合

	auto itpn = next(it_pthelmp);//最后返回原位置
	auto itdn = next(it_pthelmd);//最后返回原位置
	(rit)->lastpossible();
	ppj->splice(itjp, itroute->path, it_pthelmp);
	int increment = 0;

	do {

		int charge = insert_chck(it_pthelmp, it_pthelmp, increment);
#ifdef CHECK
		int vio = 0;
		rit->updata(vio);//
		if (rit->twv > 0 && charge == 1)
		{
			my_ERROR
				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
				{
					auto et = max(iy->st, iy->pcus->etime);
					if (et != iy->atime)
					{
						my_ERROR
					}
				}
		}
		if (rit->twv == 0 && charge != 1)
		{
			my_ERROR
				for (auto iy = rit->path.begin(); iy != rit->path.end(); iy++)
				{
					auto et = max(iy->st, iy->pcus->etime);
					if (et != iy->atime)
					{
						my_ERROR
					}
				}
			insert_chck(it_pthelmp, it_pthelmp, increment);
			rit->updata(vio);
		}
#endif
		if (charge == -2)
		{
			break;
		}
		if (charge == -1)
		{
			itjp++;
			itjp_index++;
			if (itjp == ppj->end()) { break; }
			ppj->splice(itjp, *ppj, it_pthelmp);
			increment = 1;
			continue;

		}//charge==1继续往后算

		perturbD(rit, itroute, itjp, it_pthelmd, itdn, record_perturb/*, oricost*/);

		itjp++;
		itjp_index++;
		if (itjp == ppj->end()) { break; }
		ppj->splice(itjp, *ppj, it_pthelmp);
		increment = 1;
	} while (1);
	itroute->path.splice(itpn, *ppj, it_pthelmp);
	//it_pthelmp->proute = itroute;
}