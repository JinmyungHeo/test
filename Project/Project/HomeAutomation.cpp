#include "HomeAutomation.h"

// Factory, SMSmanager Class SingleTone 
Factory& factory = Factory::getInstance();
SMSmanager& sms = SMSmanager::getInstance();
// 삭제된 기기 전력량 저장
double Appliance::power = 0.0;

// 생성 시 기 저장된 상태 정보 Load 후 저장
HomeAutomation::HomeAutomation() { 
	FILE* fp;
	Appliance* app;
	std::string appName;
	char info[256];
	int typeIdx;
	int type, powerConsumption, stOneKey, stOneValue, stTwoKey, stTwoValue;
	long long startHour, useHour;
	double power;
	bool powerFlag;
	fp = fopen(FILE_INFO, "rt");
	if (fp != nullptr) {
		fgets(info, sizeof(info), fp);
		power = std::stod(info);
		Appliance::setPower(power);
		while (fgets(info, sizeof(info), fp) != nullptr) {
			std::stringstream str(static_cast<std::string>(info));
			str >> type >> powerConsumption >> powerFlag >> startHour >> useHour
				>> stOneKey >> stOneValue >> stTwoKey >> stTwoValue >> appName;
			typeIdx = getTypeIdx(type);
			app = factory.create(type);
			app->setName(appName);
			app->setPowerConsumption(powerConsumption);
			app->setPowerFlag(powerFlag);
			app->setTime(startHour, useHour);
			if (stOneKey != 0) { app->setState(stOneKey, stOneValue); }
			if (stTwoKey != 0) { app->setState(stTwoKey, stTwoValue); }
			if (typeIdx == -1) { appList.push_back({ app }); }
			else { appList[typeIdx].push_back(app); }
		}
		fclose(fp);
	}
	init();
}
// 소멸 시 상태 정보 저장 (삭제 기기 전력량, 현 상태 등)
HomeAutomation::~HomeAutomation() { 
	FILE* fp;
	Appliance* app;
	std::string info;
	fp = fopen(FILE_INFO, "wt");
	if (fp != nullptr) {
		info = std::to_string(Appliance::getPower()) + '\n';
		fputs(info.c_str(), fp);
		for (int i = 0; i < appList.size(); i++) {
			for (int j = 0; j < appList[i].size(); j++) {
				app = appList[i][j];
				info = app->getInfo() + '\n';
				fputs(info.c_str(), fp);
				delete app;
			}
		}
		fclose(fp);
	}
}
// 메뉴 초기화
void HomeAutomation::init() {
	std::shared_ptr<PopupMenu> root	   (new PopupMenu("메인 메뉴"));
	std::shared_ptr<PopupMenu> mCtrl   (new PopupMenu("기기 제어"));
	std::shared_ptr<PopupMenu> mNum    (new PopupMenu("전화 번호 관리"));
	std::shared_ptr<BaseMenu>  mAdd    (new MenuItem("기기 등록", std::bind(&HomeAutomation::add, this)));
	std::shared_ptr<BaseMenu>  mDp     (new MenuItem("기기 조회", std::bind(&HomeAutomation::displayAllInfo, this)));
	std::shared_ptr<BaseMenu>  mCtrlT  (new MenuItem("전체 제어", std::bind(&HomeAutomation::ctrlPower, this, -1, 0)));
	std::shared_ptr<BaseMenu>  mCtrlI  (new MenuItem("개별 제어", std::bind(&HomeAutomation::ctrl, this)));
	std::shared_ptr<BaseMenu>  mDel    (new MenuItem("기기 삭제", std::bind(&HomeAutomation::del, this)));
	std::shared_ptr<BaseMenu>  mNumAdd (new MenuItem("전화 번호 등록", std::bind(&SMSmanager::addNumber, &sms)));
	std::shared_ptr<BaseMenu>  mNumDel (new MenuItem("전화 번호 삭제", std::bind(&SMSmanager::delNumber, &sms)));	
	mCtrl->add(mCtrlT);
	mCtrl->add(mCtrlI);
	mNum->add(mNumAdd);
	mNum->add(mNumDel);
	root->add(mAdd);
	root->add(mDp);
	root->add(mCtrl);
	root->add(mDel);
	root->add(mNum);
	root->command();
}
// 기기 추가
void HomeAutomation::add() {
	Appliance* app = nullptr;
	std::string appName;
	int type;
	int powerConsumption;
	int typeIdx;
	if (inputNum(type, factory.getSize(), "타입 번호", std::bind(&Factory::showType, &factory))) { return; }
	typeIdx = getTypeIdx(type); // 등록된 기기 타입인지 확인, 아닐 시 -1
	app = factory.create(type); // 신규 기기 생성
	if (app == nullptr) {
		std::cout << "등록 실패! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return;
	}
	if (inputName(appName, appList, typeIdx)) { return; }
	if (inputNum(powerConsumption, MAX_INT, "소비 전력")) { return; }
	app->setName(appName);
	app->setPowerConsumption(powerConsumption);
	if (typeIdx == -1) { appList.push_back({ app }); }
	else { appList[typeIdx].push_back(app); }
	app = appList[getTypeIdx(type)][0];
	sms.setName(app->getTypeName(), appName); // CallBack 위한 타입명, 기기명 저장
	sms.sendMsg("등록");
	std::cout << "등록 성공! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 기기 제어 (개별 제어)
void HomeAutomation::ctrl() {
	if (isEmpty()) return;
	Appliance* app;
	int typeIdx;
	int appIdx;
	if (getIdx(typeIdx, appIdx)) return; // 제어할 기기 타입 및 기기 Idx 정보 입력
	app = appList[typeIdx][appIdx];
	// 파생 메뉴 등록
	std::shared_ptr<PopupMenu> mCtrlPopup (new PopupMenu("개별 제어"));
	std::shared_ptr<BaseMenu>  mCtrlName  (new MenuItem ("이름 변경",  std::bind(&HomeAutomation::changeName, this, typeIdx, appIdx)));
	std::shared_ptr<BaseMenu>  mCtrlPower (new MenuItem ("전원 제어",  std::bind(&HomeAutomation::ctrlPower,  this, typeIdx, appIdx)));
	std::shared_ptr<BaseMenu>  mCtrlStOne (new MenuItem ("상태1 제어", std::bind(&HomeAutomation::setState,   this, typeIdx, appIdx, 1)));
	std::shared_ptr<BaseMenu>  mCtrlStTwo (new MenuItem ("상태2 제어", std::bind(&HomeAutomation::setState,   this, typeIdx, appIdx, 2)));
	mCtrlPopup->add(mCtrlName);
	mCtrlPopup->add(mCtrlPower);
	mCtrlPopup->add(mCtrlStOne);
	mCtrlPopup->add(mCtrlStTwo);
	mCtrlPopup->command();
}
// 기기 삭제
void HomeAutomation::del() {
	if (isEmpty()) return;
	Appliance* app;
	int typeIdx;
	int appIdx;
	if (getIdx(typeIdx, appIdx)) return;
	app = appList[typeIdx][appIdx];
	Appliance::setPower(app->powerCalc());
	sms.setName(app->getTypeName(), app->getName());
	delete app;
	appList[typeIdx].erase(appList[typeIdx].begin() + appIdx);
	if (appList[typeIdx].size() == 0) appList.erase(appList.begin() + typeIdx); // 기기 타입이 비어있다면, 기기 타입 삭제
	sms.sendMsg("삭제");
	std::cout << "삭제 성공! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 전원 제어 (전체 제어)
void HomeAutomation::ctrlPower(const int& typeIdx, const int& appIdx) {
	if (isEmpty()) return;
	Appliance* app = nullptr;
	int powerFlag;
	if (inputNum(powerFlag, 2, "전원 제어 번호", []() { std::cout << "1. ON\n2. OFF" << std::endl; })) { return; }
	powerFlag %= 2;
	for (int i = 0; i < appList.size(); i++) {
		for (int j = 0; j < appList[i].size(); j++) {
			if (typeIdx == -1 /* 전체 제어 메뉴 */ || (typeIdx == i && appIdx == j) /* 특정 기기 제어 */ ) {
				app = appList[i][j];
				app->setPowerFlag(powerFlag);
			}
		}
	}
	sms.setName("전체 타입", "전체 기기"); 
	if (typeIdx != -1) sms.setName(app->getTypeName(), app->getName());
	sms.sendMsg("제어");
	std::cout << "제어 성공! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 기기명 변경
void HomeAutomation::changeName(const int& typeIdx, const int& appIdx) {
	Appliance* app;
	std::string newAppName;
	if (inputName(newAppName, appList, typeIdx)) { return; }
	app = appList[typeIdx][appIdx];
	app->setName(newAppName);
	std::cout << "변경 성공! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 기기 상태 변경
void HomeAutomation::setState(const int& typeIdx, const int& appIdx, const int& sel) {
	Appliance* app = appList[typeIdx][appIdx];
	int st;
	if (app->getStateSize() < sel) {    // 해당 기기 State 제어 기능 없으면
		std::cout<< "제어 기능 없음! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return;
	}
	if (app->getPowerFlag() == false) { // 해당 기기 전원 꺼져 있으면
		std::cout << "전원 ON 필요! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return;
	}
	while (true) {
		if (inputNum(st, MAX_INT, "제어할 상태 번호", std::bind(&Appliance::showStateList, app, sel))) { return; }
		if (Appliance::checkStateRange(app, sel, st)) continue; // 입력 범위 검사
		app->setState(sel, st);
		break;
	}
	sms.setName(app->getTypeName(), app->getName());
	sms.sendMsg("제어");
	std::cout << "제어 성공! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 기기 조회
void HomeAutomation::displayAllInfo() {
	if (isEmpty()) return;
	Appliance* app;
	double power = 0;
	system("cls");
	for (int i = 0; i < appList.size(); i++) {
		app = appList[i][0];
		std::cout << app->getTypeName() << std::endl;
		power += app->powerCalc(); // 각 기기 별 현재까지 사용된 전력량 합산
		displayAppList(i);         // 각 타입 -> 기기 별 정보 출력
		std::cout << std::endl;
	}
	std::cout << std::fixed;
	std::cout.precision(2);
	std::cout << "사용 전력량 : " << power << " (삭제 기기 사용 전력량 : " << Appliance::getPower() << ")" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "조회 완료! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 등록된 타입 정보 출력
void HomeAutomation::displayTypeList() {
	Appliance* app;
	for (int i = 0; i < appList.size(); i++) {
		app = appList[i][0];
		std::cout << i + 1 << ". " << app->getTypeName() << std::endl;
	}
}
// 등록된 기기 별 정보 출력
void HomeAutomation::displayAppList(const int& typeIdx) {
	Appliance* app;
	for (int i = 0; i < appList[typeIdx].size(); i++) {
		app = appList[typeIdx][i];
		std::cout << i + 1 << ". ";
		app->display();
		std::cout << std::endl;
	}
}
// 해당 타입 번호의 등록된 Idx 탐색, 미 등록된 타입 일 시 -1 리턴
const int HomeAutomation::getTypeIdx(const int& type) {
	for (int i = 0; i < appList.size(); i++) {
		if (appList[i][0]->getType() == type) return i;
	}
	return -1;
}
const bool HomeAutomation::isEmpty() {
	if (appList.size() == 0) {
		std::cout << "기기 없음! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return true;
	}
	return false;
}
// 타입 및 기기 선택 후
const bool HomeAutomation::getIdx(int& typeIdx, int& appIdx) {
	if (inputNum(typeIdx, appList.size(), "타입 번호", std::bind(&HomeAutomation::displayTypeList, this))) { return true; }
	typeIdx--;
	if (inputNum(appIdx, appList[typeIdx].size(), "제품 번호", std::bind(&HomeAutomation::displayAppList, this, typeIdx))) { return true; }
	appIdx--;
	return false;
}
// int형 정보 사용자로부터 입력
const bool HomeAutomation::inputNum(int& num, const int& comp, const std::string& str, const voidHandler_t display) {
	while (true) {
		system("cls");
		if (display != nullptr) display();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << str << "를 입력하세요 (종료 : 0) >> ";
		std::cin >> num;
		if (bufCheck()) continue;
		if (num == 0) return true;
		if (checkRange(num, comp)) continue;
		return false;
	}
}
// 입력 범위 검사
const bool HomeAutomation::checkRange(const int& num, const int& comp) {
	if (num < 0 || num > comp) {
		std::cout << "입력 범위 초과! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return true;
	}
	return false;
}
// 기기명 사용자로부터 입력 
const bool HomeAutomation::inputName(std::string& appName, const appList_t& appList, const int& typeIdx, const voidHandler_t display) {
	while (true) {
		system("cls");
		if (display != nullptr) display();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "기기명을 입력하세요 (종료 : 0) >> ";
		std::cin >> appName;
		if (bufCheck()) continue;
		if (appName.length() == 1 && appName[0] == '0') return true;
		if (checkRange(appName.length(), MAX_LENGTH)) continue;
		if (typeIdx != -1 && (checkNameDup(appName, appList, typeIdx))) continue;
		return false;
	}
}
// 기기명 중복 검사
const bool HomeAutomation::checkNameDup(const std::string& name, const appList_t& appList, const int& typeIdx) {
	Appliance* app;
	for (int i = 0; i < appList[typeIdx].size(); i++) {
		app = appList[typeIdx][i];
		if (name == app->getName()) {
			std::cout << "기기명 중복! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
			getch();
			return true;
		}
	}
	return false;
}
const bool HomeAutomation::bufCheck() {
	if (std::cin.get() != '\n') {
		std::cin.clear();
		while (std::cin.get() != '\n') { ; }
		std::cout << "잘못된 입력! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return true;
	}
	return false;
}
