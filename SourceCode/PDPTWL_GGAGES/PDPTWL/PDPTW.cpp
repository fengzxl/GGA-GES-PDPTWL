// PDPTW.cpp : 定义控制台应用程序的入口点。

#ifdef _DEBUG // 内存泄漏检测支持。
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <malloc.h>    // 解决 malloc.h 与 crtdbg.h 顺序导致的 Debug Assertion Failed, "Corrupted pointer passed to _freea" 。
#include <crtdbg.h>
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
void exit1() {
	_CrtDumpMemoryLeaks();
}
#endif// _DEBUG

#include <stdio.h>
// #include "parameters.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <set>
#include <vector>
#include <algorithm>    
#include "io.h"
#include "fileOperator.h"
#include "load.h"
#include "uti.h"
#include "Solution.h"
#include "ejection.h"

#include "thpool.h"
using namespace std;
#ifdef _WIN32
#define MAX_THR 1
#else
#define MAX_THR 12
#endif

string RESULT = "ABCD/";
string Result;
typedef struct inputcontent {//输入
	inputcontent(int runtime, int clty, int twty, int pdp, int f_ind, int baseroute, int populges, int testt, vector<string> file) :
		runtime(runtime), clty(clty), twty(twty), pdp(pdp), j(f_ind), baseroute(baseroute), populges(populges), testt(testt), file(file) {
	}
	inputcontent() {}
	int runtime, clty, twty, pdp, j, baseroute, populges, testt;
	vector<string> file;
}ict;
typedef struct returncontent {//输出
	int vehicle_num = -1;
	string str_outp;
}rct;

struct one_task_tw_input
{
	//vector<int> v_ipt_int;
	int cluster_type, twtype, runtime, pdp, baseroute, populges, testt;
	one_task_tw_input(int cluster_type,int twtype,int runtime,int pdp,int baseroute,int populges,int testt):
		cluster_type(cluster_type), twtype(twtype), runtime(runtime), pdp(pdp), baseroute(baseroute), populges(populges), testt(testt)
	{

	}
};

void one_task(inputcontent ipt, returncontent *pret);

int popul_cross(vector<Solution*> &S, Solution& s0,int CN,int PN);

void CROSS(Solution &s1, Solution &s2);
void CROSS_large(Solution &s1, Solution &s2);
int main()
{

	srand(time(0));

#ifdef _DEBUG //for memory check
	//_CrtSetBreakAlloc(157);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	atexit(exit1);
#endif // _DEBUG
	fileinfo fi;

	
	for (int populges = 1; populges < 2; ++populges)
	{
		for (int baseroute = 1; baseroute < 2; ++baseroute) {//
			{
				if (baseroute == 0)//traditional GES
				{
					Result = RESULT + "populGES/GESbase/";
				}
				else
				{//our improved GES: the creating a new route operator in the GES-based local search
					Result = RESULT + "populGES/GESroute/";
				}
			}

			for (int runtime = 1; runtime <= 10; runtime++)
			{//runtimes loop
			
			
					for (int cluster_type = lc; cluster_type <= lrc; cluster_type++)
					{//cluster_type loop

						for (int twtype = narrow; twtype <= wide; twtype++)
						{// timewindow type loop

	//						one_task_tw(one_task_tw_input(cluster_type, twtype, runtime, pdp, baseroute, populges, testt));
							auto f1 = fi.reg_read(0, cluster_type, twtype);//所有的文件信息
							fileinfo::result_root_path = Result;
							ofstream cltw_writedata(fileinfo::get_cltw_resultfilepath(f1[1], runtime));//打开总结文件
							cout << fileinfo::get_cltw_resultfilepath(f1[1], runtime) << endl;
							if (!cltw_writedata.is_open()) { cerr << "can't open cltw record file" << endl; exit(1); }//判断是否打开

							for (int pdp = pdp100; pdp <= pdp600; pdp++) //NAME={1 2 4 6 8 10}
							{//problem 问题规模 loop

							auto file_ = fi.reg_read(pdp, cluster_type, twtype);
							int maxrange = file_.size() - 1;
							vector<rct> vret(15);
							{//此括号不能去掉
								ThreadPool pool(MAX_THR);
								for (int j = 1; j <= maxrange; j++)
								{//each problem loop
									int testt = rand();
									pool.enqueue(one_task, ict(runtime, cluster_type, twtype, pdp, j, baseroute, populges, testt, file_[j]), &(vret[j]));
								}//end of each problem
							}//等待完成所有的任务
							for (auto &it : vret) {
								if (it.vehicle_num == -1)continue;
#ifdef PRINT
								cout << setw(5) << it.vehicle_num;
								cout << it.str_outp << endl;
#endif // PRINT
								cltw_writedata << it.str_outp << endl;
							}
							}//end of 问题规模 loop
							cltw_writedata.close();
						}//end of time window type loop
					}//end of cluster type loop
				
		}//end of run loop
		}
	}

	


	return 0;
}



