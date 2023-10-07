from typing import Union
from pydantic import BaseModel
from fastapi import FastAPI
import databases
import sqlalchemy
from sqlalchemy.sql import text

DATABASE_URL = "sqlite:///./tutorial.db"

database = databases.Database(DATABASE_URL)
metadata = sqlalchemy.MetaData()
waterdata = sqlalchemy.Table(
    "Persons",
    metadata,
    sqlalchemy.Column("ID", sqlalchemy.Integer, primary_key=True),
    sqlalchemy.Column("Time", sqlalchemy.Integer),
    sqlalchemy.Column("Temp", sqlalchemy.DECIMAL),
    sqlalchemy.Column("pH", sqlalchemy.DECIMAL),
    sqlalchemy.Column("Salinity", sqlalchemy.DECIMAL),
)

engine = sqlalchemy.create_engine(
    DATABASE_URL, connect_args={"check_same_thread": False}
)
#metadata.create_all(engine)


app = FastAPI()

class Item(BaseModel):
    id: int
    data: str
    token: Union[str, None] = None

class ItemIn(BaseModel):
    data: str
    token: Union[str, None] = None

@app.on_event("startup")
async def startup():
    await database.connect()

@app.on_event("shutdown")
async def shutdown():
    await database.disconnect()

@app.get("/")
def read_root():
    return {"Hello": "World"}


@app.get("/all/", )
async def read_all_data():
    query = waterdata.select()
    data = await database.fetch_all(query)
    return {"data":data}

@app.post("/water/")
async def create_newentry(newdata: ItemIn):
    lines = newdata.data.splitlines()
    headstring = "INSERT INTO Persons (Time,Temp,pH,Salinity) Values "
    valuesstring = ""
    #print(lines)
    for line in lines:
        values = line.rstrip(";").split(",")
        valuesstring += f"({values[0]},{values[1]},{values[2]},{values[3]}),"
    valuesstring = valuesstring[:-1]
    query = text(headstring+valuesstring)
    print(query)
    last_record_id = await database.execute(query)
    return {"msg":"success","id":last_record_id}