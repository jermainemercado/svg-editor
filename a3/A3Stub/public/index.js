// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getSVGFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
                      
           //Create FileLog Panel.
            createFileLogPanel(data);

            //Add Dropdown For svgFiles.
            createDropDown(data);

            
            //Add Event Listeners for Each DropDown.

            console.log(data); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});


  
let fileLog = document.getElementById(fileLog);

function createFileLogPanel(data) {
    $(".filelog tbody").remove();
    if (data.length == 0) {
        let table = "<tbody><tr>"
        +"<td width=\"100%\">NO FILES</td>";
        $(".filelog").append(table);
    }
     for (let i = 0; i < data.length; i++) {
         let fileName = data[i].file.substring(10, data[i].file.length);
         let table = "<tbody><tr>"
         +"<td><img src=\"" + data[i].file + "\" id=\"imageView\"></td>"
         +"<td><a href=\"" + data[i].file + "\">"+ fileName + "</a></td>"
         +"<td>" + data[i].size + "KB</td>"
         +"<td>" + data[i].numRect + "</td>"
         +"<td>" + data[i].numCirc + "</td>"
         +"<td>" + data[i].numPaths + "</td>"
         +"<td>" + data[i].numGroups + "</td>"
         $(".filelog").append(table);
     }
}

function createDropDown(data) {
    for (let i = 0; i < data.length; i++) {
        let fileName = data[i].file.substring(10, data[i].file.length);
        let dropDownOptions = "<a id=\"" + fileName + "\" class=\"dropdown-item\" onclick=\"selectImage(this.id)\">" + fileName + "</a>"
        $("#svgDropdown div").append(dropDownOptions);
    }
}

function selectImage(fileName) {
    let fileNameWD = "./uploads/" + fileName;
    let defaultImage = document.querySelector("#svgDisplay");
    let svgImage = document.createElement("img");
    svgImage.setAttribute("src", fileNameWD);
    svgImage.setAttribute("id", "svgDisplay");

    defaultImage.parentNode.replaceChild(svgImage,defaultImage);
}

