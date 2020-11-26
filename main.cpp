using namespace std;

#include "Database.h"

int main() {
    Database *db = new Database();
    db->menu();
    //db->load_db();

    //una volta uscito dal menu stampa lo stato del database
    cout << "Stato del database: \n";
    cout << db->print_DB();
}
