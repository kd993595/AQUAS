//mkase button group
var alldata = []
var curPage = 1;
var buffersize;

document.addEventListener('DOMContentLoaded', function() {
    let table = document.getElementById("myTable")
    let rows = table.getElementsByTagName('tr');
    for (let i = 0; i < rows.length; i++) {
 
        // Get each column data
        let cols = rows[i].querySelectorAll('td,th');
 
        // Stores each csv row data
        let csvrow = [];
        for (let j = 0; j < cols.length; j++) {
 
            // Get the text data of each cell of
            // a row and push it to csvrow
            csvrow.push(cols[j].innerHTML);
        }
 
        // Combine each column value with comma
        alldata.push(csvrow);
    }
    alldata.shift();
    alldata.pop();
    //console.log(alldata)
    curPage = 1;
    buffersize = 25;
    tableToBuffer(1);
    
});

function tableToBuffer(page){
    let buffer = buffersize;
    console.log(buffer);


    let body = document.getElementById("body");
    body.innerHTML = "";
    newhtml = "";
    let upperbound = min(page*buffer,alldata.length)
    for(let i=(page-1)*buffer;i<upperbound;i++){
        // Get each column data
        let cols = alldata[i];
 
        // Stores each csv row data
        let csvrow = "<tr>";
        for (let j = 0; j < cols.length; j++) {
            csvrow=csvrow.concat(`<td>${cols[j]}</td>`)
        }
        csvrow=csvrow.concat("</tr>");
        newhtml = newhtml.concat(csvrow);
    }
    body.innerHTML = newhtml;

    let quotient = Math.ceil(alldata.length / buffer);
    let pagediv = document.getElementById("pageDiv");
    let start = (curPage-1)*buffersize+1;
    let end = min(curPage*buffersize,alldata.length);
    pagediv.innerText = `showing ${start} to ${end} of ${alldata.length} entries`;
}

function IncreasePage(){
    let totalPages = Math.ceil(alldata.length / buffersize);
    if(curPage+1>totalPages){
        return;
    }
    tableToBuffer(curPage+1);
    curPage += 1;
    let pagediv = document.getElementById("pageDiv");
    let start = (curPage-1)*buffersize+1;
    let end = min(curPage*buffersize,alldata.length);
    pagediv.innerText = `showing ${start} to ${end} of ${alldata.length} entries`;
}

function DecreasePage(){
    let totalPages = Math.ceil(alldata.length / buffersize);
    if(curPage-1<1){
        return;
    }
    tableToBuffer(curPage-1);
    curPage -= 1;
    let pagediv = document.getElementById("pageDiv");
    let start = (curPage-1)*buffersize+1;
    let end = min(curPage*buffersize,alldata.length);
    pagediv.innerText = `showing ${start} to ${end} of ${alldata.length} entries`;
}

function JumpStartPage(){
    let totalPages = Math.ceil(alldata.length / buffersize);
    if(curPage == 1){
        return;
    }
    tableToBuffer(1);
    curPage = 1;
    let pagediv = document.getElementById("pageDiv");
    let start = (curPage-1)*buffersize+1;
    let end = min(curPage*buffersize,alldata.length);
    pagediv.innerText = `showing ${start} to ${end} of ${alldata.length} entries`;
}

function JumpEndPage(){
    let totalPages = Math.ceil(alldata.length / buffersize);
    if(curPage == totalPages){
        return;
    }
    tableToBuffer(totalPages);
    curPage = totalPages;
    let pagediv = document.getElementById("pageDiv");
    let start = (curPage-1)*buffersize+1;
    let end = min(curPage*buffersize,alldata.length);
    pagediv.innerText = `showing ${start} to ${end} of ${alldata.length} entries`;
}

function changeBufferSize(){
    let newbuffer = document.getElementById("buffer-select");
    buffersize = parseInt(newbuffer.value);
    curPage = 1;
    tableToBuffer(1);
}

function min(a,b){
    if(a < b){
        return a;
    }
    return b
}

function tableToCSV() {
 
    // Variable to store the final csv data
    let csv_data = [];

    // Get headers
    let table = document.getElementById("myTable")
    let header = table.getElementsByTagName('tr');
    let cols = header[0].querySelectorAll('th');
    let csvrow = [];
    for (let j = 0; j < cols.length; j++) {
 
        // Get the text data of each cell of
        // a row and push it to csvrow
        csvrow.push(cols[j].innerHTML);
    }
    csv_data.push(csvrow.join(","));
 
    // Get each row data
    let rows = alldata
    for (let i = 0; i < rows.length; i++) {
 
        // Get each column data
        let cols = rows[i]
 
        // Stores each csv row data
        let csvrow = [];
        for (let j = 0; j < cols.length; j++) {
 
            // Get the text data of each cell of
            // a row and push it to csvrow
            csvrow.push(cols[j]);
        }
 
        // Combine each column value with comma
        csv_data.push(csvrow.join(","));
    }
    // Combine each row data with new line character
    csv_data = csv_data.join('\n');
 
    downloadCSVFile(csv_data);
}


function downloadCSVFile(csv_data) {
 
    // Create CSV file object and feed our
    // csv_data into it
    CSVFile = new Blob([csv_data], { type: "text/csv" });
 
    // Create to temporary link to initiate
    // download process
    let temp_link = document.createElement('a');
 
    // Download csv file
    temp_link.download = "alldata.csv";
    let url = window.URL.createObjectURL(CSVFile);
    temp_link.href = url;
 
    // This link should not be displayed
    temp_link.style.display = "none";
    document.body.appendChild(temp_link);
 
    // Automatically click the link to trigger download 
    temp_link.click();
    document.body.removeChild(temp_link);
}