#include "HomeAutomation.h"

// Factory, SMSmanager Class SingleTone 
Factory& factory = Factory::getInstance();
SMSmanager& sms = SMSmanager::getInstance();
// ������ ��� ���·� ����
double Appliance::power = 0.0;

// ���� �� �� ����� ���� ���� Load �� ����
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
// �Ҹ� �� ���� ���� ���� (���� ��� ���·�, �� ���� ��)
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
// �޴� �ʱ�ȭ
void HomeAutomation::init() {
	std::shared_ptr<PopupMenu> root	   (new PopupMenu("���� �޴�"));
	std::shared_ptr<PopupMenu> mCtrl   (new PopupMenu("��� ����"));
	std::shared_ptr<PopupMenu> mNum    (new PopupMenu("��ȭ ��ȣ ����"));
	std::shared_ptr<BaseMenu>  mAdd    (new MenuItem("��� ���", std::bind(&HomeAutomation::add, this)));
	std::shared_ptr<BaseMenu>  mDp     (new MenuItem("��� ��ȸ", std::bind(&HomeAutomation::displayAllInfo, this)));
	std::shared_ptr<BaseMenu>  mCtrlT  (new MenuItem("��ü ����", std::bind(&HomeAutomation::ctrlPower, this, -1, 0)));
	std::shared_ptr<BaseMenu>  mCtrlI  (new MenuItem("���� ����", std::bind(&HomeAutomation::ctrl, this)));
	std::shared_ptr<BaseMenu>  mDel    (new MenuItem("��� ����", std::bind(&HomeAutomation::del, this)));
	std::shared_ptr<BaseMenu>  mNumAdd (new MenuItem("��ȭ ��ȣ ���", std::bind(&SMSmanager::addNumber, &sms)));
	std::shared_ptr<BaseMenu>  mNumDel (new MenuItem("��ȭ ��ȣ ����", std::bind(&SMSmanager::delNumber, &sms)));	
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
// ��� �߰�
void HomeAutomation::add() {
	Appliance* app = nullptr;
	std::string appName;
	int type;
	int powerConsumption;
	int typeIdx;
	if (inputNum(type, factory.getSize(), "Ÿ�� ��ȣ", std::bind(&Factory::showType, &factory))) { return; }
	typeIdx = getTypeIdx(type); // ��ϵ� ��� Ÿ������ Ȯ��, �ƴ� �� -1
	app = factory.create(type); // �ű� ��� ����
	if (app == nullptr) {
		std::cout << "��� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return;
	}
	if (inputName(appName, appList, typeIdx)) { return; }
	if (inputNum(powerConsumption, MAX_INT, "�Һ� ����")) { return; }
	app->setName(appName);
	app->setPowerConsumption(powerConsumption);
	if (typeIdx == -1) { appList.push_back({ app }); }
	else { appList[typeIdx].push_back(app); }
	app = appList[getTypeIdx(type)][0];
	sms.setName(app->getTypeName(), appName); // CallBack ���� Ÿ�Ը�, ���� ����
	sms.sendMsg("���");
	std::cout << "��� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ��� ���� (���� ����)
void HomeAutomation::ctrl() {
	if (isEmpty()) return;
	Appliance* app;
	int typeIdx;
	int appIdx;
	if (getIdx(typeIdx, appIdx)) return; // ������ ��� Ÿ�� �� ��� Idx ���� �Է�
	app = appList[typeIdx][appIdx];
	// �Ļ� �޴� ���
	std::shared_ptr<PopupMenu> mCtrlPopup (new PopupMenu("���� ����"));
	std::shared_ptr<BaseMenu>  mCtrlName  (new MenuItem ("�̸� ����",  std::bind(&HomeAutomation::changeName, this, typeIdx, appIdx)));
	std::shared_ptr<BaseMenu>  mCtrlPower (new MenuItem ("���� ����",  std::bind(&HomeAutomation::ctrlPower,  this, typeIdx, appIdx)));
	std::shared_ptr<BaseMenu>  mCtrlStOne (new MenuItem ("����1 ����", std::bind(&HomeAutomation::setState,   this, typeIdx, appIdx, 1)));
	std::shared_ptr<BaseMenu>  mCtrlStTwo (new MenuItem ("����2 ����", std::bind(&HomeAutomation::setState,   this, typeIdx, appIdx, 2)));
	mCtrlPopup->add(mCtrlName);
	mCtrlPopup->add(mCtrlPower);
	mCtrlPopup->add(mCtrlStOne);
	mCtrlPopup->add(mCtrlStTwo);
	mCtrlPopup->command();
}
// ��� ����
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
	if (appList[typeIdx].size() == 0) appList.erase(appList.begin() + typeIdx); // ��� Ÿ���� ����ִٸ�, ��� Ÿ�� ����
	sms.sendMsg("����");
	std::cout << "���� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ���� ���� (��ü ����)
void HomeAutomation::ctrlPower(const int& typeIdx, const int& appIdx) {
	if (isEmpty()) return;
	Appliance* app = nullptr;
	int powerFlag;
	if (inputNum(powerFlag, 2, "���� ���� ��ȣ", []() { std::cout << "1. ON\n2. OFF" << std::endl; })) { return; }
	powerFlag %= 2;
	for (int i = 0; i < appList.size(); i++) {
		for (int j = 0; j < appList[i].size(); j++) {
			if (typeIdx == -1 /* ��ü ���� �޴� */ || (typeIdx == i && appIdx == j) /* Ư�� ��� ���� */ ) {
				app = appList[i][j];
				app->setPowerFlag(powerFlag);
			}
		}
	}
	sms.setName("��ü Ÿ��", "��ü ���"); 
	if (typeIdx != -1) sms.setName(app->getTypeName(), app->getName());
	sms.sendMsg("����");
	std::cout << "���� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ���� ����
void HomeAutomation::changeName(const int& typeIdx, const int& appIdx) {
	Appliance* app;
	std::string newAppName;
	if (inputName(newAppName, appList, typeIdx)) { return; }
	app = appList[typeIdx][appIdx];
	app->setName(newAppName);
	std::cout << "���� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ��� ���� ����
void HomeAutomation::setState(const int& typeIdx, const int& appIdx, const int& sel) {
	Appliance* app = appList[typeIdx][appIdx];
	int st;
	if (app->getStateSize() < sel) {    // �ش� ��� State ���� ��� ������
		std::cout<< "���� ��� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return;
	}
	if (app->getPowerFlag() == false) { // �ش� ��� ���� ���� ������
		std::cout << "���� ON �ʿ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return;
	}
	while (true) {
		if (inputNum(st, MAX_INT, "������ ���� ��ȣ", std::bind(&Appliance::showStateList, app, sel))) { return; }
		if (Appliance::checkStateRange(app, sel, st)) continue; // �Է� ���� �˻�
		app->setState(sel, st);
		break;
	}
	sms.setName(app->getTypeName(), app->getName());
	sms.sendMsg("����");
	std::cout << "���� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ��� ��ȸ
void HomeAutomation::displayAllInfo() {
	if (isEmpty()) return;
	Appliance* app;
	double power = 0;
	system("cls");
	for (int i = 0; i < appList.size(); i++) {
		app = appList[i][0];
		std::cout << app->getTypeName() << std::endl;
		power += app->powerCalc(); // �� ��� �� ������� ���� ���·� �ջ�
		displayAppList(i);         // �� Ÿ�� -> ��� �� ���� ���
		std::cout << std::endl;
	}
	std::cout << std::fixed;
	std::cout.precision(2);
	std::cout << "��� ���·� : " << power << " (���� ��� ��� ���·� : " << Appliance::getPower() << ")" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "��ȸ �Ϸ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// ��ϵ� Ÿ�� ���� ���
void HomeAutomation::displayTypeList() {
	Appliance* app;
	for (int i = 0; i < appList.size(); i++) {
		app = appList[i][0];
		std::cout << i + 1 << ". " << app->getTypeName() << std::endl;
	}
}
// ��ϵ� ��� �� ���� ���
void HomeAutomation::displayAppList(const int& typeIdx) {
	Appliance* app;
	for (int i = 0; i < appList[typeIdx].size(); i++) {
		app = appList[typeIdx][i];
		std::cout << i + 1 << ". ";
		app->display();
		std::cout << std::endl;
	}
}
// �ش� Ÿ�� ��ȣ�� ��ϵ� Idx Ž��, �� ��ϵ� Ÿ�� �� �� -1 ����
const int HomeAutomation::getTypeIdx(const int& type) {
	for (int i = 0; i < appList.size(); i++) {
		if (appList[i][0]->getType() == type) return i;
	}
	return -1;
}
const bool HomeAutomation::isEmpty() {
	if (appList.size() == 0) {
		std::cout << "��� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return true;
	}
	return false;
}
// Ÿ�� �� ��� ���� ��
const bool HomeAutomation::getIdx(int& typeIdx, int& appIdx) {
	if (inputNum(typeIdx, appList.size(), "Ÿ�� ��ȣ", std::bind(&HomeAutomation::displayTypeList, this))) { return true; }
	typeIdx--;
	if (inputNum(appIdx, appList[typeIdx].size(), "��ǰ ��ȣ", std::bind(&HomeAutomation::displayAppList, this, typeIdx))) { return true; }
	appIdx--;
	return false;
}
// int�� ���� ����ڷκ��� �Է�
const bool HomeAutomation::inputNum(int& num, const int& comp, const std::string& str, const voidHandler_t display) {
	while (true) {
		system("cls");
		if (display != nullptr) display();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << str << "�� �Է��ϼ��� (���� : 0) >> ";
		std::cin >> num;
		if (bufCheck()) continue;
		if (num == 0) return true;
		if (checkRange(num, comp)) continue;
		return false;
	}
}
// �Է� ���� �˻�
const bool HomeAutomation::checkRange(const int& num, const int& comp) {
	if (num < 0 || num > comp) {
		std::cout << "�Է� ���� �ʰ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return true;
	}
	return false;
}
// ���� ����ڷκ��� �Է� 
const bool HomeAutomation::inputName(std::string& appName, const appList_t& appList, const int& typeIdx, const voidHandler_t display) {
	while (true) {
		system("cls");
		if (display != nullptr) display();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "������ �Է��ϼ��� (���� : 0) >> ";
		std::cin >> appName;
		if (bufCheck()) continue;
		if (appName.length() == 1 && appName[0] == '0') return true;
		if (checkRange(appName.length(), MAX_LENGTH)) continue;
		if (typeIdx != -1 && (checkNameDup(appName, appList, typeIdx))) continue;
		return false;
	}
}
// ���� �ߺ� �˻�
const bool HomeAutomation::checkNameDup(const std::string& name, const appList_t& appList, const int& typeIdx) {
	Appliance* app;
	for (int i = 0; i < appList[typeIdx].size(); i++) {
		app = appList[typeIdx][i];
		if (name == app->getName()) {
			std::cout << "���� �ߺ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
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
		std::cout << "�߸��� �Է�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return true;
	}
	return false;
}
