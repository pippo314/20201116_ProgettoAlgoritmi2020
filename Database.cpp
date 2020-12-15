#include "Database.h"


void Database::menu() {
    //TODO [Miriana]
    //TODO fare in modo che si possa fare query anche da linea di comando direttamente
    //TODO fare in modo che file di input si prendano dalla cartella "input"
    bool quit = false;
    string command,fn;
    string exit_command = "QUIT()";
    int i = 0;
    vector<string> query;
    //system("cls");  //pulisco lo schermo

    this->load_db();    //legge DB da db.txt

    while(!quit){   //ripropongo il menu all'utente finchè non scrive "QUIT"
        cout << this->commands();    //stampa i comandi a schermo
        cin >> command;

        if(command == exit_command){
            // l'utente ha deciso di uscire dal programma
            quit = true;
            this->save_db();
        }
        else{//l'utente ha scelto una voce del menu
            //converto la stringa in intero
            char *cm = new char[command.length() + 1];
            strcpy(cm, command.c_str());
            i = atoi(cm); //converto string in int

            switch (i) {
                case 1: //legge query da file e la stampa su schermo
                    //system("CLS"); //pulisce lo schermo
                    cout << "[formato es. \"input.txt\", senza spazi prima o dopo il nome del file]"
                            "\nInserire il nome del file contenente una o piu' query: ";
                    cin >> fn;
                    query = this->read_query_from_file(fn);
                    for(int l=0;l < query.size();l++)
                        cout << query[l] << " ";
                    if(!query.empty()){
                        if(this->runQuery(query))
                            cout << "Query eseguita con successo.\n";
                        else
                            cout << "Non e' stata eseguita nessuna query.\n";
                    }else{
                        cout << "Il file non esiste o non contiene una query valida.\n";
                    }
                    break;
                case 2: //stampa lo stato del DB
                    cout << this->print_DB();
                    break;
                default:
                    cout << "Comando non riconosciuto\n";
            }
            system("pause"); //bisogna premere invio anche se dice che qualunque tasto va bene
        }
    }
}

string Database::commands() {
    string comm;
    //system("CLS"); //pulisce lo schermo, dovrebbe ma non lo fa
    comm = "Menu: ----- inserisci il numero o la parola corrispondente al comando (senza spazi) ----- \n"
           "      [1] - Inserisci il nome di un file contenente una query \n"
           "      [2] - Stampa lo stato del DB\n"
           " [QUIT()] - Esci dal programma e salva lo stato del DB\n "
           "\tComando: ";
    return comm;
}

vector<string> Database::read_query_from_file(string fn) {
    //TODO [Miriana]
    //TODO gestire date negative ecc scritte da parte dell'utente e eventuali errori
    //TODO errore se due colonne della tabella hanno lo stesso nome
    //TODO il campo char può contenere caratteri speciali (es : , ; ecc.)
    //TODO convertire la parola ALLCAPS prima di confrontarla con WHERE, SELECT, ...
    string word, word2;
    char del[5] = {')','(',';',','};
    string::size_type j;
    int i,k;
    fstream f;
    vector<string> query;   //vettore di stringhe contenente l'intera query

    //converto string in char*
    char *filename = new char[fn.length() + 1];
    strcpy(filename, fn.c_str());

    //apro il file e leggo la query
    f.open(filename);
    if (f.good() && f.is_open()) { //controllo che il file esista
        cout << "File trovato... Lettura in corso... \n";
        if(f.peek() != EOF){  //controllo che il file non sia vuoto
            while (f >> word) { //leggo una stringa alla volta e la metto nel vettore di stringhe contenente quindi l'intera query
                if(!word.empty()){ //se il file non è vuoto vado avanti
                    for(i = 0; i < word.size();i++); //calcola una sola volta la dimensione della parola

                    if(!word.compare(");") || word[i] == ';' ){//è arrivato il fondo della query
                        return query;
                    }

                    while(!word.compare("(") || !word.compare(")") || !word.compare(";") || !word.compare(",")){
                        f >> word;  //se una parola contiene solo un carattere speciale non la considero
                    }

                    //mi libero di altri eventuali caratteri inutili


                    for(k = 0; k < 4; k++){
                        j = word.find(del[k]);
                        if(j != string::npos)
                            word.erase(j, 1);
                    }

                    if((word.find('"')!=string::npos)){ //è una stringa, deve stare tutta in una parola
                        //word.erase(j,1); //elimino dalla parola il carattere ' " '
                        f >> word2; //leggo la word successiva
                        while((j = word2.find('"') )==string::npos){ //finchè non trovo il carattere di fine stringa aggiungo parole alla stringa
                            word.append(" ");
                            word.append(word2);
                            f >> word2;
                        }
                        word.append(" ");
                        word.append(word2);
                    }

                    // cerca se c'è ",
                    if((j = word.find("\","))!=string::npos)
                        word.erase(j+1,1);
                    if((j = word.find(");"))!=string::npos){
                        word.erase(j,2);
                    }
                    query.push_back(word);  //aggiungo la parola alla stringa della query

                }else{
                    cout << "Il database e' vuoto.";
                }
            }
        }
    }else{
        cout << "Non e' stato possibile aprire il file\n";
    }
    f.close();

    return query;
}