void one_task(inputcontent ipt, returncontent* pret) {
	//并行数据
	srand(ipt.testt);
	auto fname = ipt.file;
	rct& ret = *pret;
	auto& outp = (pret->str_outp);
	int runtime = ipt.runtime;
	//end of 并行数据
	g_data* g_d = new g_data;
	ostringstream cltw_writedata;
	//g_d->testt = ipt.testt;
	g_d->begin_time = util::nowtime();
	g_d->baseR = ipt.baseroute;


	string topo_file = fileinfo::get_topofilepath(fname);
	string result_file = fileinfo::get_each_resultfilepath(fname, runtime);

	cltw_writedata <<
		setw(3) << setiosflags(ios::left) << fileinfo::i_toposize[ipt.pdp] <<
		setw(3) << setiosflags(ios::left) << ipt.j;
	//cout << fileinfo::get_filename(fname) << endl;

	int y = g_d->load(topo_file.c_str());
	_assert(y != 0);
	Solution Sobest(g_d, SN + 10);
	Sobest.PARA.i = ipt.pdp;
	Solution s0(g_d, 0);
	s0.pbest = &Sobest;
	g_d->each_problem_output.open(result_file);
	if (!g_d->each_problem_output.is_open())
	{
		cerr << "can't open each_problem_output record file" << endl; exit(1);
	}

	s0.PARA.i = ipt.pdp;
	//s0.tess = ipt.testt;
	s0.construction_sequential();
	//产生初始解种群 要产生四个 定义四个解
	Sobest.copy(s0, Sobest.snum);
	int CN = 2;// the number of individual involved in crossover and mutation.
	int PN = 6;//population size
	pair<int, double> p1;
	Solution s1(s0, 1);
	Solution s2(s0, 2);
	Solution s3(s0, 3);
		Solution s4(s0, 4);
		Solution s5(s0, 5);
// 		Solution s6(s0, 6);
// 		Solution s7(s0, 7);
// 		Solution s8(s0, 8);
// 		Solution s9(s0, 9);
		

	Solution sf0(s0, 11);
	Solution sf1(s0, 12);
	// 		Solution sf2(s0, 10);
	// 		Solution sf3(s0, 11);

	vector<Solution*> So;
	So = { &s0,&s1,&s2,&s3,&s4,&s5/*,&s6,&s7,&s8,&s9*/,&sf0,&sf1/*,&sf2,&sf3*/ };




	for (int i = 0; i < PN; i++)
	{
		So[i]->ejection_insertion();//产生初始解，fail就退出
		So[i]->checkR(0, 1);//不能注释 Update path information 
	}
	for (int i = 0; i < CN; i++)/////////////////////////////////////////////////////////
	{
		So[i + PN]->copy(*So[i], So[i + PN]->snum);
	}




	int crossk = 0;//交叉代数
	crossk = popul_cross(So, s0,CN,PN);
	//s0.ejection_insertion(s0.PARA.ite_num[s0.PARA.i]);


#ifdef WRITE
	g_d->each_problem_output << "\nR.size:" + to_string(s0.pbest->R.size()) + "\n";
#endif
	ret.vehicle_num = static_cast<int>(s0.pbest->R.size());

#ifdef WRITE
	int rc = 0;
	g_d->each_problem_output << "\n";
	s0.pbest->checkR(0, 1);
	for (auto it = s0.pbest->R.begin(); it != s0.pbest->R.end(); it++)
	{
		rc++;
		g_d->each_problem_output << to_string(rc) + "\t\t";
		for (auto iit = next((it)->path.begin()); iit != prev((it)->path.end()); iit++)
		{
			g_d->each_problem_output << to_string((*iit).pcus->cusid) + "\t";
		}
		g_d->each_problem_output << "\n";
	}g_d->each_problem_output << "\n";
	g_d->each_problem_output << fileinfo::get_topofilepath(fname) << "\n";
	g_d->each_problem_output << "total_cost " + to_string(s0.pbest->total_dist) + "\n\n";
#endif

	//pickup delivery 检查

#ifdef WRITE
	g_d->each_problem_output.close();
#endif
	cltw_writedata <<
		setw(4) << setiosflags(ios::left) << s0.pbest->R.size() <<

		setw(10) << setprecision(2) << setiosflags(ios::fixed | ios::left) << s0.pbest->minTime / 1000.0 <<
		setw(15) << setprecision(1) << setiosflags(ios::fixed | ios::left) << s0.pbest->hashtable.size() <<
		setw(10) << setprecision(1) << setiosflags(ios::fixed | ios::left) << s0.pbest->total_cost <<
		setw(10) << setprecision(1) << setiosflags(ios::fixed | ios::left) << s0.pbest->total_dist <<
		setw(10) << setprecision(2) << setiosflags(ios::fixed | ios::left) << s0.pbest->total_time / 1000.0 <<
		setw(4) << setiosflags(ios::left) << crossk <<
		setw(10) << setprecision(2) << setiosflags(ios::fixed | ios::left) << s0.pbest->removebad <<
		setw(10) << setprecision(2) << setiosflags(ios::fixed | ios::left) << s0.pbest->removerand;

#ifdef PRINTF
	printf("%s %d %.1lf %.1f %d\n",
		fileinfo::get_topofilepath(fname).c_str(), s0.pbest->R.size(), s0.pbest->total_cost, s0.pbest->total_dist, int(s0.pbest->total_time / 1000));
#endif
	pret->str_outp = cltw_writedata.str();
	delete g_d;
}




