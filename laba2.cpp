#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <limits>
#include <memory>
#include <clocale>
#include <windows.h>

using namespace std;

enum class Service {
    ELECTRICITY,
    WATER,
    GAS,
    HEATING,
    MAINTENANCE
};

string serviceToString(Service service) {
    switch (service) {
    case Service::ELECTRICITY: return "Электричество";
    case Service::WATER: return "Вода";
    case Service::GAS: return "Газ";
    case Service::HEATING: return "Отопление";
    case Service::MAINTENANCE: return "Обслуживание";
    default: return "Неизвестная услуга";
    }
}

class Resident {
private:
    int id;
    string name;
    string address;

public:
    vector<Service> services;
    vector<double> consumption;

    Resident(int id, const string& name, const string& address)
        : id(id), name(name), address(address) {
    }

    ~Resident() {
        services.clear();
        consumption.clear();
    }

    double calculateTotalCost(const map<Service, double>& tariffs) const {
        double total = 0.0;
        for (size_t i = 0; i < services.size(); ++i) {
            auto it = tariffs.find(services[i]);
            if (it != tariffs.end()) {
                total += it->second * consumption[i];
            }
        }
        return total;
    }

    void displayInfo(const map<Service, double>& tariffs) const {
        cout << "\n=== Информация о жильце ===" << endl;
        cout << "ID: " << id << endl;
        cout << "ФИО: " << name << endl;
        cout << "Адрес: " << address << endl;

        if (services.empty()) {
            cout << "Услуги не потреблялись." << endl;
        }
        else {
            cout << "\nПотребленные услуги:" << endl;
            cout << setw(20) << left << "Услуга"
                << setw(15) << "Потребление"
                << setw(15) << "Стоимость" << endl;
            cout << string(50, '-') << endl;

            for (size_t i = 0; i < services.size(); ++i) {
                auto it = tariffs.find(services[i]);
                double cost = (it != tariffs.end()) ? it->second * consumption[i] : 0.0;

                cout << setw(20) << left << serviceToString(services[i])
                    << setw(15) << fixed << setprecision(2) << consumption[i]
                    << setw(15) << cost << " руб." << endl;
            }

            cout << string(50, '-') << endl;
            cout << setw(35) << left << "Общая стоимость: "
                << calculateTotalCost(tariffs) << " руб." << endl;
        }
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getAddress() const { return address; }
};

class HousingOffice {
private:
    static HousingOffice* instance;

    vector<shared_ptr<Resident>> residents;
    map<Service, double> tariffs;
    double totalRevenue;

    HousingOffice() : totalRevenue(0.0) {
        tariffs[Service::ELECTRICITY] = 0.0;
        tariffs[Service::WATER] = 0.0;
        tariffs[Service::GAS] = 0.0;
        tariffs[Service::HEATING] = 0.0;
        tariffs[Service::MAINTENANCE] = 0.0;
    }

public:
    HousingOffice(const HousingOffice&) = delete;
    HousingOffice& operator=(const HousingOffice&) = delete;

    ~HousingOffice() {
        residents.clear();
        tariffs.clear();
    }

    static HousingOffice* getInstance() {
        if (instance == nullptr) {
            instance = new HousingOffice();
        }
        return instance;
    }

    void addResident(const string& name, const string& address) {
        int newId = residents.empty() ? 1 : residents.back()->getId() + 1;
        residents.push_back(make_shared<Resident>(newId, name, address));
        cout << "Жилец добавлен успешно. ID: " << newId << endl;
    }

    void addServiceConsumption(int residentId, Service service, double amount) {
        for (auto& resident : residents) {
            if (resident->getId() == residentId) {
                resident->services.push_back(service);
                resident->consumption.push_back(amount);

                auto it = tariffs.find(service);
                if (it != tariffs.end()) {
                    totalRevenue += it->second * amount;
                }

                cout << "Потребление услуги добавлено успешно." << endl;
                return;
            }
        }
        cout << "Жилец с ID " << residentId << " не найден." << endl;
    }

    void setTariff(Service service, double rate) {
        tariffs[service] = rate;
        cout << "Тариф для " << serviceToString(service)
            << " установлен: " << rate << " руб./ед." << endl;
    }

    double calculateTotalForResident(const string& surname) const {
        for (const auto& resident : residents) {
            if (resident->getName().find(surname) != string::npos) {
                return resident->calculateTotalCost(tariffs);
            }
        }
        return -0.1;
    }

    void displayStats() const {
        cout << "\n=== Статистика ЖЭС ===" << endl;
        cout << "Количество жильцов: " << residents.size() << endl;

        cout << "\nДействующие тарифы:" << endl;
        for (const auto& tariff : tariffs) {
            cout << setw(20) << left << serviceToString(tariff.first)
                << ": " << tariff.second << " руб./ед." << endl;
        }

        cout << "\nОбщая выручка: " << totalRevenue << " руб." << endl;

        if (!residents.empty()) {
            cout << "\nСписок жильцов:" << endl;
            for (const auto& resident : residents) {
                cout << "ID: " << resident->getId()
                    << ", ФИО: " << resident->getName()
                    << ", Адрес: " << resident->getAddress() << endl;
            }
        }
    }