map<string, int> Database::getWordsMap() {
    map<string, int> map;
    //riempo la mappa con i tipi di dato riconosciuti (presenti nelle query)
    map["INT"] = 1;
    map["FLOAT"] = 2;
    map["CHAR"] = 3;
    map["TEXT"] = 4;
    map["DATE"] = 5;
    map["TIME"] = 6;

    map["CREATE"] = 10;
    map["INSERT"] = 20;
    map["DELETE"] = 30;
    map["TRUNCATE"] = 40;
    map["UPDATE"] = 50;
    map["SELECT"] = 60;
    map["DROP"] = 70;

    //restituisco la mappa piena
    return map;
}


bool Database::runQuery(const vector<string>& query) {
    map<string, int> data_types = this->getWordsMap();   //riempo il dizionario dei tipi di dato accettati
    map<string, int>::iterator i_dt;
    int i,j = 0;
    bool ret = false;

    if(query.empty()){
        return false; //il file è vuoto
    }

    i_dt = data_types.find(query[j]);   //confronto la prima parola della query con la mappa
    if(i_dt == data_types.end())
        i = 0;
    else
        i = i_dt->second;

    switch (i){
        case 10:    //CREATE
            ret = this->create_table(query);
            break;
        case 20:    //INSERT
            ret = this->insert_values(query);
            break;
        case 30:    //DELETE
        //TODO  [Federico] metti qua il tuo metodo
            break;
        case 40:    //TRUNCATE
        //TODO [Federico] metti qua il tuo metodo
        break;
        case 50:    //UPDATE
            ret = this->update(query);
            break;
        case 60:    //SELECT
        //TODO [Walter] metti qua il tuo metodo
            this->select(query);
            break;
        case 70:    //DROP
            ret = this->drop_table(query[j+2]);
            break;
        default:
            cout << "Query non riconosciuta.\n";
            ret = false;
    }
    //this->save_db();
    return ret; //se qualcosa va storto restituisco false durante l'esecuzione
}

bool Database::table_exists(string name) {
    for(int i = 0; i < this->tables.size(); i++){
        if(this->tables[i].name == name)
            return true;
    }
    return false;
    //return (this->tables.find(name) != this->tables.end()); //return vero se esiste, falso se non è presente nel db
}

