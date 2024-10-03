var globalQueue = [];
var flipColor = true;

document.addEventListener("DOMContentLoaded", (event) => {
    CollapseInit();
    fetch('/queue')
        .then(data => {
            return data.json();
        })
        .then(post => {
            post.forEach(element => {
                globalQueue.push(element)
                //console.log(element);
            });
            AppendEntries(globalQueue);
        });
    setTimeout(refresh, 15000);
});

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function AppendEntries(queue) {
    const mainbody = document.getElementById("main");

    for (let index = queue.length - 1; index >= 0; index--) {
        const element = queue[index];
        const newDiv = document.createElement("div");
        newDiv.className = "entry";
        const classColor = (flipColor)?"white":"gray";
        flipColor = !flipColor;
        newDiv.innerHTML = `
                <table>
                    <tbody>
                        <tr class="${classColor}">
                            <td>${element.Time}</td>
                            <td>${element.Temp}</td>
                            <td>${element.Ph}</td>
                            <td>${element.Sal}</td>
                        </tr>
                    </tbody>
                </table>`
        mainbody.insertBefore(newDiv, mainbody.firstChild)
        // await sleep(1000);
    }
}

function refresh() {
    fetch('/queue')
        .then(data => {
            return data.json();
        })
        .then(post => {
            let newqueue = [];
            let nums = 0;
            post.forEach(element => {
                if (!checkElementExists(element)) {
                    newqueue.push(element);
                    globalQueue.push(element);
                    nums += 1;
                }
            });
            console.log(newqueue);
            const mainbody = document.getElementById("main");
            for (let i = 0; i < nums; i++) {
                mainbody.removeChild(mainbody.lastChild);
                mainDequeue();
            }
            AppendEntries(newqueue);
        });
    setTimeout(refresh, 5000);
}

function mainDequeue() {
    if (globalQueue.length < 100) {
        return
    }
    globalQueue.shift();
}

function checkElementExists(element) {
    for (let i = 0; i < globalQueue.length; i++) {
        const e = globalQueue[i];
        if (e.Time.localeCompare(element.Time) == 0) {
            return true;
        }
    }
    return false;
}

/*
* Collapsible code
*/
function CollapseInit() {
    var coll = document.getElementsByClassName("collapsible");
    var i;

    for (i = 0; i < coll.length; i++) {
        coll[i].addEventListener("click", function () {
            this.classList.toggle("active");
            var content = this.nextElementSibling;
            if (content.style.maxHeight) {
                content.style.maxHeight = null;
            } else {
                content.style.maxHeight = content.scrollHeight + "px";
            }
        });
    }
}
