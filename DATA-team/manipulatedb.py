import sqlite3

con = sqlite3.connect("tutorial.db")
cur = con.cursor()

cur.execute("""CREATE TABLE IF NOT EXISTS Persons (
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    Time int,
    Temp decimal(6,2),
    pH decimal(4,2),
    Salinity decimal(6,2)
)""")