void Database::load_db(string filename) {
    ifstream f;
    char del[5] = {')','(',';',','};
    string::size_type j;
    int i,k;
    vector<string> query;
    //int j,i = 0,k = 0;
    string word,word2;
    bool quit = false;
    vector<T>::iterator f_number;
    string table_name;
    vector< vector<string> > records;
    vector<string> r;
    char *fn = new char[filename.length() + 1];
    strcpy(fn, filename.c_str());

    cout << "Lettura dell'ultimo DB salvato in corso...\n";
    f.open(fn);
    if (f.good() && f.is_open()) { //controllo che il file esista e sia riuscito a aprirlo
        cout << "File trovato... Lettura in corso... \n";
        if(f.peek() != EOF){  //controllo che il file non sia vuoto
            while (f >> word) { //leggo una stringa alla volta e la metto nel vettore di stringhe contenente quindi l'intera query

                if(word.compare("<<<")){ //nuova tabella
                    query.push_back("CREATE");
                    quit = false;
                    while (f >> word && !quit){  //legge tutta la query della tabella

                        for(i = 0; i < word.size();i++); //calcola una sola volta la dimensione della parola
                        if(!word.compare(");") || word[i] == ';' || !word.compare("<<")){//è arrivato il fondo della query
                            this->runQuery(query);
                            table_name = query[2]; //salvo il nome della tabella corrente
                            //cout << table_name <<"\n";
                            query.clear();
                            quit = true;
                        }else{ //continua a leggere la query
                            while(!word.compare("(") || !word.compare(")") || !word.compare(";") || !word.compare(",")){
                                f >> word;  //se una parola contiene solo un carattere speciale non la considero
                            }

                            //mi libero di altri eventuali caratteri inutili
                            for(k = 0; k < 4; k++){
                                j = word.find(del[k]);
                                if(j != string::npos)
                                    word.erase(j, 1);
                            }
                            if((word.find('"')!=string::npos)){ //è una stringa, deve stare tutta in una parola
                                //word.erase(j,1); //elimino dalla parola il carattere ' " '
                                f >> word2; //leggo la word successiva
                                while((j = word2.find('"') )==string::npos){ //finchè non trovo il carattere di fine stringa aggiungo parole alla stringa
                                    word.append(" ");
                                    word.append(word2);
                                    f >> word2;
                                }
                                word.append(" ");
                                word.append(word2);
                            }
                            // cerca se c'è "
                            if((j = word.find("\","))!=string::npos)
                                word.erase(j+1,1);
                            if((j = word.find(");"))!=string::npos){
                                word.erase(j,2);
                            }
                            query.push_back(word);  //aggiungo la parola alla stringa della query
                        }
                    }
                }
                if(word == "<<") {   //leggo i valori della tabella

                    quit = false;
                    f_number = find_table_by_name(table_name);
                    j = distance(this->tables.begin(),f_number);


                    while(f >> word && !quit){ //scorro tutto il resto finchè finisce il file o trovo una nuova tabella
                        if(word == "<<<"){
                            quit = true;
                        }else{
                            if(word != "<"){ //scorro una riga e ne salvo i valori
                                if(!word.find('\"')){ //è una stringa di più parole
                                    do{
                                        f >> word2;
                                        word += " " + word2;
                                    }while((word2.find('\"'))== string::npos); //la memorizzo come stringa unica (es.indirizzo)
                                }
                                if(word == "-"){ //se è un trattino non ha valore
                                    r.push_back("");
                                }else{
                                    r.push_back(word);
                                }
                            }else{
                                this->tables[j].table.records.push_back(r);
                                r.clear();
                            }
                        }
                    }
                }
            }

        }else{
            cout << "Il database e' vuoto.";
        }
        cout << "Lettura del database da \"" << filename << "\" avvenuta con successo.\n";

    }else{
        cout << "Non e' stato possibile aprire il file\n";
    }
    f.close();


}

void Database::save_db(const string& filename) {
    ofstream o_f;
    string query;
    char *fn = new char[filename.length() + 1];
    strcpy(fn, filename.c_str());

    o_f.open(fn);
    if(this->tables.size() != 0){ //se ci sono tabelle
        cout << this->tables.size() << "\n";
        for(int i = 0; i < this->tables.size(); i++){ //scorro le tabelle
            query += "<<<\n";    //nuova tabella
            query += "CREATE TABLE " + this->tables[i].name + "(\n";
            for(int j = 0; j < this->tables[i].table.fields.size(); j++){  //scorro i campi della tabella
                query += this->tables[i].table.fields[j].name +
                         " " + this->tables[i].table.fields[j].type;
                if(this->tables[i].table.fields[j].not_null)
                    query += " NOT NULL";
                if(this->tables[i].table.fields[j].auto_increment)
                    query += " AUTO_INCREMENT";
                query += ",\n";
            }

            //stampo primary e secondary key
            query += "PRIMARY KEY (" + this->tables[i].table.get_primary_key() + ")\n";
            if(!this->tables[i].table.get_secondary_key().empty())
                query += "SECONDARY KEY (" + this->tables[i].table.get_secondary_key() + ")\n";
            query += ");";

            //stampo i valori della tabella
            query += "\n<<\n";
            for(int j = 0; j < this->tables[i].table.records.size(); j++){  //scorro i record della tabella
                for(int k = 0; k < this->tables[i].table.records[j].size(); k++){
                    if(this->tables[i].table.records[j][k].empty()){
                        query += " - ";
                    }else{
                        query += this->tables[i].table.records[j][k] + " ";
                    }
                }
                query += " < \n";
            }
            query += "<<<\n";
        }
        o_f << query; //stampo tutto su file
    }else{
        o_f.open(fn, std::ofstream::out | std::ofstream::trunc);    //elimino tutto quello che è nel file
    }
    cout << "Database salvato con successo in " << filename << "\n";
}


