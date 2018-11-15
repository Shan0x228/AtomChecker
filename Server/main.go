package main

import (
	"html/template"
	"log"
	"net/http"
)

type ResultData struct {
	ResData string
}

func login(w http.ResponseWriter, r *http.Request) {
	tmpl := template.Must(template.ParseFiles("login.gtpl"))
	data := ResultData{}

	if r.Method == http.MethodPost {
		r.ParseForm()

		// logic part of log in
		//fmt.Println("username:", r.Form["username"][0])
		//fmt.Println("password:", r.Form["password"][0])

		if r.Form["username"][0] == "S0M3_USER" && r.Form["password"][0] == "S0M3_PdSSW0RD" {
			data.ResData = "Loggined!"
		} else {
			data.ResData = "Bad login or password!"
		}
	}
	tmpl.Execute(w, data)
}

func main() {
	http.HandleFunc("/", login)

	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		log.Fatal("ListenAndServe: ", err)
	}
}
