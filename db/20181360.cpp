/**************************************************
 * Title: DatabaseSystem - Project 2
 * Summary: blah blah
 *  |Date              |Author             |Version
    |2024-05-30        |HyeongWon Ham      |1.0.0
**************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "mysql.h"
#pragma comment(lib, "libmysql.lib")

const char* host = "localhost"; // change if necessary
const char* user = "root";      // change if necessary
const char* pw = "mysql";       // change if necessary

#define MAX_LEN			13000

int main(void) {
    MYSQL* connection = NULL;
    MYSQL conn;
    MYSQL_RES* sql_result;
    MYSQL_ROW sql_row;
    FILE* fp = fopen("CRUD.txt", "rt");    // open CRUD file.
    char line[MAX_LEN];

    if (mysql_init(&conn) == NULL)
        printf("mysql_init() error!");

    connection = mysql_real_connect(&conn, host, user, pw, NULL, 3306, (const char*)NULL, 0);   // the first NULL can be replaced to an existing db instance.
    if (connection == NULL) {
        printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
        return 1;
    }
    else {
        printf("Connection Succeed\n\n");

        while (fgets(line, sizeof(line), fp) != NULL) {
            if (!strcmp(line, "$$$\n"))      // read lines from CRUD file, '$$$' separates CREATE / DELETE parts.
                break;
            mysql_query(connection, line);
        }

        if (mysql_select_db(&conn, "project")) {
            printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
            return 1;
        }

        /// 모든 테이블 확인
        const char* query = "select * from agents";
        int state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("[AGENTS]\n");
            printf("ID   |이름 |전화번호\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-5s|%s\n", sql_row[0], sql_row[1], sql_row[2]);

            mysql_free_result(sql_result);
        }

        query = "select * from properties";
        state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("\n[PROPERTIES]\n");
            printf("ID   |이름 |구        |동        |상세주소  |유형      |침실|화장실|가격  |크기 |등록일    |상태\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-5s|%-10s|%-10s|%-10s|%-10s|%-4s|%-6s|%-6s|%-s|%-s|%-s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5], sql_row[6], sql_row[7], sql_row[8], sql_row[9], sql_row[10], sql_row[11]);

            mysql_free_result(sql_result);
        }

        query = "select * from buyers";
        state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("\n[BUYERS]\n");
            printf("ID   |이름 |전화번호     |중개인\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-5s|%-10s|%-s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);

            mysql_free_result(sql_result);
        }

        query = "select * from sellers";
        state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("\n[SELLERS]\n");
            printf("ID   |이름 |전화번호     |중개인\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-5s|%-10s|%-s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);

            mysql_free_result(sql_result);
        }

        query = "select * from images";
        state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("\n[IMAGES]\n");
            printf("ID   |유형      |촬영일    |부동산\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-10s|%-10s|%-s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]);

            mysql_free_result(sql_result);
        }

        query = "select * from transactions";
        state = 0;

        state = mysql_query(connection, query);

        if (state == 0) {
            sql_result = mysql_store_result(connection);

            printf("\n[TRANSACTIONS]\n");
            printf("ID   |거래일    |구매인|판매인|중개인|부동산\n");
            while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                printf("%-5s|%-10s|%-6s|%-6s|%-6s|%-s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4], sql_row[5]);

            mysql_free_result(sql_result);
        }

        // 인터페이스
        int type, subtype, k, cnt;
        while (1) {
            printf("\n[SELECT QUERY TYPE]\n");
            printf("1. TYPE 1\n");
            printf("2. TYPE 2\n");
            printf("3. TYPE 3\n");
            printf("4. TYPE 4\n");
            printf("5. TYPE 5\n");
            printf("6. TYPE 6\n");
            printf("7. TYPE 7\n");
            printf("0. QUIT\n");
            type = subtype = k = -1;
            cnt = 0;
            state = 0;
            scanf(" %d", &type);
            switch (type) {
            case 1:
                printf("\n[TYPE 1]\n");
                printf("Find address of homes for sale in the district 'MAPO'.\n");
                query = "SELECT CITY, ADDRESS1, ADDRESS2 FROM PROPERTIES WHERE CITY = 'MAPO'";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("주소\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%s %s %s\n", sql_row[0], sql_row[1], sql_row[2]);
                }
                printf("\n[SELECT QUERY SUBTYPE]\n");
                printf("1. TYPE 1-1\n");
                scanf(" %d", &subtype);
                if (subtype == 1) {
                    printf("\n[TYPE 1-1]\n");
                    printf("Find address of homes for sale in the district 'MAPO' costing between ￦1,000,000,000 and ￦1,500,000,000.\n");
                    query = "SELECT CITY, ADDRESS1, ADDRESS2 FROM PROPERTIES WHERE CITY = 'MAPO' AND PRICE >= 100000 AND PRICE <= 150000 AND STATUS = 'ON SALE'";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("주소\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                            printf("%s %s %s\n", sql_row[0], sql_row[1], sql_row[2]);
                    }
                }
                break;
            case 2:
                printf("\n[TYPE 2]\n");
                printf("Find the address of homes for sale in the 8th school district.\n");
                query = "SELECT CITY, ADDRESS1, ADDRESS2 FROM PROPERTIES WHERE CITY IN ('GANGNAM', 'SEOCHO') AND STATUS = 'ON SALE'";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("주소\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%s %s %s\n", sql_row[0], sql_row[1], sql_row[2]);
                }
                printf("\n[SELECT QUERY SUBTYPE]\n");
                printf("1. TYPE 2-1\n");
                scanf(" %d", &subtype);
                if (subtype == 1) {
                    printf("\n[TYPE 2-1]\n");
                    printf("Then find properties with 4 or more bedrooms and 2 bathrooms.\n");
                    query = "SELECT CITY, ADDRESS1, ADDRESS2 FROM PROPERTIES WHERE CITY IN ('GANGNAM', 'SEOCHO') AND BEDROOM >= 4 AND BATHROOM = 2 AND STATUS = 'ON SALE'";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("주소\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                            printf("%s %s %s\n", sql_row[0], sql_row[1], sql_row[2]);
                    }
                }
                break;
            case 3:
                printf("\n[TYPE 3]\n");
                printf("Find the name of the agent who has sold the most properties in the year 2022 by total won value.\n");
                query = "SELECT NAME\
                        FROM AGENTS\
                        WHERE AGENT_ID = (SELECT AGENT_ID\
                        FROM PROPERTIES P\
                        LEFT JOIN TRANSACTIONS T\
                        ON P.PROPERTY_ID = T.PROPERTY_ID\
                        WHERE YEAR(T.DATE) = 2022 AND STATUS = 'SOLD OUT'\
                        GROUP BY AGENT_ID\
                        ORDER BY SUM(PRICE) DESC\
                        LIMIT 1)";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("이름\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%s\n", sql_row[0]);
                }
                printf("\n[SELECT QUERY SUBTYPE]\n");
                printf("1. TYPE 3-1\n");
                printf("2. TYPE 3-2\n");
                scanf(" %d", &subtype);
                if (subtype == 1) {
                    printf("\n[TYPE 3-1]\n");
                    printf("Then find the top k agents in the year 2023 by total won value.\n");
                    printf("k: ");
                    scanf(" %d", &k);
                    query = "SELECT AGENT_ID\
                        FROM PROPERTIES P\
                        LEFT JOIN TRANSACTIONS T\
                        ON P.PROPERTY_ID = T.PROPERTY_ID\
                        WHERE YEAR(T.DATE) = 2023 AND STATUS = 'SOLD OUT'\
                        GROUP BY AGENT_ID\
                        ORDER BY SUM(PRICE) DESC";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("ID\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
                            printf("%s\n", sql_row[0]);
                            cnt += 1;
                            if (cnt == k)
                                break;
                        }
                    }
                }
                else if (subtype == 2) {
                    printf("\n[TYPE 3-2]\n");
                    printf("And then find the bottom 10%% agents in the year 2021 by total won value.\n");
                    query = "WITH TEMP AS (\
                                SELECT AGENT_ID, SUM(PRICE), PERCENT_RANK() OVER (ORDER BY SUM(PRICE)) AS PERCENTILE_RANK\
                                FROM TRANSACTIONS T\
                                LEFT JOIN PROPERTIES P\
                                ON P.PROPERTY_ID = T.PROPERTY_ID\
                                WHERE YEAR(T.DATE) = 2021\
                                GROUP BY AGENT_ID\
                            )\
                            SELECT AGENT_ID\
                            FROM TEMP\
                            WHERE PERCENTILE_RANK <= 0.1";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("ID\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                            printf("%s\n", sql_row[0]);
                    }
                }
                break;
            case 4:
                printf("\n[TYPE 4]\n");
                printf("For each agent, compute the average selling price of properties sold in 2022, and the average time the property was on the market.\n");
                query = "SELECT AGENT_ID, AVG(PRICE), AVG(DATEDIFF(T.DATE, P.DATE))\
                        FROM TRANSACTIONS T\
                        LEFT JOIN PROPERTIES P\
                        ON T.PROPERTY_ID = P.PROPERTY_ID\
                        WHERE YEAR(T.DATE) = 2022\
                        GROUP BY AGENT_ID";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("중개인|평균 판매액|평균 거래일\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%-6s|%-10s|%-10s\n", sql_row[0], sql_row[1], sql_row[2]);
                }
                printf("\n[SELECT QUERY SUBTYPE]\n");
                printf("1. TYPE 4-1\n");
                printf("2. TYPE 4-2\n");
                scanf(" %d", &subtype);
                if (subtype == 1) {
                    printf("\n[TYPE 4-1]\n");
                    printf("Then compute the maximum selling price of properties sold in 2023 for each agent.\n");
                    query = "SELECT AGENT_ID, MAX(PRICE)\
                            FROM TRANSACTIONS T\
                            LEFT JOIN PROPERTIES P\
                            ON T.PROPERTY_ID = P.PROPERTY_ID\
                            WHERE YEAR(T.DATE) = 2023\
                            GROUP BY AGENT_ID";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("중개인|최고 판매액\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                            printf("%-6s|%-10s\n", sql_row[0], sql_row[1]);
                    }
                }
                else if (subtype == 2) {
                    printf("\n[TYPE 4-2]\n");
                    printf("And then compute the longest time the property was on the market for each agent.\n");
                    query = "SELECT AGENT_ID, MAX(DATEDIFF(T.DATE, P.DATE))\
                            FROM TRANSACTIONS T\
                            LEFT JOIN PROPERTIES P\
                            ON T.PROPERTY_ID = P.PROPERTY_ID\
                            GROUP BY AGENT_ID";
                    state = 0;
                    state = mysql_query(connection, query);
                    if (state == 0) {
                        sql_result = mysql_store_result(connection);
                        printf("중개인|최장 거래일\n");
                        while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                            printf("%-6s|%-10s\n", sql_row[0], sql_row[1]);
                    }
                }
                break;
            case 5:
                printf("\n[TYPE 5]\n");
                printf("Show photos of the most expensive studio, one - bedroom, multi - bedroom apartment(s), and detached house(s), respectively, from the database.\n");
                query = "SELECT I.IMAGE_ID, P.TYPE\
                        FROM IMAGES I\
                        LEFT JOIN PROPERTIES P\
                        ON I.PROPERTY_ID = P.PROPERTY_ID\
                        WHERE (P.TYPE, P.PRICE) IN (SELECT TYPE, MAX(PRICE)\
                                                    FROM PROPERTIES\
                                                    GROUP BY TYPE)";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("이미지|타입\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%-6s|%-10s\n", sql_row[0], sql_row[1]);
                }
                break;
            case 6:
                printf("\n[TYPE 6]\n");
                printf("Record the sale of a property that had been listed as being available. This entails storing the sales price, the buyer, the selling agent, the buyer's agent(if any), and the date.\n");
                query = "SELECT PROPERTY_ID, DATE, BUYER_ID, SELLER_ID, AGENT_ID FROM TRANSACTIONS";
                state = mysql_query(connection, query);
                if (state == 0) {
                    sql_result = mysql_store_result(connection);
                    printf("부동산|날짜      |구매인|판매인|중개인\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%-6s|%-10s|%-6s|%-6s|%-6s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3], sql_row[4]);
                }
                break;
            case 7:
                printf("\n[TYPE 7]\n");
                printf("Add a new agent to the database.\n");
                char q[100], aID[6], aNAME[11], aTLNO[16];
                printf("ID: ");
                scanf("%s", aID);
                printf("이름: ");
                scanf("%s", aNAME);
                printf("전화번호: ");
                scanf("%s", aTLNO);
                sprintf(q, "INSERT INTO AGENTS VALUES('%s', '%s', '%s')", aID, aNAME, aTLNO);
                
                state = mysql_query(connection, q);
                if (state == 0)
                    sql_result = mysql_store_result(connection);
                query = "select * from agents";
                state = mysql_query(connection, query);
                state = 0;
                if (state == 0) {
                    sql_result = mysql_store_result(connection);

                    printf("[AGENTS]\n");
                    printf("ID   |이름 |전화번호\n");
                    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
                        printf("%-5s|%-5s|%-s\n", sql_row[0], sql_row[1], sql_row[2]);

                    mysql_free_result(sql_result);
                }
                break;
            default:
                break;
            }
            if (type == 0)
                break;
        }
        

        // comment out if you want to persist example db instance.
        while (fgets(line, sizeof(line), fp) != NULL)
            mysql_query(connection, line);			// these are DELETEs & DROPs.

        mysql_close(connection);
    }

    return 0;
}
