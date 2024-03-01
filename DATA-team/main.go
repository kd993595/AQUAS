package main

import (
	"database/sql"
	"encoding/csv"
	"fmt"
	"mime"
	"net/http"
	"text/template"
	"time"

	"github.com/go-chi/chi"
	"github.com/go-chi/chi/middleware"
	_ "modernc.org/sqlite"
)

type DataRow struct {
	Time string
	Temp float64
	Ph   float64
	Sal  float64
}

func SelectTable(db *sql.DB) ([]DataRow, error) {
	rows, err := db.Query("select * from AQUAS1;")
	if err != nil {
		return nil, err
	}
	allrows := make([]DataRow, 0, 100)
	for rows.Next() {
		var row DataRow
		var timestamp int64
		rows.Scan(&timestamp, &row.Temp, &row.Ph, &row.Sal)
		myDate := time.Unix(timestamp, 0)
		row.Time = myDate.Format(time.UnixDate)
		allrows = append(allrows, row)
	}
	return allrows, nil
}

// use std encoding/csv
func main() {

	db, err := sql.Open("sqlite", "./testing.db")
	if err != nil {
		panic(err)
	}
	defer db.Close()

	tmpl := template.Must(template.ParseFiles("./static/layout.html"))

	//SetupDB(db)

	mime.AddExtensionType(".js", "application/javascript")
	mime.AddExtensionType(".css", "text/css")

	r := chi.NewRouter()
	r.Use(middleware.Logger)

	fs := http.FileServer(http.Dir("static"))
	r.Handle("/static/*", http.StripPrefix("/static/", fs))

	r.Get("/", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("pinged!"))
	})
	r.Get("/all", func(w http.ResponseWriter, r *http.Request) {

		data, err := SelectTable(db)
		if err != nil {
			w.WriteHeader(http.StatusForbidden)
			w.Write([]byte("selecting table values failed"))
			return
		}

		tmpl.Execute(w, data)
	})
	r.Post("/upload", func(w http.ResponseWriter, r *http.Request) {

		// for k, v := range r.Header {
		// 	fmt.Printf("Header field %s, Value %q\n", k, v)
		// }

		if r.Header.Get("Content-type") != "text/csv" {
			w.WriteHeader(http.StatusUnsupportedMediaType)
			w.Write([]byte("fix content type"))
			return
		}
		apikey := r.Header.Get("Authorization")
		_ = apikey
		rc := http.MaxBytesReader(w, r.Body, 2<<19) //1 megabyte limit

		csvreader := csv.NewReader(rc)

		records, err := csvreader.ReadAll()
		if err != nil {
			w.WriteHeader(http.StatusNotAcceptable)
			w.Write([]byte("fix content"))
			return
		}
		//* for printing values
		for _, val := range records {
			fmt.Println(val)
		}
		//* inserting data to database
		// tx, err := db.Begin()
		// if err != nil {
		// 	w.WriteHeader(http.StatusInternalServerError)
		// 	w.Write([]byte("fix content"))
		// }
		// query, err := tx.Prepare("insert into AQUAS1 (Timestamp, temp, ph, salinity) values (?, ?, ?, ?);")
		// if err != nil {
		// 	w.WriteHeader(http.StatusInternalServerError)
		// 	w.Write([]byte("fix content"))
		// }
		// for i := range records {
		// 	_, err := query.Exec(records[i][3], records[i][0], records[i][1], records[i][2])
		// 	if err != nil {
		// 		tx.Rollback()
		// 		w.WriteHeader(http.StatusInternalServerError)
		// 		w.Write([]byte("fix content"))
		// 	}
		// }
		// tx.Commit()
		w.WriteHeader(http.StatusAccepted)
		w.Write([]byte("recieved"))
	})
	fmt.Println("http://127.0.0.1:3030/")
	http.ListenAndServe(":3030", r)

}