int popul_cross(vector<Solution*> &S, Solution& s0, int CN, int PN)
{
	

	//PN 初始化种群个数
	 //CN 参加交叉的
	double btime = s0.g_d->begin_time;

	vector<pair<pair<int, double>, int>> scor;
	scor.reserve(S.size());

	int snumber = S.size();
	int quitflag = INT_MAX;
	vector<int> ar;
	ar.reserve(PN);
	for (int i = 0; i < PN; i++)
	{
		ar.push_back(i);
	}
	int minRs = s0.R.size();
	int sametime = 0;
	int k = 0;
	for (; k < s0.PARA.ite_num[s0.PARA.i]; k++)
	{//GES+perturb+select  //测试+变异+选择
		cout << k << endl;
		s0.g_d->each_problem_output << "cross iterater" << k << endl;
		if (util::nowtime() - btime > s0.PARA.TIME[s0.PARA.i] * 1000)
		{
			break;
		}


		//先把父代复制一份

		for (int i = 0; i < CN; i++)//8 to 2 复制前四个
		{
			S[i + PN]->copy(*S[i], S[i + PN]->snum);
			S[i]->order_perturb(100);
		}
		random_shuffle(ar.begin(), ar.end());
		for (int i = 0; i <= CN / 2; i = i + 2)
		{//交叉
		 //交叉概率
			if (rand() / (double)RAND_MAX < (s0).g_d->pcross)
			{
				CROSS_large(*S[ar[i]], *S[ar[i + 1]]);
			}
		}





		//从S Sf中选择最好的四个。

		scor.clear();
		for (int i = 0; i < snumber; i++)
		{
			scor.emplace_back(make_pair(make_pair(int(S[i]->R.size()), S[i]->total_cost), i));
		}
		sort(scor.begin(), scor.end());


		vector<int> ap;
		vector<int> bp;
		ap.reserve(PN);
		bp.reserve(PN);
		for (int i = 0; i < PN; i++)
		{
			if (scor[i].second >= PN)
			{
				ap.push_back(scor[i].second);
			}
		}
		for (int i = PN; i < snumber; i++)
		{
			if (scor[i].second < PN)
			{
				bp.push_back(scor[i].second);
			}
		}


		s0.g_d->each_problem_output << "ab" << endl;
		for (int i = 0; i < ap.size(); i++)
		{
			cout << ap[i] << "\t" << bp[i] << endl;
			s0.g_d->each_problem_output << "ab" << ap[i] << "\t" << bp[i] << endl;
			Solution* temp; //int tmp;
			temp = S[ap[i]]; //tmp = S[ap[i]]->snum;
			S[ap[i]] = S[bp[i]]; //S[ap[i]]->snum = S[bp[i]]->snum;
			S[bp[i]] = temp; //S[bp[i]]->snum = tmp;		
		}

		//最好的替换最差的 并且perturb

		scor.clear();
		for (int i = 0; i < snumber; i++)
		{
			scor.emplace_back(make_pair(make_pair(int(S[i]->R.size()), S[i]->total_cost), i));
		}
		sort(scor.begin(), scor.end());

		//GES local search概率
		for (int i = 0; i < PN; i++)//2*PN
		{
			if (rand() / (double)RAND_MAX < (s0).g_d->plocal)
			{
				S[i]->ejection_insertion((int)0.1*S[i]->R.size());

				S[i]->checkR(0, 1);//不能注释
			}
		}

		// 		int rr=PN-1;
		//  	    S[scor[rr].second]->copy(*S[scor.begin()->second], S[scor[rr].second]->snum);
		// 		S[scor[rr].second]->order_perturb(100);


	}

	//Sobest 与 S[0]->minTime

	vector<pair<pair<pair<int, double>, double>, int>> sb;
	sb.reserve(2);
	sb.clear();

	sb.emplace_back(make_pair(make_pair(make_pair(s0.pbest->R.size(), s0.pbest->total_dist), s0.pbest->total_time), 0));
	sb.emplace_back(make_pair(make_pair(make_pair(s0.R.size(), s0.total_dist), s0.total_time), 1));
	sort(sb.begin(), sb.end());
	if (sb.begin()->second == 1)//S[0]更好
	{
		//s0.pbest->copy(s0, s0.pbest->snum);
		s0.pbest->R = s0.R;
		s0.pbest->total_time = s0.total_time;
		s0.pbest->total_cost = s0.total_cost;
		s0.pbest->total_dist = s0.total_dist;
	}

	return k;

}
const string fileinfo::topo_rootpath = "data1/";
string fileinfo::result_root_path;
const string fileinfo::toposize[10] = { "pdp100","pdp200","pdp400",
"pdp600","pdp800","pdp1000","pdp1200","pdp1400","pdp1600","pdp1800" };
const int fileinfo::i_toposize[10] = { 0,2,4,6,8,10,12,14,16,18 };
const string fileinfo::cluster_name[4] = { "", "lc","lr","lrc" };
const string fileinfo::tw_name[3] = { "", "1","2" };
