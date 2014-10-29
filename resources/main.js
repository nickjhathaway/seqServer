// based on example from http://thecodeplayer.com/walkthrough/html5-game-tutorial-make-a-snake-game-using-html5-canvas-jquery

$(document).ready(function(){
	function tabulate(data, columns, divId) {
		//adapted from http://stackoverflow.com/questions/9268645/creating-a-table-linked-to-a-csv-file
	    var table = d3.select(divId).append("table"),
	        thead = table.append("thead"),
	        tbody = table.append("tbody");
	
	    // append the header row
	    thead.append("tr")
	        .selectAll("th")
	        .data(columns)
	        .enter()
	        .append("th")
	            .attr("style", "font-weight: bold; padding: 2px 4px;")
	            .text(function(column) { return column; });
	
	    // create a row for each object in the data
	    var rows = tbody.selectAll("tr")
	        .data(data)
	        .enter()
	        .append("tr");
	
	    // create a cell in each row for each column
	    var cells = rows.selectAll("td")
	        .data(function(row) {
	            return columns.map(function(column) {
	                return {column: column, value: row[column]};
	            });
	        })
	        .enter()
	        .append("td")
	            .attr("style", "padding: 2px 4px;")
	            .text(function(d) { return d.value; });
	    
	    return table;
	}
	function drawCircle(x, y, radius, color, borderColor){
        context.beginPath();
        context.arc(x, y, radius, 0, 2 * Math.PI, false);
        context.fillStyle = color;
        context.fill();
        context.lineWidth = 5;
        context.strokeStyle = borderColor;
        context.stroke();
    }

    function drawLine(sx, sy, ex, ey, width){
        context.beginPath();
        context.moveTo(sx, sy);
        context.lineTo(ex, ey, width);
        context.stroke();
    }

    function ajax(url, func){
        $.ajax({ url: url, dataType: 'json', async: false,
            success: function(ct){ func(ct); } });
    }

    function ajaxAsync(url, func){
        $.ajax({ url: url, dataType: 'json', async: true,
                 success: function(ct){ func(ct); } });
    }

	var mNames;
	ajax("/ssv/mipNames", function(mn){ mNames = mn; });
	console.log(mNames);
	/*d3.select("body")
	    .data(mNames)
	  	.enter()
	  		.append("p");*/
	d3.select("body").selectAll("p")
	    .data(mNames)
	  	.enter()
	  		.append("p")
  			.append("a")
  				.attr("href", function(d){console.log(d);return "/ssv/info/" + d;})
  				.html(function(d){return d;});
	//init();




});