string Database::print_DB() {
    string db;
    string t_name;
    int i;
    int j;
    if(this->tables.empty()){
        db = "Il database e' vuoto.\n";
    } else{
        for(i = 0; i < this->tables.size(); i++){
            cout << "\nTable \"" << this->tables[i].name << "\" :\n"    <<
                    " - Fields: \n" <<
                    "[NAME\tTYPE\tNOT_NULL\tAUTO_INCREMENT]\n";
            for(j = 0; j < this->tables[i].table.fields.size(); j++){
                cout << this->tables[i].table.fields[j].name <<
                     "\t" << this->tables[i].table.fields[j].type <<
                     //"\t" << this->tables[i].table.fields[j].value <<
                     "\t" << this->tables[i].table.fields[j].not_null <<
                     "\t" << this->tables[i].table.fields[j].auto_increment << "\n";
            }

            //scorro tutti i valori delle tabelle
            cout << "\n - Values: \n [ - ";
            for(j = 0; j < this->tables[i].table.fields.size(); j++)
                cout << this->tables[i].table.fields[j].name << " - ";
            cout << "]";

            for(int row = 0; row < this->tables[i].table.records.size(); row++){
                cout << "\n { - ";
                for(j = 0; j < this->tables[i].table.records[row].size(); j++){
                    cout << this->tables[i].table.records[row][j] << " - ";
                }
                cout << "}";
            }
            cout <<"\nPrimary key: " << this->tables[i].table.get_primary_key() <<
                 ", Secondary Key: " << this->tables[i].table.get_secondary_key() << "\n";
        }
    }
    return db;
}

bool Database::drop_table(const string& table_name) {
    vector<T>::iterator i;
    string filename;

    if(this->table_exists(table_name)){    //la tabella da eliminare esiste
        i = this->find_table_by_name(table_name);
        i->table.fields.clear(); //elimina tutti i campi della tabella
        i->table.records.clear();   //elimina tutti i record della tabella
        this->tables.erase(i); // elimina la tabella

        cout << "E' stata eliminata la tabella \"" << table_name << "\".\n";
        return true;
    }else{
        cout << "Tabella da eliminare non trovata.\n";
        return false;
    }
}

vector<T>::iterator Database::find_table_by_name(string name) {
    vector<T>::iterator i;
    for(i = this->tables.begin(); i != this->tables.end(); i++){
        if(i->name == name)
            return i;
    }
    return this->tables.end(); //tabella non trovata
}

bool Database::create_table(vector<string> query) {
    map<string, int> data_types = this->getWordsMap();   //riempo il dizionario dei tipi di dato accettati
    map<string, int>::iterator i_dt;
    string name;
    int tb_at;
    vector<T>::iterator tb;
    int i,j = 0;
    T t;
    Field field;
    Table* table = new Table();

    if(!this->table_exists(query[j+2])){ //non esiste una tabella con lo stesso nome
        name = query[j+2];
        t.name = name;
        //prendo i dati della tabella finchè non finisce la query
        for(j = 3; j < query.size(); j++){
            if(query.at(j) !="PRIMARY" && query.at(j)!="SECONDARY" ){
                field.name = query.at(j);
                field.type = query.at(j + 1);
                j+=2;
                if(query.at(j) == "NOT" && query.at(j+1) == "NULL"){
                    field.not_null = true;
                    j +=2;
                }else{
                    field.not_null = false;
                }
                if(query.at(j) == "AUTO_INCREMENT" && field.type == "INT"){ //auto_increment può solo essere applicaato a INT
                    field.auto_increment = true;
                }else{
                    field.auto_increment = false;
                    j-=1;
                }
                table->add_field(field);
            }else{
                if(query.at(j)=="PRIMARY" && query.at(j+1) == "KEY"){
                    table->set_primary_key(query.at(j+2));
                    j+=2;
                }else{
                    if(query.at(j)=="SECONDARY" && query.at(j+1) == "KEY")
                        table->set_secondary_key(query.at(j+2));
                    else{
                        cout << "Attenzione la tabella \"" << t.name << "\" non ha chiavi.";
                    }
                }

            }
            t.table = new Table(table);
        }

        this->tables.push_back(t); //aggiungo la tabella al database
        //assegno anche il nome
        tb = this->find_table_by_name(name);
        tb_at = distance(this->tables.begin(),tb);
        this->tables[tb_at].table.set_name(name);

        return true;
    }else{
        cout << "Esiste gia' la tabella \"" << query[j+2] << "\", si prega di cancellare la tabella prima di crearne un'altra con lo stesso nome.\n";
        return false;
    }
}

