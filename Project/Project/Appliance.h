#ifndef APPLIANCE_H_
#define APPLIANCE_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
#include <vector>
#include <map>

// 에어컨 설정 온도
#define MIN_AC 18 
#define MAX_AC 30 
#define DEF_AC 24 

using pii = std::pair<int, int>;
using stList_t = std::vector<std::string>;

// 각 가전 기기 기반 클래스
class Appliance {
private:
	std::string name;
	std::string typeName;
	int type;
	int powerConsumption;
	bool powerFlag;
	static double power;
	std::map<int, int> stateList;
	time_t startHour;
	time_t useHour;
public:
	Appliance(const int& t, const std::string& tn);
	virtual ~Appliance();
	virtual void showState() = 0;
	virtual void setStateDefault() = 0;
	virtual const stList_t& getStList() = 0;
	virtual const pii& getStateRange(const int& sel) = 0;
	void setName(const std::string& name);
	void setPowerConsumption(const int& powerConsumption);
	void setState(const int& sel, const int& st);
	void setPowerFlag(const bool& powerFlag);
	void setTime(const long long& startHour, const long long& useHour);
	void display();
	const std::string getInfo();
	const std::string& getName();
	const std::string& getTypeName();
	const int getStateSize();
	const int& getType();
	const int& getState(const int& sel);
	const bool& getPowerFlag();
	const double powerCalc();
	static void showStateList(Appliance* app, const int& sel);
	static void setPower(const double& power);
	static const bool checkStateRange(Appliance* app, const int& sel, const int& st);
	static const double& getPower();
};

using creator_t = Appliance*(*)();

// 가전 기기 등록 위한 Factory 클래스
class Factory {	
	std::map<int, std::pair<std::string, creator_t>> creatorList;
public:
	void regType(const int& type, const std::string& typeName, creator_t creator);
	void showType();
	const int getSize();
	static Factory& getInstance();
	Appliance* create(const int& type);
};

// 가전 기기 자동 등록 클래스
class Register {
public:
	Register(const int& type, const std::string& typeName, creator_t func);
};

// 에어컨 클래스
class AirCon : public Appliance {
private:
	enum acStat { SENDAIR = 1, COOLER, HEATER, COUNT }; // COUNT 개수 COUNT용
	stList_t stList = { "-", "송풍", "냉방", "난방" };
	std::map<int, pii> stateRange = { {1, {SENDAIR, COUNT-1}}, {2, {MIN_AC, MAX_AC}} };
public:
	AirCon(const int& t, const std::string& tn);
	~AirCon();
	void showState() override;
	void setStateDefault() override;
	const stList_t& getStList() override;
	const pii& getStateRange(const int& sel) override;
	static Appliance* create();
	static Register AutoRegister;
};

// 세탁기 클래스
class Washer : public Appliance {
private:
	enum wsStat { NO_OPERATION = 1, GENERAL, BOIL, DRY, COUNT };
	std::vector<std::string> stList = { "-", "무동작", "일반빨래", "삶은빨래", "건조"};
	std::map<int, pii> stateRange = { {1, {NO_OPERATION, COUNT - 1}} };
public:
	Washer(const int& t, const std::string& tn);
	~Washer();
	void showState() override;
	void setStateDefault() override;
	const stList_t& getStList() override;
	const pii& getStateRange(const int& sel) override;
	static Appliance* create();
	static Register AutoRegister;
};

// 밥솥 클래스
class RiceCooker : public Appliance {
private:
	enum rcStat { NO_OPERATION = 1, WARM, COOK, HEAT, COUNT };
	std::vector<std::string> stList = { "-", "무동작", "보온", "밥짓기", "데우기" };
	std::map<int, pii> stateRange = { {1, {NO_OPERATION, COUNT - 1}} };
public:
	RiceCooker(const int& t, const std::string& tn);
	~RiceCooker();
	void showState() override;
	void setStateDefault() override;
	const stList_t& getStList() override;
	const pii& getStateRange(const int& sel) override;
	static Register AutoRegister;
	static Appliance* create();
};

// 전등 클래스
class Light : public Appliance {
public:
	Light(const int& t, const std::string& tn);
	~Light();
	void showState() override;
	void setStateDefault() override;
	const stList_t& getStList() override;
	const pii& getStateRange(const int& sel) override;
	static Register AutoRegister;
	static Appliance* create();
};

#endif