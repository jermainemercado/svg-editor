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

            //console.log(data); 
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


  
//let fileLog = document.getElementById(fileLog);

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
    svgImage.setAttribute("colspan", "3");
    defaultImage.parentNode.replaceChild(svgImage,defaultImage);

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/parseTitleDesc',   //The server endpoint we are connecting to
        data: {
            sendFile: fileName
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            if (data.title.length == 0) {
                data.title = "No Title Available."
            }
            if (data.desc.length == 0) {
                data.desc = "No Description Available."
            }
            data.title = data.title.replace(/~/g, '\n');
            data.title = data.title.replace(/\^/g, '"');
            data.desc = data.desc.replace(/~/g, '\n'); 
            data.desc = data.desc.replace(/\^/g, '"');
            $(".svgPanel #titleDesc").empty();
            let titleDescRow = "<tr><th id=\"title\" colspan=\"1\">Title</th>"
                            + "<th id=\"desc\" colspan=\"3\">Description</th>"
                            + "<tr><td colspan=\"1\">" + data.title
                            + "<td colspan=\"3\">" + data.desc
                            + "</tr>";
            $(".svgPanel #titleDesc").append(titleDescRow);
            // console.log(data); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getComponents',
        data: {
            sendFile: fileName
        },
        success: function(data) {
            $(".svgPanel #components").empty();
            let headerRow = "<tr><th id=\"component\">Component</th>"
                            + "<th id=\"summary\">Summary</th>"
                            + "<th id=\"otherAttr\">Other Attributes</th>";
            $(".svgPanel #components").append(headerRow);
            for (let i = 0; i < data.rectangles.length ; i++) {
                let component = "<tr><td id=\"rectangle" + (i-(-1)) + "\">" 
                                + "Rectangle " + (i-(-1)) + "</td>"
                                + "<td id=\"rectangleSummary\"" + (i-(-1)) + "\">" 
                                + "Upper Left Corner: x = " 
                                + data.rectangles[i].x 
                                + data.rectangles[i].units + ", y = " 
                                + data.rectangles[i].y 
                                + data.rectangles[i].units + "<br>Width: " 
                                + data.rectangles[i].w 
                                + data.rectangles[i].units + ", Height: " 
                                + data.rectangles[i].h 
                                + data.rectangles[i].units 
                                + "<td id=\"rectangleOtherAttr\"" + (i-(-1)) + "\>"
                                + data.rectangles[i].numAttr
                                + "<td></tr>";
                $(".svgPanel #components").append(component);
            }
            for (let i = 0; i < data.circles.length ; i++) {
                let component = "<tr><td id=\"circle" + (i-(-1)) + "\">" 
                                + "circle " + (i-(-1)) + "</td>"
                                + "<td id=\"circleSummary\"" + (i-(-1)) + "\">" 
                                + "Center: x = " 
                                + data.circles[i].cx 
                                + data.circles[i].units + ", y = " 
                                + data.circles[i].cy 
                                + data.circles[i].units + "<br>Radius: " 
                                + data.circles[i].r 
                                + data.circles[i].units 
                                + "<td id=\"circleOtherAttr\"" + (i-(-1)) + "\>"
                                + data.circles[i].numAttr
                                + "<td></tr>";
                $(".svgPanel #components").append(component);
            }
            for (let i = 0; i < data.paths.length ; i++) {
                let component = "<tr><td id=\"path" + (i-(-1)) + "\">" 
                                + "path " + (i-(-1)) + "</td>"
                                + "<td id=\"pathSummary\"" + (i-(-1)) + "\">" 
                                + "Path Data: " 
                                + data.paths[i].d 
                                + "<td id=\"pathOtherAttr\"" + (i-(-1)) + "\>"
                                + data.paths[i].numAttr
                                + "<td></tr>";
                $(".svgPanel #components").append(component);
            }
            for (let i = 0; i < data.groups.length ; i++) {
                let component = "<tr><td id=\"group" + (i-(-1)) + "\">" 
                                + "group " + (i-(-1)) + "</td>"
                                + "<td id=\"groupSummary\"" + (i-(-1)) + "\">"
                                + data.groups[i].children
                                + " child elements"
                                + "</td>"
                                + "<td id=\"groupOtherAttr\"" + (i-(-1)) + "\>"
                                + data.groups[i].numAttr
                                + "<td></tr>";
                $(".svgPanel #components").append(component);
            }
        }
    })
}

