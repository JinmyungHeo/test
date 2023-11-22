#include "Appliance.h"

Appliance::Appliance(const int& t, const std::string& tn) 
	: type(t), typeName(tn), powerFlag(0), powerConsumption(0), startHour(0), useHour(0) { ; }
Appliance::~Appliance() { ; }
void Appliance::setName(const std::string& name) { this->name = name; }
void Appliance::setPowerConsumption(const int& powerConsumption) { this->powerConsumption = powerConsumption; }
void Appliance::setState(const int& sel, const int& st) { stateList[sel] = st; }
void Appliance::setPowerFlag(const bool& powerFlag) {
	if (this->powerFlag == powerFlag) return;
	this->powerFlag = powerFlag;
	time_t now;
	switch (this->powerFlag) {
	// Off 시 사용된 시간 저장 및 State Default로 초기화
	case false: { useHour = time(&now) - startHour; setStateDefault(); break; } 
	case true: { startHour = time(&now); break; }
	}
}
void Appliance::setTime(const long long& startHour, const long long& useHour) {
	this->startHour = startHour;
	this->useHour = useHour;
}
// 제품 정보 출력 
void Appliance::display() {
	std::cout << std::fixed;
	std::cout.precision(2);
	std::cout << "[ 이름 : " << std::setw(9) << name << " ]  [ 전원 상태 : " << std::setw(3) << (powerFlag ? "ON" : "OFF") << " ]  [ 전력 소비량 : " << std::setw(7) << powerCalc() << " ]";
	if (powerFlag == false) return;
	else showState();
}
// 제품 정보 저장 위한 Info String 형태 변경 함수
const std::string Appliance::getInfo() {
	std::string type = std::to_string(this->type);
	std::string powerConsumption = std::to_string(this->powerConsumption);
	std::string powerFlag = std::to_string(this->powerFlag);
	std::string startHour = std::to_string(this->startHour);
	std::string useHour = std::to_string(this->useHour);
	std::string stOne = "1 ";
	std::string stTwo = "2 ";
	if (stateList.find(1) != stateList.end()) { stOne += std::to_string(stateList[1]); }
	else { stOne = "0 0"; }
	if (stateList.find(2) != stateList.end()) { stTwo += std::to_string(stateList[2]); }
	else { stTwo = "0 0"; }
	std::string info = type + " " + powerConsumption + " " + powerFlag + " " + startHour + " " + useHour + " " + stOne + " " + stTwo + " " + name;
	return info;
}
const std::string& Appliance::getName() { return name; }
const std::string& Appliance::getTypeName() { return typeName; }
const int Appliance::getStateSize() { return stateList.size(); }
const int& Appliance::getType() { return type; }
const int& Appliance::getState(const int& sel) { return stateList[sel]; }
const bool& Appliance::getPowerFlag() { return powerFlag; }
// 조회 시점의 전력 사용량 계산
const double Appliance::powerCalc() {
	double tempPower;
	time_t now;
	time_t useHour = this->useHour;
	if (powerFlag == true) {
		useHour += time(&now) - startHour;
	}
	tempPower = useHour / 3600.0 * powerConsumption;
	return tempPower;
}
// State 표시 
// State1 : 정해진 State 문자열 표기
// State2 : MIN ~ MAX 범위 표기
void Appliance::showStateList(Appliance* app, const int& sel) {
	pii range = app->getStateRange(sel);
	stList_t stList = app->getStList();
	if (stList.size() - 1 == range.second) {
		for (int i = 1; i < stList.size(); i++) {
			std::cout << i << ". " << stList[i] << std::endl;
		}
	}
	else { std::cout << "MIN : " << range.first << " ~ " << "MAX : " << range.second << std::endl; }
}
// 기기 삭제 시 static int power 변수에 전력 사용량 저장
void Appliance::setPower(const double& power) { Appliance::power += power; }
// 사용자로 부터 입력받은 State 값 범위 검사 
const bool Appliance::checkStateRange(Appliance* app, const int& sel, const int& st) {
	pii range = app->getStateRange(sel);
	if (st < range.first || st > range.second) return true;
	return false;
}
const double& Appliance::getPower() { return power; }

