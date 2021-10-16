// Lab4.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include <clocale>

using namespace std;
const unsigned int gluttonyFactor = 1; //коэффициент обжорства
const unsigned int efficiencyFactor = 100000; //коэффициент производительности
const unsigned int amountOfDays = 5; //количество дней
const unsigned int maxAmountOfNuggets = 10000; //максимальное количество наггетсов
clock_t start;
double days = 0; //количество прошедших дней
mutex platesLock; //работа с тарелками + проверка условий
mutex printLock; //вывод в консоль

void daysRecount() {
	//пересчёт количества прошедших дней
	clock_t end = clock();
	days = (double)(end - start) / CLOCKS_PER_SEC;
}

void cookingNuggets(long long* dish, long long* eatenNuggets) {
	setlocale(LC_CTYPE, "rus");
	int amountOfNuggets = 0; //количество наггетсов, которое приготовит кук за время работы
	while (days <= amountOfDays) {
		//Проверка условий
		platesLock.lock();
		if ((dish[0] == 0) || (dish[1] == 0) || (dish[2] == 0)) {
			//Наггетсы закончились хотя бы у одного толстяка
			printLock.lock();
			cout << "Кука уволили" << endl;
			printLock.unlock();
			platesLock.unlock();
			return;
		}
		platesLock.unlock();
		platesLock.lock();
		if ((eatenNuggets[0] > maxAmountOfNuggets) && (eatenNuggets[1] > maxAmountOfNuggets) && (eatenNuggets[2] > maxAmountOfNuggets)) {
			//Все три толстяка лопнули
			printLock.lock();
			cout << "Кук не получит зарплату" << endl;
			printLock.unlock();
			platesLock.unlock();
			return;
		}
		platesLock.unlock();
		//Выкладывание наггетсов на стол
		platesLock.lock();
		//Толстяки ждут, чтобы случайно не откусить куку руки
		amountOfNuggets += efficiencyFactor;
		dish[0] += efficiencyFactor / 3;
		dish[1] += efficiencyFactor / 3;
		dish[2] += efficiencyFactor / 3;
		platesLock.unlock();
		daysRecount();
	}
	platesLock.lock();
	if (days > amountOfDays) {
		printLock.lock();
		cout << "Кук уволился сам" << endl;
		printLock.unlock();
		platesLock.unlock();
		return;
	}
	platesLock.unlock();
	printLock.lock();
	cout << "Кук приготовил " << amountOfNuggets << " наггетсов" << endl;
	printLock.unlock();
}

void eatingFatMan(short fatManID, long long& dish, long long& eatenNuggets) {
	setlocale(LC_CTYPE, "rus");
	while (days <= amountOfDays) {
		platesLock.lock();
		if (dish == 0) {
			platesLock.unlock();
			return;
		}
		if (eatenNuggets <= maxAmountOfNuggets) {
			for (size_t i = 1; i <= gluttonyFactor; i++) {
				if ((dish != 0) && (eatenNuggets < maxAmountOfNuggets + 1)) {
					eatenNuggets++;
					dish--;
				}
			}
		}
		else {
			platesLock.unlock();
			return;
		}
		platesLock.unlock();
		daysRecount();
	}
	platesLock.lock();
	if (eatenNuggets > maxAmountOfNuggets) {
		printLock.lock();
		cout << "Толстяк " << fatManID + 1 << " самоуничтожился";
		printLock.unlock();
	}
	platesLock.unlock();
}

int main() {
	setlocale(LC_CTYPE, "rus");
	long long dish[3] = { 3000, 3000, 3000 };
	long long eatenNuggets[3] = { 0, 0, 0 };
	thread cook(cookingNuggets, dish, eatenNuggets);
	thread fatMan1(eatingFatMan, 0, ref(dish[0]), ref(eatenNuggets[0]));
	thread fatMan2(eatingFatMan, 1, ref(dish[1]), ref(eatenNuggets[1]));
	thread fatMan3(eatingFatMan, 2, ref(dish[2]), ref(eatenNuggets[2]));
	start = clock();
	cook.join();
	fatMan1.join();
	fatMan2.join();
	fatMan3.join();
	cout.setf(ios::fixed);
	cout.precision(2);
	cout << "Прошло " << days << " дней" << endl;
	for (size_t i = 0; i <= 2; i++) {
		cout << "На тарелке осталось " << dish[i] << " наггетсов, толстяк " << i + 1 << " съел " << eatenNuggets[1] << " наггетсов" << endl;
	}
	system("pause");
}