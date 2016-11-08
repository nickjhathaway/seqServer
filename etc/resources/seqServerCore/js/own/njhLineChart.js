/**
 * 
 */


d34.njh = d34.njh || {};

d34.njh.LineChart = function() {			
    var margin = {top: 20, right: 20, bottom: 30, left: 40},
	    width = 960,
	    height = 500,
	    data,
	    chartWidth, chartHeight,
	    xScale, yScale, color,
	    xAxis, yAxis,
	    svg,
	    line = d34.line()
		    .x(function(d,i) { return xScale(i); })
		    .y(function(d) { return yScale(d); }),
		  yValue = "quals",
		  nameValue = "name",
			getMaxX = function(inputData){ return d34.max(inputData, function(d) { return d[yValue].length;   }); },
		 	getMinY = function(inputData){ return d34.min(inputData, function(q) { return d34.min(q[yValue]); }); },
		 	getMaxY = function(inputData){ return d34.max(inputData, function(q) { return d34.max(q[yValue]); }); };

    /**
     * This function creates the graph using the selection as container
     * @param  {d34Selection} _selection A d34 selection that represents
     * the container(s) where the chart(s) will be rendered
     */
    function chart(_selection){
        /* @param {object} _data The data to attach and generate the chart */
        _selection.each(function(_data){
        	
            chartWidth = width - margin.left - margin.right;
            chartHeight = height - margin.top - margin.bottom;
            if(data){
            	console.log(data);
            }
            data = _data;
            
        	var curUid = 0;
        	data.forEach(function(d){
        		 d.uid = curUid;
        		 ++curUid;
        	});
            buildScales();
            buildAxis();
            buildSVG(this);
            drawLines();
            drawAxis();
        });
    }
    /**
     * Creates the d34 x and y axis, setting orientations
     * @private
     */
    function buildAxis(){
        xAxis = d34.axisBottom(xScale).tickFormat(function(e){
	         if(Math.floor(e) != e)
	         {
	             return;
	         }

	         return e;
	     });
        yAxis = d34.axisLeft(yScale);
    }
    
    /**
     * @description
     * Draws the x and y axis on the svg object within their
     * respective groups
     * @private
     */
    function drawAxis(){
        svg.select('.x-axis-group.axis')
            .attr('transform', 'translate(0,' + chartHeight + ')')
            .call(xAxis);

        svg.select('.y-axis-group.axis')
            .call(yAxis);
    }
    
    /**
     * Builds containers for the chart, the axis and a wrapper for all of them
     * Also applies the Margin convention
     * @private
     */
    function buildContainerGroups(){
        var container = svg.append('g')
	        .classed('container-group', true)
	        .attr("transform",'translate(' + margin.left + ',' + margin.top + ')');
        container.append('g')
        	.classed('chart-group', true)
        	.append("rect")
				.attr("class", "rect-wrap")
				.attr("x", 1)
				.attr("y", 1)
				.attr("fill", "#FFF");
        
        container.append('g').classed('x-axis-group axis', true);
        container.append('g').classed('y-axis-group axis', true);
    }
    
    /**
     * @param  {HTMLElement} container DOM element that will work as the container of the graph
     * @private
     */
    function buildSVG(container){
        if (!svg) {
            svg = d34.select(container)
                .append('svg')
                .classed('bar-chart', true);
            buildContainerGroups();
        }
        svg.transition()
        	.attr("width", width + margin.left + margin.right)
        	.attr("height", height + margin.top + margin.bottom);;
    }
    
    /**
     * Creates the x and y scales of the graph
     * @private
     */
    function buildScales(){
 		var maxX = getMaxX(data);
	 	var minY = getMinY(data);
	 	var maxY = getMaxY(data);

		color = d34.scaleOrdinal(d34.schemeAccent)
				.domain(data.map(function(c) { return c[nameValue]; }));
	 
        xScale = d34.scaleLinear()
            .domain([0,  maxX])
            .range([0, chartWidth]);

        yScale = d34.scaleLinear()
            .domain([d34.min([0,  minY]),d34.max([42, maxY])])
            .range([chartHeight, 0]);
    }
    
    /**
     * Draws the bar elements within the chart group
     * @private
     */
    function drawLines(){
    	var g = svg.select(".chart-group");
    	//re-adjust dims of the background rec
    	if(data.length > 0){
    		g.select(".rect-wrap")
			.attr("height", yScale(getMinY(data)))
			.attr("width", xScale(getMaxX(data)))
    	}
		var lineGroups = g.selectAll(".qual-line")
				.data(data);
		// Exit
	    lineGroups.exit()
	    		.transition(500)
					.style("opacity", 0)
					.remove();
	    
	    lineGroups = lineGroups.enter()
				.append("g")
					.attr("class", function(d){ return "qual-line " + "qual-" + d.uid; })
	    		.merge(lineGroups)
		    		.style("fill", function(d){ return color(d[nameValue]);})
		    		.style("stroke", function(d){ return color(d[nameValue]);});
	    //add paths and labels for entering groupings;
		g.on("mousemove", function(){
			/*
			var xPos = Math.floor(xScale.invert(d34.mouse(this)[0]));
			tooltip.style("left", d34.mouse(svg.node())[0] + "px").style("top", d34.mouse(svg.node())[1] + "px");
			tooltip.html(quals.map(function(d){ 
					 if(xPos < d[yValue].length){
						return {"name":d[nameValue] , "value":d[yValue][xPos]};
					 }
				}));*/
			})
		//line setup
		var allPaths = lineGroups.selectAll(".line")
				.data(function(d){ return [d[yValue]];});
		allPaths.exit()
			.transition(500)
			.style("opacity", 0)
			.remove();
		
		allPaths.enter()
				.append("path")
				.attr("class", "line")
				.style("stroke-width", "2px")
				.style("fill", "none")
			.merge(allPaths)
			.attr("d", function(d) { return line(d); })
			.style("opacity", 0)
			.transition(500)
			.style("opacity", 0.60);
		//dot setup
		var dots = lineGroups.selectAll(".dot")
			.data(function(d){ return d[yValue]});
		dots.exit()
    		.transition(500)
			.style("opacity", 0)
			.remove();
		
		dots.enter()
			.append("circle")
				.attr("class", "dot")
				.attr("r", 3.5)
			.merge(dots)
				.style("opacity", 0)
			    .attr("cx", function(d, i) { return xScale(i); })
			    .attr("cy", function(d, i) { return yScale(d); })
			    .transition(500)
				.style("opacity", 0.60);
    }
	
	/**
	 * Create an accessor function for the given attribute
	 * @param  {string} attr Public attribute name
	 * @return {func}      Accessor function
	 */
	function generateAccessor(attr) {
	    /**
	     * Gets or Sets the public attribute of the chart
	     * @param  {object} value Attribute object to get/set
	     * @return { attr | chart} Current attribute value or Chart module to chain calls
	     */
	    function accessor(value) {
	        if (!arguments.length) { return publicAttributes[attr]; }
	        publicAttributes[attr] = value;
	        return chart;
	    }
	    return accessor;
	}
	
	return chart;
}

