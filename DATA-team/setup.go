package main

import (
	"database/sql"
	"encoding/csv"
	"fmt"
	"os"
	"time"
)

func CreateTables(db *sql.DB) {
	db.Exec(`create table if not exists AQUAS1(
		Timestamp int primary key,
		temp float,
		ph float,
		salinity float
	);`)
}

func InsertMockData(db *sql.DB, path string) error {
	f, err := os.Open(path)
	if err != nil {
		return err
	}
	defer f.Close()

	csvreader := csv.NewReader(f)
	records, err := csvreader.ReadAll()
	if err != nil {
		return err
	}
	// for i := range records {
	// 	fmt.Println(records[i])
	// }

	tx, err := db.Begin()
	if err != nil {
		return err
	}
	query, err := tx.Prepare("insert into AQUAS1 (Timestamp, temp, ph, salinity) values (?, ?, ?, ?);")
	if err != nil {
		return err
	}
	for i := range records {
		_, err := query.Exec(records[i][3], records[i][0], records[i][1], records[i][2])
		if err != nil {
			tx.Rollback()
			return err
		}
	}
	tx.Commit()
	return nil
}
func ReadTable(db *sql.DB) {
	rows, err := db.Query("select * from AQUAS1;")
	if err != nil {
		panic(err)
	}
	for rows.Next() {
		var Timestamp int
		var temp float64
		var ph float64
		var salinity float64
		rows.Scan(&Timestamp, &temp, &ph, &salinity)
		myDate := time.Unix(int64(Timestamp), 0)
		fmt.Printf("%s; %f; %f; %f\n", myDate.Format(time.UnixDate), temp, ph, salinity)
	}
}

func SetupDB(db *sql.DB) {
	CreateTables(db)
	err := InsertMockData(db, "./MOCK_DATA.csv")
	if err != nil {
		panic(err)
	}
	ReadTable(db)
}