bool Database::insert_values(vector<string> query) { //inserisce i valori nelle tabelle
    int j = 0;
    vector<T>::iterator i;
    Field r;
    vector<string> names;
    vector<string> values;
    if(this->table_exists(query[j+2]) && query[j+1] == "INTO"){    //la tabella esiste
        i = this->find_table_by_name(query[j+2]);
        j+=3;

        for(;query[j] != "VALUES";j++){
            names.push_back(query[j]);
            //cout << query[j] << " ";
        }
        j++; //salta la parola VALUES
        if(j > query.size())
            return false; //non ha trovato la parola VALUES
        for(;j < query.size(); j++){
            values.push_back(query[j]);
        }

        return i->table.add_record(names, values);  //se qualcosa va storto return false

    }else{ //la tabella non esiste
        cout << "La tabella \"" << query[j+2] << "\" non esiste.\n";
        return false;
    }
}

bool Database::update(vector<string> query) { //TODO !ANCORA DA CONTROLLARE SE FUNZIONA [Miriana]
    int i = 0,j;
    vector<T>::iterator table_pos;
    vector<Field>::iterator field_pos;
    vector<string> set_field;
    vector<string> set_values;
    vector<bool> rows;

    string field_name;
    string value;

    string table_name = query[1];
    if(this->table_exists(table_name)){
        //trovo il SET
        while(query[i] != "SET"){
            i++;
        }
        i++;
        //memorizzo i campi e i corrispettivi valori in cui devono cambiare
        for(;query[i] != "WHERE" && i < query.size(); i++){
            set_field.push_back(query[i]);
            cout << query[i] << " " << query[i+2] << "\n";
            set_values.push_back(query[i+2]);
            i +=2;
        }
        //eseguo la where e cambio i campi dove richiesto
        table_pos = this->find_table_by_name(table_name);
        //la tabella esiste
        rows = table_pos->table.records_where(query); //chiamo il metodo che crea una vettore che indica in qualci righe devo eseguire la query in base al WHERE
        i = 0;
        for(int row = 0; row < table_pos->table.records.size(); i++, row++){ //scorro tutte le righe della tabella
            if(rows[row]){ //posso fare l'update della riga
                field_pos = table_pos->table.get_field_number(set_field[i]);
                j = distance(table_pos->table.fields.begin(),field_pos);
                table_pos->table.records[row][j] = set_values[i];
                cout << "Riga aggiornata\n";
            }
        }
    }else{
        cout << "Nel DB non è presente la tabella \"" << table_name << "\"\n";
        return false;    //tabella non trovata
    }
}

