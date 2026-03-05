#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <limits>

using namespace std;


unsigned int encodeTransaction(int type, int amount)
{
    return (type << 28) | amount;
}

int getType(unsigned int code)
{
    return code >> 28;
}

int getAmount(unsigned int code)
{
    return code & 0x0FFFFFFF;
}


class Account
{
protected:
    int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<unsigned int> transactions;

public:
    Account() {}

    Account(int id, string n, double b, unsigned int p)
    {
        accountId = id;
        name = n;
        balance = b;
        permissions = p;
    }

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void saveToFile(ofstream& file) = 0;

    virtual void show()
    {
        cout << accountId << " " << name << " " << balance << " " << permissions << endl;
    }

    virtual ~Account() {}
};


class SavingsAccount : public Account
{
public:
    SavingsAccount(int id, string n, double b, unsigned int p)
        : Account(id, n, b, p) {}

    void deposit(double amount)
    {
        if (permissions & 2)
        {
            balance += amount;
            transactions.push_back(encodeTransaction(1, (int)amount));
            cout << "Deposit successful\n";
        }
        else
            cout << "Deposit permission denied\n";
    }

    void withdraw(double amount)
    {
        if (permissions & 1)
        {
            balance -= amount;
            transactions.push_back(encodeTransaction(2, (int)amount));
            cout << "Withdraw successful\n";
        }
        else
            cout << "Withdraw permission denied\n";
    }

    void saveToFile(ofstream& file)
    {
        file << "ACCOUNT Savings\n";
        file << accountId << " " << name << " " << balance << " " << permissions << "\n";
        file << "TRANSACTIONS\n";

        for (unsigned int t : transactions)
            file << t << "\n";
    }
};


class CurrentAccount : public Account
{
public:
    CurrentAccount(int id, string n, double b, unsigned int p)
        : Account(id, n, b, p) {}

    void deposit(double amount)
    {
        if (permissions & 2)
        {
            balance += amount;
            transactions.push_back(encodeTransaction(1, (int)amount));
            cout << "Deposit successful\n";
        }
        else
            cout << "Deposit permission denied\n";
    }

    void withdraw(double amount)
    {
        if (permissions & 1)
        {
            balance -= amount;
            transactions.push_back(encodeTransaction(2, (int)amount));
            cout << "Withdraw successful\n";
        }
        else
            cout << "Withdraw permission denied\n";
    }

    void saveToFile(ofstream& file)
    {
        file << "ACCOUNT Current\n";
        file << accountId << " " << name << " " << balance << " " << permissions << "\n";
        file << "TRANSACTIONS\n";

        for (unsigned int t : transactions)
            file << t << "\n";
    }
};


void saveAll(vector<Account*>& accounts)
{
    ofstream file("bank.txt");

    for (Account* acc : accounts)
        acc->saveToFile(file);

    file.close();
}


void loadAll(vector<Account*>& accounts)
{
    ifstream file("bank.txt");

    if (!file)
    {
        cout << "File not found\n";
        return;
    }

    string word, type;

    while (file >> word)
    {
        if (word == "ACCOUNT")
        {
            file >> type;

            int id;
            string name;
            double balance;
            unsigned int perm;

            file >> id >> name >> balance >> perm;

            if (type == "Savings")
                accounts.push_back(new SavingsAccount(id, name, balance, perm));
            else
                accounts.push_back(new CurrentAccount(id, name, balance, perm));
        }
    }

    file.close();
}


double monthlyTotals[12] = {0};


int main()
{
    vector<Account*> accounts;
    int choice;

    while (true)
    {
        cout << "\n1 Create Account\n2 Deposit\n3 Withdraw\n4 Show Accounts\n5 Save to File\n6 Load from File\n7 Exit\n";
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1)
        {
            int type, id;
            string name;
            double bal;
            unsigned int perm;

            cout << "1 Savings 2 Current: ";
            cin >> type;

            if (type != 1 && type != 2)
            {
                cout << "Invalid account type\n";
                continue;
            }

            cout << "Enter id name balance permissions: ";
            cin >> id >> name >> bal >> perm;

            if (type == 1)
                accounts.push_back(new SavingsAccount(id, name, bal, perm));
            else
                accounts.push_back(new CurrentAccount(id, name, bal, perm));
        }

        else if (choice == 2)
        {
            int index;
            double amount;

            cout << "Account index: ";
            cin >> index;

            cout << "Amount: ";
            cin >> amount;

            if (index >= 0 && index < (int)accounts.size())
                accounts[index]->deposit(amount);
            else
                cout << "Invalid index\n";
        }

        else if (choice == 3)
        {
            int index;
            double amount;

            cout << "Account index: ";
            cin >> index;

            cout << "Amount: ";
            cin >> amount;

            if (index >= 0 && index < (int)accounts.size())
                accounts[index]->withdraw(amount);
            else
                cout << "Invalid index\n";
        }

        else if (choice == 4)
        {
            for (Account* acc : accounts)
                acc->show();
        }

        else if (choice == 5)
        {
            saveAll(accounts);
            cout << "Saved\n";
        }

        else if (choice == 6)
        {
            loadAll(accounts);
            cout << "Loaded\n";
        }

        else if (choice == 7)
        {
            break;
        }
    }

    for (Account* acc : accounts)
        delete acc;

    accounts.clear();

    return 0;
}