// Factory 클래스 내 타입 정보 저장
void Factory::regType(const int& type, const std::string& typeName, creator_t creator) {
	if (creatorList.find(type) == creatorList.end()) {
		creatorList[type] = make_pair(typeName, creator);
	}
}
// Factory 클래스 내 저장된 타입 정보 출력
void Factory::showType() {
	int i = 0;
	for (auto it = creatorList.begin(); it != creatorList.end(); it++) {
		std::cout << ++i << ". " << it->second.first << std::endl;
	}
}
// Factory 클래스 내 저장된 타입 개수 
const int Factory::getSize() { return creatorList.size(); }
Factory& Factory::getInstance() {
	static Factory factory;
	return factory;
}
Appliance* Factory::create(const int& type) {
	Appliance* app = nullptr;
	creator_t func = creatorList[type].second;
	if (func != nullptr) {
		app = func();
	}
	return app;
}

Register::Register(const int& type, const std::string& typeName, creator_t func) {
	Factory& factory = Factory::getInstance();
	factory.regType(type, typeName, func);
}

AirCon::AirCon(const int& t, const std::string& tn) : Appliance(t, tn) { setStateDefault(); }
AirCon::~AirCon() { ; }
// 제품 상태 정보 출력
void AirCon::showState() { std::cout << "  [ 상태1 : " << std::setw(8) << stList[getState(1)] << " ]  [ 상태2 : " << std::setw(2) << getState(2) << "도 ]"; }
// 제품 상태 기본값 세팅
void AirCon::setStateDefault() { setState(1, SENDAIR); setState(2, DEF_AC); }
const stList_t& AirCon::getStList() { return stList; }
const pii& AirCon::getStateRange(const int& sel) { return stateRange[sel]; }
Appliance* AirCon::create() {
	Appliance* p = new AirCon (1, "에어컨");
	return p;
}
Register AirCon::AutoRegister (1, "에어컨", AirCon::create);

Washer::Washer(const int& t, const std::string& tn) : Appliance(t, tn) { setStateDefault(); }
Washer::~Washer() { ; }
void Washer::showState() { std::cout << "  [ 상태1 : " << std::setw(8) << stList[getState(1)] << " ]"; }
void Washer::setStateDefault() { setState(1, NO_OPERATION); }
const stList_t& Washer::getStList() { return stList; }
const pii& Washer::getStateRange(const int& sel) { return stateRange[sel]; }
Appliance* Washer::create() {
	Appliance* p = new Washer(2, "세탁기");
	return p;
}
Register Washer::AutoRegister(2, "세탁기", Washer::create);

RiceCooker::RiceCooker(const int& t, const std::string& tn) : Appliance(t, tn) { setStateDefault(); }
RiceCooker::~RiceCooker() { ; }
void RiceCooker::showState() { std::cout << "  [ 상태1 : " << std::setw(8) << stList[getState(1)] << " ]"; }
void RiceCooker::setStateDefault() { setState(1, NO_OPERATION); }
const stList_t& RiceCooker::getStList() { return stList; }
const pii& RiceCooker::getStateRange(const int& sel) { return stateRange[sel]; }
Appliance* RiceCooker::create() {
	Appliance* p = new Washer(3, "밥솥");
	return p;
}
Register RiceCooker::AutoRegister(3, "밥솥", RiceCooker::create);

Light::Light(const int& t, const std::string& tn) : Appliance(t, tn) { ; }
Light::~Light() { ; }
void Light::showState() { ; }
void Light::setStateDefault() { ; }
const stList_t& Light::getStList() { return {}; }
const pii& Light::getStateRange(const int& sel) { return {}; }
Appliance* Light::create() {
	Appliance* p = new Light(4, "전등");
	return p;
}
Register Light::AutoRegister(4, "전등", Light::create);