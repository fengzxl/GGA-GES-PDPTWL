#pragma once

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include<utility>
#include <regex>
typedef class fileinfo
{
public:
	using str = std::string;
	using rettype = std::vector<std::vector<str>>;
	fileinfo()
	{

	}
	rettype reg_read(str size, int type1, int type2) {
		for (int j = 0; j < 6; ++j) {
			if (toposize[j] == size)
				return reg_read(j, type1, type2);
		}
		std::cerr << "error\n";
		return std::vector<std::vector<str>>();
	}
	//0-6 1-3 1-2
	rettype reg_read(int size, int type1, int type2) {
		std::vector<std::pair<str, int>> result;
		str reg;
		str s_tpsize = toposize[size], s_cl_name = cluster_name[type1], s_tw_name = tw_name[type2];
		auto allpath = visit(topo_rootpath + s_tpsize);
		if (s_tpsize == "pdp100") {
			reg = s_cl_name + s_tw_name + "0" + R"(([1-9]\d*).txt)";
		}
		else {
			std::transform(s_cl_name.begin(), s_cl_name.end(), s_cl_name.begin(), ::toupper);
			reg = s_cl_name + s_tw_name + "_" + std::to_string(i_toposize[size]) + R"(_([1-9]\d*).txt)";
		}
		for (auto &it : allpath)
		{
			std::smatch m;
			if (std::regex_search(it, m, std::regex(reg)))
			{
				//std::regex_match(it, m, std::regex(reg));
				result.push_back(make_pair(it, stol(m[1], nullptr, 10)));

			}
		}
		std::sort(result.begin(), result.end(),
			[](const std::pair<str, int> &a1, const std::pair<str, int> &a2) {
			return a1.second < a2.second;
		});
		rettype rsl{ std::vector<str>{ str("") } };
		for (auto &it : result) {
			std::vector<str> tmp;
			tmp.emplace_back(topo_rootpath + s_tpsize + "/" + it.first); //topofilepath
			tmp.emplace_back(it.first);//filename
			tmp.emplace_back(s_cl_name + s_tw_name +
				"_" + std::to_string(i_toposize[size])+
				"_" + std::to_string(it.second) + ".txt");  //each result file path
			tmp.emplace_back(s_cl_name + s_tw_name + ".txt");  //resultfilepath
			rsl.emplace_back(tmp);
		}
		return rsl;
	}
	static str get_topofilepath(const std::vector<str>& vec) {
		return vec[0];
	}
	static str get_filename(const std::vector<str>& vec) {
		return vec[1];
	}
	static str get_each_resultfilepath(const std::vector<str>& vec,int run=999) {

		str pth = result_root_path + "run" + std::to_string(run);
#ifdef _WIN32
		_finddata_t filefind;
		auto handle = _findfirst(pth.c_str(), &filefind);
		if (handle == -1) {
			printf("create new folder\n");
			_findclose(handle);
			system(str("md \"" + pth + "\"").c_str());
		}
		else {
			//printf("exist\n");
		}
#else
		DIR *dfd;
		//dirent *dp;
		if ((dfd = opendir(pth.c_str())) == nullptr) {//fail
//			printf("can't open %s file\n", path.c_str());
//			return vs;
			printf("create new folder\n");

			closedir(dfd);
			system(str("mkdir -p \"" + pth + "\"").c_str());
		}
		else{
		}
#endif
		pth += "/" + vec[2];
		return pth;
	}
	static str get_cltw_resultfilepath(const std::vector<str>& vec,int run=999) {
		str pth = result_root_path + "run" + std::to_string(run);
#ifdef _WIN32
		_finddata_t filefind;
		auto handle = _findfirst(pth.c_str(), &filefind);
		if (handle == -1) {
			printf("create new folder\n");
			_findclose(handle);
			system(str("md \"" + pth + "\"").c_str());
		}
		else {
			//printf("exist\n");
		}
#else
		DIR *dfd;
		//dirent *dp;
		if ((dfd = opendir(pth.c_str())) == nullptr) {//fail
//			printf("can't open %s file\n", path.c_str());
//			return vs;
			printf("create new folder\n");
			system(str("mkdir -p \"" + pth + "\"").c_str());
			closedir(dfd);
			
		}
		else{
		}
#endif
		pth += "/" + vec[3];
		return pth;
	}
	static str get_cluster(int id) {
		return cluster_name[id];
	}
	static str get_tw(int id) {
		return tw_name[id];
	}
#ifdef _WIN32
	std::vector<str> visit(str path) {
		if (path.back() != '/')path.append("/");
		path.append("*");
		std::vector<str> vs;
		_finddata_t filefind;
		int done = 0;
		auto handle = _findfirst(path.c_str(), &filefind);
		if (handle == -1)return vs;
		while (!(done = _findnext(handle, &filefind)))
		{
			if (!strcmp(filefind.name, ".."))continue;
			//if ((_A_SUBDIR == filefind.attrib)) {
			vs.emplace_back(filefind.name);
			//}
		}
		_findclose(handle);
		std::sort(vs.begin(), vs.end());
		return vs;
	}
#else
	std::vector<str> visit(str path) {
		if (path.back() != '/')path.append("/");
		//path.append("*");
		std::vector<str> vs;
		DIR *dfd;
		dirent *dp;
		if ((dfd = opendir(path.c_str())) == nullptr) {
			printf("can't open %s file\n", path.c_str());
			return vs;
		}
		while ((dp = readdir(dfd)) != nullptr) {
			if (strncmp(dp->d_name, ".", 1) == 0)continue;
			//if (dp->d_type == 4) {
				vs.emplace_back(str(dp->d_name));
			//}
		}
		closedir(dfd);
		std::sort(vs.begin(), vs.end());
		return vs;
	}
#endif
public:
	static const str topo_rootpath;
	static str result_root_path;
	static const str toposize[10];
	static const int i_toposize[10];
	static const str cluster_name[4];
	static const str tw_name[3];
}cfile;
