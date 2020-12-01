#include "Table.h"


Table::Table() {
    this->primary_key = "";
    this->secondary_key = "";
}

void Table::set_name(string n) {
    this->name = n;
}

void Table::set_primary_key(string key) {
    this->primary_key = key;
}

void Table::set_secondary_key(string key) {
    this->secondary_key = key;
}

Table::Table(Table *t) {
    this->fields = t->fields;
    this->primary_key = t->primary_key;
    this->secondary_key = t->secondary_key;
}

string Table::get_primary_key() {
    return this->primary_key;
}

string Table::get_secondary_key() {
    return this->secondary_key;
}

bool Table::add_record(vector<string> fields_name, vector<string> values) {  //aggiunge un record alla tabella
    int i,j,n = 0;
    vector<Field>::iterator f_number;
    string last_number;
    vector<string> record;

    for(i = 0; i < this->fields.size(); i++){
        record.push_back("");
    }
    //string record[this->fields.size()]; //record da aggiungere in tabella

    if(this->fields.empty())
        return false;
    //Assegno i valori ai campi
    for(i = 0; i < fields_name.size(); i++){ //scorro i campi dati
        if((f_number = get_field_number(fields_name[i])) != this->fields.end()){ //campo trovato nella tabella
           j = distance(this->fields.begin(),f_number); //trasforma iteratore in int
           record[j] = values[i];   //aggiungo il valore al campo del record
        }else{ //il campo dato non corrisponde a uno di quelli presenti nella tabella o non sono stati assegnati i campi
            cout << "Il campo \"" << fields_name[i] << "\" non e' stato trovato in tabella.\n";
            return false;
        }
    }

    //controllo eventuali auto_increment e not null
    for(f_number = this->fields.begin(); f_number != this->fields.end(); f_number++){
        if(f_number->auto_increment){   //è un campo da aumentare
            j = distance(this->fields.begin(),f_number); //trasforma iteratore in int
            if(this->records.empty())
                record[j] = "0";
            else{
                last_number = this->records[this->records.size()-1][j];
                stringstream geek(last_number);
                geek >> n;
                n++; //incremento il numero per poi assegnarlo al record
                ostringstream incremented_number;
                incremented_number << n;
                record[j] = incremented_number.str(); //assegno il numero incrementato
            }
        }
        if(f_number->not_null && record[j].empty()){
            cout << "Il campo \"" << f_number->name << "\" non puo' rimanere vuoto.\n";
            return false;
        }
    }

    this->records.push_back(record);
    return true;    //è andato tutto bene
}

vector<Field>::iterator Table::get_field_number(string name) {
    vector<Field>::iterator i;
    for(i = this->fields.begin(); i != this->fields.end(); i++){
        if(i->name == name)
            return i;   //ha trovato il campo
    }
    return i;   //non ha trovato il campo
}

void Table::add_field(Field f) {
    this->fields.push_back(f);
}

