var createMinTree = function(data, appendTo, name, width, height){
	// create a interconnected graph for a minimum spanning tree
	// data needs to have a "nodes" array and a "links" array 
	// nodes need to have at least the following variables, color (color of node), 
	// name (name of the node), and size (the size of the node)
	// links need to have at least color (the color of the link), target (the node position to connect to), 
	// source (the node position from where the connection is forming), and value (the value that controls the link distance)
	var svg = d3.select(appendTo).append("svg")
	    .attr("width", width)
	    .attr("height", height)
	    .attr("id", name);
	
	d3.json(data, function(error, graph)
	 {
	 var force = d3.layout.force()
	    .charge(-120)
	    .linkDistance(function(d, i){ return d.value * 10;})
	    .size([width, height]);
	    
	  force
	      .nodes(graph.nodes)
	      .links(graph.links)
	      .start();
	
	  var link = svg.selectAll(".link")
	      .data(graph.links)
	      .enter().append("line")
	      .attr("class", "link")
	      .style("stroke-width", function(d) { return Math.sqrt(d.value);})
	      .style("stroke", function(d) { return d.color;});
	
	  /*var node = svg.selectAll(".node")
	      .data(graph.nodes)
	    .enter().append("circle")
	      .attr("class", "node")
	      .attr("r", function(d){ return Math.pow(d.size * 60, 1/2);})
	      .style("fill", function(d) { return d.color; })
	      .call(force.drag);*/
	 var node = svg.selectAll(".node")
	      .data(graph.nodes)
	    .enter().append("circle")
	      .attr("class", "node")
	      .attr("r", function(d){ return d.size * 8;})
	      .style("fill", function(d) { return d.color; })
	      .call(force.drag);
	  node.append("title")
	      .text(function(d) { return d.name; });
	
	  force.on("tick", function() {
	    link.attr("x1", function(d) { return d.source.x; })
	        .attr("y1", function(d) { return d.source.y; })
	        .attr("x2", function(d) { return d.target.x; })
	        .attr("y2", function(d) { return d.target.y; });
	
	    node.attr("cx", function(d) { return d.x; })
	        .attr("cy", function(d) { return d.y; });
	  });
	  
	});
	return force;
};