Table Database::getTable(string name) {
    vector<T>::iterator i;
    for (i = this->tables.begin(); i != this->tables.end(); i++) {
        if(i->name == name)
            return i->table;
    }
}
map<string, vector<string>> mapQuery(vector<string> query, map<string, string> alias_map){
    map<string, vector<string>> ret;
    vector<string> init = {"NONE"};
    vector<string> select;
    vector<string> from;
    vector<string> where;
    vector<string> order_by;
    vector<string>::iterator from_it = find(query.begin(), query.end(), "FROM");
    vector<string>::iterator where_it = find(query.begin(), query.end(), "WHERE");
    vector<string>::iterator order_it = find(query.begin(), query.end(), "ORDER BY");
    vector<string>::iterator it = query.begin() + 1; // esclude "SELECT"

    // ret vector init
    ret["SELECT"] = init;
    ret["FROM"] =  init;
    ret["WHERE"] = init;
    ret["ORDER BY"] = init;

    while(it != from_it){
        if(*it == "AS"){
            alias_map[*(it - 1)] = *(it + 1);
            it+=2;
        } else {
            select.push_back(*it);
            it++;
        }
    }
    while(it != where_it) {
        from.push_back(*it);
        it++;
    }
    while(it != order_it) {
        where.push_back(*it);
        it++;
    }
    while(it != query.end()){
        order_by.push_back(*it);
        it++;
    }

    ret["SELECT"] = select;
    ret["FROM"] = from;
    ret["WHERE"] = where;
    ret["ORDER BY"] = order_by;

    return ret;
}

void Database::select(vector<string> query){

    Table t; //tabella temporanea su cui operare
    Table result = new Table(); //tabella finale da stampare
    vector<bool> where_lines; //righe della tabella dove ha effetto la where
    vector<int> order_fields; // index of rows sorted as the order by requests
    map<string, string> alias_map; // tabella corispondenze per alias
    map<string, vector<string>> query_map = mapQuery(query, alias_map);
    int n_tables = query_map.at("FROM").size();
    vector<string>::iterator i;
    vector<int> fields_indexes;
    vector<vector<string>> print_me;

    //checking if all the tables are in the database
    for (i = query_map.at("FROM").begin(); i != query_map.at("FROM").end(); ++i) {
        if (this->table_exists(*i) == false){
            cout << "One of the selected tables does not exists.";
            return; //or throw exception
        }
    }

    // controlla la necessità di concat di tabelle e salva in una variabile locale Table t la tabella su cui operare
    if (n_tables > 1) {
        Table t; //TODO: creare metodo per concatenazione tabelle (necessaria implementazione gestione chiavi esterne)
    } else {
        Table t = this->getTable(query_map.at("FROM").at(0));
    }

    //check WHERE conditions
    if(query_map.at("WHERE").at(0) != "NONE") {
        where_lines = t.records_where(query);
    } else {
        //niente where nella query
    }
    if(query_map.at("ORDER BY").at(0) != "NONE") {
        order_fields = t.get_order_by_indexes(query_map.at("ORDER BY").at(0), *(query_map.at("ORDER BY").end()));
    } else {
        //niente order by nella query
    }



/*  try {
    vector<string> where_condition = query_map.at("WHERE");
    where_lines = t.records_where(query);
  } catch (const std::out_of_range& oor) { //niente where nella query
    // TODO: assicurarsi che in questo caso la funzione ritorni un vettore tutto true
  }

  try {
    order_fields = t.get_order_by_indexes(query_map.at("ORDER BY").at(0), *(query_map.at("ORDER BY").end()));
  } catch (const std::out_of_range& oor) { // no order by nella query
    // TODO:
  }
*/


    //aggiungo gli indici dei fields richiesti dalla select
    for (i = query_map.at("SELECT").begin(); i != query_map.at("SELECT").end(); ++i)
    {
        fields_indexes.push_back(t.get_field_index(*i));
    }
    //TODO: print Fields first
    for (int j = 0; j < fields_indexes.size(); ++j) {
        int field_index = fields_indexes.at(j);
        string field_name = t.fields.at(field_index).name;
        if (alias_map.count(field_name) > 0) {
            string tmp = alias_map.at(field_name); //variable to be printed
            cout << setw(15) << tmp;
        } else {
            cout << setw(15) << field_name;
        }
    }

    cout << endl;

    //per ogni riga che rispetta le condizioni della where stampo il record corrispondente al field
    // TODO: maybe call a function here like printTable
    for (size_t row = 0; row < where_lines.size(); row++)
    {
        if( (where_lines.at(row)) == true){
            //TODO: stampa dei valori a video
            for (int j = 0; j < fields_indexes.size(); ++j) {
                //next variable is the index of the field
                int col = fields_indexes.at(j);
                //next variable contains the record to be printed
                string record = t.records.at(row).at(col);
                cout << setw(15) << record;
            }
        }
        cout << endl;
    }


}
