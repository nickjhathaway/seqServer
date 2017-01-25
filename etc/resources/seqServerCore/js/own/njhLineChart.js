/**
 * 
 */


d34.njh = d34.njh || {};

d34.njh.LineChart = function() {	
//Attributes that will be configurable from the outside
	var publicAttributes = {
	    margin: {
	        top: 20,
	        right: 20,
	        bottom: 30,
	        left: 40
	    },
	    width: 960,
	    height: 500,
		  yValue :"quals",
		  nameValue : "name",
		  xScale:null,
		  yScale:null,
		  color:null,
		  tooltip:null,
		  tooltipTab:null,
		  selectedPos:null,
		  selectline:null
	};


	   var data, chartWidth, chartHeight,
	    
	    xAxis, yAxis,
	    svg,
	    line = d34.line()
		    .x(function(d,i) { return publicAttributes.xScale(i); })
		    .y(function(d) { return publicAttributes.yScale(d); }),

			getMaxX = function(inputData){ return d34.max(inputData, function(d) { return d[publicAttributes.yValue].length;   }); },
		 	getMinY = function(inputData){ return d34.min(inputData, function(q) { return d34.min(q[publicAttributes.yValue]); }); },
		 	getMaxY = function(inputData){ return d34.max(inputData, function(q) { return d34.max(q[publicAttributes.yValue]); }); };

    /**
     * This function creates the graph using the selection as container
     * @param  {d34Selection} _selection A d34 selection that represents
     * the container(s) where the chart(s) will be rendered
     */
    function chart(_selection){
        /* @param {object} _data The data to attach and generate the chart */
        _selection.each(function(_data){
        	//console.log("function chart");
	        chartWidth = publicAttributes.width - publicAttributes.margin.left - publicAttributes.margin.right;
	        chartHeight = publicAttributes.height - publicAttributes.margin.top - publicAttributes.margin.bottom;
	        data = _data;
	        if(data){
	        	//console.log(data);
	        }
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
     * Generate accessors for each element in attributes
     * We are going to check if it's an own property and if the accessor
     * wasn't already created
     */
    
    for (var attr in publicAttributes) {
        if ((!chart[attr]) && (publicAttributes.hasOwnProperty(attr))) {
            chart[attr] = generateAccessor(attr);
        }
    }
    /**
     * Creates the d34 x and y axis, setting orientations
     * @private
     */
    function buildAxis(){
        xAxis = d34.axisBottom(publicAttributes.xScale).tickFormat(function(e){
	         if(Math.floor(e) != e)
	         {
	             return;
	         }

	         return e;
	     });
        yAxis = d34.axisLeft(publicAttributes.yScale);
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
    	publicAttributes.tooltip = d34.select(svg.node().parentNode)
    		.append("div")
    		.attr("class", "njhLine-tooltip")
    		.style("background-color", "#88aaaa")
    		.style("position", "absolute")
				.style("visibility", "hidden");
    	publicAttributes.tooltipTab = createTable(publicAttributes.tooltip)
	  		.style("border", "1px solid black")
	  		.style("box-shadow", "3px 3px 1.5px rgba(0, 0, 0, 0.5)")
	  		.attr("class", "njhLine-tooltip-tab");

        var container = svg.append('g')
	        .classed('container-group', true)
	        .attr("transform",'translate(' + publicAttributes.margin.left + ',' + publicAttributes.margin.top + ')');
        container.append('g')
        	.classed('chart-group', true)
        	.append("rect")
				.attr("class", "rect-wrap")
				.attr("x", 1)
				.attr("y", 1)
				.attr("fill", "#FFF");
        svg.select(".chart-group")
        	.append("line")
        	.attr("class", "njh-hoverline")
	      	.attr("y1", publicAttributes.yScale(d34.max([42, getMaxY(data)])))
	      	.attr("y2", publicAttributes.yScale(d34.min([0, getMinY(data)])));
        publicAttributes.selectline = svg.select(".chart-group")
	      	.append("line")
	      	.attr("class", "njh-posline")
	      	.attr("y1", publicAttributes.yScale(d34.max([42, getMaxY(data)])))
	      	.attr("y2", publicAttributes.yScale(d34.min([0, getMinY(data)])));
        
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
                .classed('line-chart', true);
            buildContainerGroups();
        }
        svg.transition()
        	.attr("width", publicAttributes.width + publicAttributes.margin.left + publicAttributes.margin.right)
        	.attr("height", publicAttributes.height + publicAttributes.margin.top + publicAttributes.margin.bottom);;
    }
    
    /**
     * Creates the x and y scales of the graph
     * @private
     */
  function buildScales(){
 		var maxX = getMaxX(data);
	 	var minY = getMinY(data);
	 	var maxY = getMaxY(data);

		//color = d34.scaleOrdinal(d34.schemeDark2)
		//		.domain(data.map(function(c) { return c[publicAttributes.nameValue]; }));
	 publicAttributes.color = d34.scaleSequential(d34.interpolateRainbow)
	 			.domain([0,data.length]);
	 			//.domain(data.map(function(c) { return c[publicAttributes.nameValue]; }));
    publicAttributes.xScale = d34.scaleLinear()
        .domain([0,  maxX])
        .range([0, chartWidth]);
    
    publicAttributes.yScale = d34.scaleLinear()
        .domain([d34.min([0,  minY]),d34.max([42, maxY])])
        .range([chartHeight, 0]);
  }
    
  /**
   * Draws the lines elements within the chart group
   * @private
   */
  function drawLines(){
  	var g = svg.select(".chart-group");
  	//re-adjust dims of the background rec
  	if(data.length > 0){
  		g.select(".rect-wrap")
				.attr("height", publicAttributes.yScale(getMinY(data)))
				.attr("width", publicAttributes.xScale(getMaxX(data)))
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
	    			.style("fill", function(d){ return publicAttributes.color(d.uid);})
		    		.style("stroke", function(d){ return publicAttributes.color(d.uid);});
/*
 * 		    		.style("fill", function(d){ return publicAttributes.color(d[publicAttributes.nameValue]);})
		    		.style("stroke", function(d){ return publicAttributes.color(d[publicAttributes.nameValue]);});
 */
	    //add paths and labels for entering groupings;
		g.on("mousemove", function(){		
			var xPos = Math.floor(publicAttributes.xScale.invert(d34.mouse(this)[0]));
			publicAttributes.tooltip.style("top", (d34.event.layerY-10)+"px").style("left",(d34.event.layerX+10)+"px");
			var display = data.map(function(d){ 
				 if(xPos < d[publicAttributes.yValue].length){
					return {"name":d[publicAttributes.nameValue] ,
									"qual":d[publicAttributes.yValue][xPos],
									"pos": xPos, 
									"color": publicAttributes.color(d.uid)};
				 }
			});
			updateTableWithColors(publicAttributes.tooltipTab, display, ["name", "qual", "pos"]);
			svg.select(".chart-group")
				.select(".njh-hoverline")
				.attr("x1", publicAttributes.xScale(xPos))
				.attr("x2", publicAttributes.xScale(xPos))
				.style("stroke-width", "2px")
    		.style("stroke", function(d){ return "#000000";});
		});
		
		if(null != publicAttributes.selectedPos && data.length > 0){
			publicAttributes.selectline = svg.select(".chart-group")
			.select(".njh-posline")
			.attr("x1", publicAttributes.xScale(publicAttributes.selectedPos))
			.attr("x2", publicAttributes.xScale(publicAttributes.selectedPos))
			.style("stroke-width", "2px")
			.style("opacity", 1)
  		.style("stroke", function(d){ return "#000000";});
		}else if(data.length  == 0){
			svg.select(".chart-group")
				.select(".njh-posline")
				.style("opacity", 0);
		}
		
		g.on("mouseout", function(){
			svg.select(".chart-group")
				.select(".njh-hoverline")
				.style("opacity", 0);
			return publicAttributes.tooltip.style("visibility", "hidden");});
		g.on("mouseover",function(){
			svg.select(".chart-group")
				.select(".njh-hoverline")
				.style("opacity", 1);
			return publicAttributes.tooltip.style("visibility", "visible");});
		
		//line setup
		var allPaths = lineGroups.selectAll(".njhline")
				.data(function(d){ return [d[publicAttributes.yValue]];});
		allPaths.exit()
			.transition(500)
			.style("opacity", 0)
			.remove();
		
		allPaths.enter()
				.append("path")
				.attr("class", "njhline")
				.style("stroke-width", "2px")
				.style("fill", "none")
			.merge(allPaths)
			.attr("d", function(d) { return line(d); })
			.style("opacity", 0)
			.transition(500)
			.style("opacity", 0.60);
		//dot setup
		var dots = lineGroups.selectAll(".njhdot")
			.data(function(d){ return d[publicAttributes.yValue]});
		dots.exit()
    		.transition(500)
			.style("opacity", 0)
			.remove();
		
		dots.enter()
			.append("circle")
				.attr("class", "njhdot")
				.attr("r", 2)
			.merge(dots)
				.style("opacity", 0)
			    .attr("cx", function(d, i) { return publicAttributes.xScale(i); })
			    .attr("cy", function(d, i) { return publicAttributes.yScale(d); })
			    .transition(500)
				.style("opacity", 0.60);
    }
	
  function setSelectLine(pos){
  	publicAttributes.selectedPos = pos;
		publicAttributes.selectline = svg.select(".chart-group")
		.select(".njh-posline")
			.attr("x1", publicAttributes.xScale(publicAttributes.selectedPos))
			.attr("x2", publicAttributes.xScale(publicAttributes.selectedPos))

			.style("stroke-width", "2px")
			.style("stroke", function(d){ return "#000000";});
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

