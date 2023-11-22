#ifndef HOMEAUTOMATION_H_
#define HOMEAUTOMATION_H_

#include "Appliance.h"
#include "Menu.h"
#include "SMSmanager.h"
#include <sstream>

#define FILE_INFO "./appInfo.txt"
#define MAX_INT 100'000 // 사용자 입력 시 INT MAX값
#define MAX_LENGTH 10   // 문자열 입력 최대 길이

using appList_t = std::vector<std::vector<Appliance*>>;

class HomeAutomation {
private:
	appList_t appList;
public:
	HomeAutomation();
	~HomeAutomation();
	void init();
	void add(); 
	void ctrl();
	void del();
	void ctrlPower(const int& idx, const int& num);
	void changeName(const int& idx, const int& num);
	void setState(const int& idx, const int& num, const int& sel);
	void displayAllInfo();
	void displayTypeList();
	void displayAppList(const int& idx);
	const int getTypeIdx(const int& type);
	const bool isEmpty();
	const bool getIdx(int& idx, int& num);
	static const bool inputNum(int& num, const int& comp, const std::string& str, const voidHandler_t display = nullptr);
	static const bool checkRange(const int& num, const int& comp);
	static const bool inputName(std::string& appName, const appList_t& appList, const int& typeIdx, const voidHandler_t display = nullptr);
	static const bool checkNameDup(const std::string& name, const appList_t& appList, const int& typeIdx);
	static const bool bufCheck();
};

#endif