    const vector<shared_ptr<Resident>>& getResidents() const {
        return residents;
    }

    const map<Service, double>& getTariffs() const {
        return tariffs;
    }
};

HousingOffice* HousingOffice::instance = nullptr;

int inputInt(const string& prompt, int minVal = 0, int maxVal = 1000000) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return value;
        }
        else {
            cout << "Ошибка ввода! Введите целое число от "
                << minVal << " до " << maxVal << "." << endl;
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        }
    }
}

double inputDouble(const string& prompt, double minVal = 0.0) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal) {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return value;
        }
        else {
            cout << "Ошибка ввода! Введите число >= " << minVal << "." << endl;
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
        }
    }
}

string inputString(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void displayMenu() {
    cout << "\n=== Меню ЖЭС ===" << endl;
    cout << "1. Добавить жильца" << endl;
    cout << "2. Установить тарифы" << endl;
    cout << "3. Добавить потребление услуги" << endl;
    cout << "4. Рассчитать сумму по фамилии" << endl;
    cout << "5. Показать статистику" << endl;
    cout << "6. Показать информацию о жильце" << endl;
    cout << "7. Выход" << endl;
    cout << "Выберите опцию: ";
}

void setConsoleEncoding() {
#ifdef _WIN32
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
#endif
}

int main() {
    setConsoleEncoding();


    cout << "Проверка на шаблон Singleton:" << endl;

    HousingOffice* office = HousingOffice::getInstance();
    HousingOffice* _office = HousingOffice::getInstance();
    cout << " office: " << office << endl;
    cout << "_office: " << _office << endl;

    int choice;

    cout << "=== Система учета ЖЭС ===" << endl;
    cout << "Для начала работы добавьте жильцов и установите тарифы." << endl;

    do {
        displayMenu();
        choice = inputInt("", 1, 7);

        switch (choice) {
        case 1: {
            string name = inputString("Введите ФИО жильца: ");
            string address = inputString("Введите адрес: ");
            office->addResident(name, address);
            break;
        }

        case 2: {
            cout << "\nУстановка тарифов:" << endl;
            cout << "1. Электричество (руб./кВт·ч)" << endl;
            cout << "2. Вода (руб./м3)" << endl;
            cout << "3. Газ (руб./м3)" << endl;
            cout << "4. Отопление (руб./Гкал)" << endl;
            cout << "5. Обслуживание (руб./мес)" << endl;

            int serviceChoice = inputInt("Выберите услугу (1-5): ", 1, 5);
            double rate = inputDouble("Введите тариф (руб./ед.): ");

            Service service;
            switch (serviceChoice) {
            case 1: service = Service::ELECTRICITY; break;
            case 2: service = Service::WATER; break;
            case 3: service = Service::GAS; break;
            case 4: service = Service::HEATING; break;
            case 5: service = Service::MAINTENANCE; break;
            }

            office->setTariff(service, rate);
            break;
        }

        case 3: {
            if (office->getResidents().empty()) {
                cout << "Сначала добавьте жильцов!" << endl;
                break;
            }

            int residentId = inputInt("Введите ID жильца: ");

            cout << "\nВыберите услугу:" << endl;
            cout << "1. Электричество" << endl;
            cout << "2. Вода" << endl;
            cout << "3. Газ" << endl;
            cout << "4. Отопление" << endl;
            cout << "5. Обслуживание" << endl;

            int serviceChoice = inputInt("Выберите услугу (1-5): ", 1, 5);
            double amount = inputDouble("Введите количество потребленных единиц: ");

            Service service;
            switch (serviceChoice) {
            case 1: service = Service::ELECTRICITY; break;
            case 2: service = Service::WATER; break;
            case 3: service = Service::GAS; break;
            case 4: service = Service::HEATING; break;
            case 5: service = Service::MAINTENANCE; break;
            }

            office->addServiceConsumption(residentId, service, amount);
            break;
        }

        case 4: {
            string surname = inputString("Введите фамилию жильца: ");
            double total = office->calculateTotalForResident(surname);
            if (total > -0.1) {
                cout << "Общая сумма для жильца с фамилией \"" << surname
                    << "\": " << fixed << setprecision(2)
                    << total << " руб." << endl;
            }
            else {
                cout << "Жилец с фамилией \"" << surname << "\" не найден." << endl;
            }
            break;
        }

        case 5: {
            office->displayStats();
            break;
        }

        case 6: {
            if (office->getResidents().empty()) {
                cout << "Нет зарегистрированных жильцов." << endl;
                break;
            }

            cout << "\nСписок жильцов:" << endl;
            for (const auto& resident : office->getResidents()) {
                cout << "ID: " << resident->getId()
                    << " - " << resident->getName() << endl;
            }

            int id = inputInt("Введите ID жильца для просмотра информации: ");

            bool found = false;
            for (const auto& resident : office->getResidents()) {
                if (resident->getId() == id) {
                    resident->displayInfo(office->getTariffs());
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Жилец с ID " << id << " не найден." << endl;
            }
            break;
        }

        case 7: {
            cout << "Выход из программы." << endl;
            break;
        }
        }

    } while (choice != 7);

    delete HousingOffice::getInstance();

    return 0;
}