//TODO [Miriana]
//TODO !ANCORA DA CONTROLLARE SE FUNZIONA
//TODO nel caso ci siano due o più tabelle nella where, legge solo le condizioni riguardanti la tabella sulla quale
//TODO è chiamato il metodo
vector<bool> Table::records_where(vector<string> query) {
    vector<bool> ret;
    bool _and = false;
    bool _not = false;
    int k;

    map<string, int> where_map = this->getWhereMap();   //riempo il dizionario dei "caratteri" accettati dalla where (es <,>=, BETWEEN,...)
    map<string, int>::iterator i_dt;    //per scorrere la mappa
    int i = 0,j,op;
    vector<Field>::iterator field_pos; //iteratore per il vettore
    string w_field0, w_field, w_table;
    string w_value, w_value2;
    float w,w2,s;

    //setto tutto il vettore a falso
    for(int r = 0; r < this->records.size(); r++){
        ret.push_back(false);
    }

    for(;query[i] != "WHERE";i++);  //mi posiziono dove inizia la where
    i++; //parola successiva
    for(;i < query.size() && query[i] != "ORDER"; i++){ //leggo tutto il resto della query, fino alla fine delle condizioni di where
        if(query[i] == "NOT"){ //registro il not se è presente
            _not = true;
            i++;
        }

        w_field0 = query[i]; //assegno il campo

        //TODO capisci cosa vuol dire WHERE AGE > 20 AND CUSTOMERS.COUNTRY_ID = COUNTRIES.ID

        //memorizzo in w_table il nome della tabella e w_field il nome del campo
        if(w_field0.find('.')){ //è del tipo tabella.campo
            for(k = 0; w_field0[k] != '.'; k++) //scorre tutti i caratteri e memorizza il nome della tabella
                w_table.push_back(w_field0[k]);
            k++; // salta il punto
            for(; k < w_field0.size() ; k++) //memorizza il campo
                w_field.push_back(w_field0[k]);
        }else{ //caso campo (perchè si compara nella stessa tabella)
            w_table = this->name; //il nome è quello stesso della tabella
            w_field = w_field0;
        }

        if(w_table == this->name){ //stiamo guardando la tabella su cui è stato chiamato il metodo
            i++; //passo all'operatore
            i_dt = where_map.find(query[i]);   //confronto l'operatore con la mappa di simboli (es. =, <, ...)
            if(i_dt == where_map.end()) {
                op = 0;  //operatore non trovato
            }
            else{
                op = i_dt->second;   //operatore trovato, assegno il corrispettivo numero a i
            }

            //TODO da modificare, va a confrontare su diverse tabella
            field_pos = this->get_field_number(w_field); //trovo il campo nella tabella TODO da modificare
            j = distance(this->fields.begin(),field_pos);

            i++;
            w_value = query[i]; //valore da sostituire nella tabella al campo corrispondente
            for(int r = 0; r < this->records.size(); r++){   //scorro le righe della tabella
                //controllo se la condizione si avvera e nel caso aggiorno il vettore rec
                cout << op << " row: " << r <<"\n";
                switch (op){ //TODO controllare il tipo, per tipi date e time è diverso
                    case 1:     //=
                        if(_not){ //se pr
                            if(this->records[r][j] != w_value){
                                cout << "qua\n";
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            cout << this->records[r][j] << "\n";
                            if(this->records[r][j] == w_value){
                                this->set_w(&_and, ret, r);
                            }
                        }

                        break;
                    case 2:     //>
                        w = stof(w_value);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s <= w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s > w){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 3:     //<
                        w = stof(w_value);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s >= w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s < w){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 4:     //>=
                        w = stof(w_value);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s < w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s >= w){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 5:     //<=
                        w = stof(w_value);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s > w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s <= w){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 6:     //<>
                        w = stof(w_value);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s == w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s != w){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 7:     //BETWEEN
                        i++;
                        w_value = query[i];
                        w = stof(w_value);
                        i+=2; //salto AND e assegno il secondo valore
                        w_value2 = query[i];
                        w2 = stof(w_value2);
                        s = stof(this->records[r][j]);
                        if(_not){
                            if(s < w || s > w){
                                this->set_w(&_and, ret, r);
                            }
                            _not = false;
                        }else{
                            if(s > w && s < w2){
                                this->set_w(&_and, ret, r);
                            }
                        }
                        break;
                    case 8:     //LIKE
                        i++;
                        w_value = query[i];
                        //elimino le virgolette
                        w_value.erase(0,1);
                        w_value.erase(w_value.size()-1,1);
                        //eseguo la where
                        if(_not){

                        }else{
                            if(query[i].find("%") == 1) {   //inizia per...
                                w_value.erase(1,1); //tolgo il %

                            }
                            if(query[i].find("%") == 0){ //finisce per
                                w_value.erase(0,1); //tolgo il %
                                if( w_value.find("_") != string::npos){
                                    int q = w_value.find("_");
                                    w_value.erase(q,1);
                                }
                                for(int r = 0; r < this->records.size(); r++){
                                    if(this->records[r][j].find(w_value) != string::npos){ //se effettivamente inizia così
                                        this->set_w(&_and,ret,r);
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        cout << "Un operatore nella query non e' stato riconosciuto. \n";
                        break;
                }
            }
            i+=2;
            if(i < query.size()){
                if(query[i] == "AND"){
                    _and = true;
                    i++;
                }
                if(query[i] == "OR")
                    i++;
            }
        }



    }
    return ret;
}

void Table::set_w(bool *_and, vector<bool> ret, int r){ //TODO da controllare (?)
    if(*_and && ret[r] == true){
        ret[r] = true;   //in quella riga è verificata la condizione TODO non ha tanto senso
        *_and = false;
    }else{
        if(!*_and)   //o c'è OR o nulla, quindi posso assegnare come vero il where
            ret[r] = true;
    }
}

map<string, int> Table::getWhereMap() {
    map<string, int> _map;
    _map["="] = 1;
    _map[">"] = 2;
    _map["<"] = 3;
    _map[">="] = 4;
    _map["<="] = 5;
    _map["<>"] = 6;
    _map["BETWEEN"] = 7;
    _map["LIKE"] = 8;
    return _map;
}

float Table::stof(string basicString) { //converte la stringa in float
    float ret = 0;
    char *cstr = new char[basicString.length() + 1];
    strcpy(cstr, basicString.c_str());
    ret = atof(cstr);
    delete [] cstr;
    return ret;
}

int Table::get_field_index(string name){                                                                                                                                                 
   int i;
   for(i = 0; i < this->fields.size(); i++){
     if( (this->fields.at(i)).name == name)
       return i;   //ha trovato il campo
   }
   return i;   //non ha trovato il campo
}


