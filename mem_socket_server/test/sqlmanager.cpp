#include "../include/usrinfo/sqlmanager.h"
#include "../include/config.h"
#include <assert.h>
#include <iostream>

int main () {
    SqlManager* sqm = new SqlManager();
    sqm->connect(SQL_IP, SQL_PORT, SQL_USERNAME, SQL_PASSWORD);
    sqm->selectDB(SQL_DBNAME);
    auto ret1 = sqm->QueryGetRes("SELECT * FROM mem_socket_login_infos");
    sqm->query("INSERT INTO mem_socket_login_infos (username, password) VALUES ('bibibi1', 'bibibi2')");
    auto ret2 = sqm->QueryGetRes("SELECT * FROM mem_socket_login_infos");
    sqm->query("DELETE FROM mem_socket_login_infos WHERE username='bibibi1' AND password='bibibi2'");

    assert(ret1.size() == ret2.size() - 1);

    int i = 0, j = 0, has_bibibi = false;
    while (i < ret1.size() && j < ret2.size()) {
        if (ret2[j].first == "bibibi1") {
            has_bibibi = true;
            j ++;
        }
        assert(ret1[i] == ret2[j]);
        i ++; j ++;
    }
    if (ret2.back().first == "bibibi1") 
        has_bibibi = true;

    assert(has_bibibi == true);
}