package main

import (
	"database/sql"
	"encoding/csv"
	"encoding/json"
	"fmt"
	"log"
	"mime"
	"net"
	"net/http"
	"os"
	"sync"
	"text/template"
	"time"

	"github.com/go-chi/chi"
	"github.com/go-chi/chi/middleware"
	_ "modernc.org/sqlite"

	qrcode "github.com/skip2/go-qrcode"
)

type DataRow struct {
	Time string
	Temp float64
	Ph   float64
	Sal  float64
}

var queue [10]DataRow
var RWmutex *sync.RWMutex

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

func SetQueue(db *sql.DB) {
	RWmutex.Lock()
	defer RWmutex.Unlock()
	rows, err := db.Query("select * from AQUAS1 order by timestamp desc limit 10")
	if err != nil {
		return
	}
	index := 0
	for rows.Next() && index < 10 {
		var timestamp int64
		rows.Scan(&timestamp, &queue[index].Temp, &queue[index].Ph, &queue[index].Sal)
		myDate := time.Unix(timestamp, 0)
		queue[index].Time = myDate.Format(time.UnixDate)
		index++
	}
}

func GetQueue() [10]DataRow {
	RWmutex.RLock()
	defer RWmutex.RUnlock()
	return queue
}

// use std encoding/csv
func main() {

	host, _ := os.Hostname()
	addrs, _ := net.LookupIP(host)
	var ip string
	for _, addr := range addrs {
		if ipv4 := addr.To4(); ipv4 != nil {
			//fmt.Println("IPv4: ", ipv4)
			ip = ipv4.String()
		}
	}
	fmt.Println("IPv4: ", ip)
	err := qrcode.WriteFile(fmt.Sprintf("http://%s:3030/all", ip), qrcode.Medium, 256, "./static/qr.png")
	if err != nil {
		log.Fatal(err)
	}

	db, err := sql.Open("sqlite", "./testing.db")
	if err != nil {
		panic(err)
	}
	defer db.Close()

	tmpl := template.Must(template.ParseFiles("./static/layout.html"))
	tmplPosts := template.Must(template.ParseFiles("./static/queue.html"))

	RWmutex = &sync.RWMutex{}
	SetQueue(db)

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
		// for _, val := range records {
		// 	fmt.Println(val)
		// }
		// inserting data to database
		tx, err := db.Begin()
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			w.Write([]byte("fix content"))
		}
		query, err := tx.Prepare("insert into AQUAS1 (Timestamp, temp, ph, salinity) values (?, ?, ?, ?);")
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			w.Write([]byte("fix content"))
		}
		for i := range records {
			_, err := query.Exec(records[i][3], records[i][0], records[i][1], records[i][2])
			if err != nil {
				tx.Rollback()
				w.WriteHeader(http.StatusInternalServerError)
				w.Write([]byte("fix content"))
			}
		}
		tx.Commit()
		SetQueue(db)
		w.WriteHeader(http.StatusAccepted)
		w.Write([]byte("recieved"))
	})
	r.Get("/recent", func(w http.ResponseWriter, r *http.Request) {
		tmplPosts.Execute(w, nil)
	})
	r.Get("/queue", func(w http.ResponseWriter, r *http.Request) {
		tmpq := GetQueue()
		response, _ := json.Marshal(tmpq)
		w.Header().Set("Content-Type", "application/json")
		w.WriteHeader(http.StatusOK)
		w.Write(response)
	})
	r.Get("/revertToBasic", func(w http.ResponseWriter, r *http.Request) {
		db.Exec("DELETE FROM AQUAS1 where timestamp > 1707805737;")
		w.WriteHeader(200)
		w.Write([]byte("all done"))
	})
	fmt.Println("http://127.0.0.1:3030/")
	http.ListenAndServe(":3030", r)

}
