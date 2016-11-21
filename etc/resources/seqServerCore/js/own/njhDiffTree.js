/**
 * 
 */



function njhDiffTree(jsonData, addTo, hovIdStub, width, height){
	tooltipId = "#" + hovIdStub + "_popHover";
	if($(tooltipId).length){
		d34.select(tooltipId).remove();
	}
	var tooltip = d34.select("body")
				.append("div")
				.style("position", "absolute")
				.style("visibility", "hidden")
				.style("background-color", "#88aaaa")
				.attr("id", hovIdStub + "_popHover");
	
	d34.select(tooltipId).append("h3")
		.attr("id", hovIdStub + "_popHover_title")
		.style("margin", "5px");
	hoverTab = createTable(tooltipId);
	hoverTab.style("border", "1px solid black");
	hoverTab.style("box-shadow", "3px 3px 1.5px rgba(0, 0, 0, 0.5)");
	if(!width){
		width = 1000;
	}
	if(!height){
		height = 1000;
	}
	
	var simulation = d34.forceSimulation()
	    .force("link", d34.forceLink().id(function(d) { return d.index; }))
	    .force("charge", d34.forceManyBody().strength(-15))
	    .force("collide",d34.forceCollide( function(d){return d.r + 8 }).iterations(16) )
	    .force("center", d34.forceCenter(width / 2, height / 2));
	
	/*
	var simulation = d34.forceSimulation()
	    .force("link", d34.forceLink().id(function(d) { return d.index }))
	    .force("collide",d34.forceCollide( function(d){return d.r + 8 }).iterations(16) )
	    .force("charge", d34.forceManyBody().strength(-60))
	    .force("center", d34.forceCenter(width / 2, height / 2))
	    .force("y", d34.forceY(0))
	    .force("x", d34.forceX(0));
	*/
	
	/*
	var simulation = d34.forceSimulation()
	    .force("link", d34.forceLink().id(function(d) { return d.index; }))
	    .force("charge", d34.forceManyBody().strength(-6))
	    .force("center", d34.forceCenter(width / 2, height / 2));
	*/
	
	d34.select(addTo)
		.attr("width", width)
		.attr("height", height);
	
	var svg = d34.select(addTo)
		.append("svg")
		.attr("width", width)
		.attr("height", height)
		.attr("id", "chart");
	
	svg.append('rect')
		.attr('width', width)
		.attr('height', height)
		.attr('fill', jsonData.backgroundColor);
	
	simulation
	      .nodes(jsonData.nodes)
	      .on("tick", ticked);
	
	simulation.force("link")
	      .links(jsonData.links);


	var link = svg.selectAll(".link")
		.data(jsonData.links)
		.enter()
			.append("line")
			.attr("class", "link")
			.style("stroke", function(d) {return d.color;})
			.style("stroke-width", function(d) { return 2;});
	// add the nodes
	svg.selectAll(".node")
		.data(jsonData.nodes)
		.enter().append("g")
			.attr("class", function(d) {return "node " + d.type;})
				.call(d34.drag()
			          .on("start", dragstarted)
			          .on("drag", dragged)
			          .on("end", dragended))
			.append("circle");
	var node = svg.selectAll(".node");
	node.select("circle")
		  .attr("r", function(d) { return Math.sqrt(d.size/Math.PI); })
	      .style("fill", function(d) { return d.color; })
	      .style("stroke", "#fff")
	      .style("stroke-width", "1.5px");

  var indels = svg.selectAll(".indel");
  var snps = svg.selectAll(".snp");
  indels.selectAll("circle").style("stroke", "#F00").style("stroke-width", 1);
  
  indels.selectAll("circle").on("mouseover", function(d){
  			d34.select("#" + hovIdStub + "_popHover_title").html("Indel")
  			updateTable(hoverTab, [{"SeqName":d.ref, "Pos":d.refPos, "GapSeq":d.refDisplay},{"SeqName":d.seq, "Pos":d.seqPos, "GapSeq":d.seqDisplay}], ["SeqName", "Pos", "GapSeq"]);
	      	 return tooltip.style("visibility", "visible");})
		  .on("mousemove", function(){return tooltip.style("top", (d34.event.layerY-10)+"px").style("left",(d34.event.layerX+10)+"px");})
		  .on("mouseout", function(){return tooltip.style("visibility", "hidden");});
  snps.selectAll("circle").style("stroke", "#00F").style("stroke-width", 1);
  snps.selectAll("circle").on("mouseover", function(d){
  			d34.select("#" + hovIdStub + "_popHover_title").html("Snp")
			 updateTable(hoverTab,[{"SeqName":d.ref, "Pos":d.refPos, "Base":d.refBase, "Qual":d.refBaseQual},{"SeqName":d.seq, "Pos":d.seqPos, "Base":d.seqBase, "Qual":d.seqBaseQual}], ["SeqName", "Pos", "Base", "Qual"]);
	      	 return tooltip.style("visibility", "visible");})
		  .on("mousemove", function(){return tooltip.style("top", (d34.event.layerY-10)+"px").style("left",(d34.event.layerX+10)+"px");})
		  .on("mouseout", function(){return tooltip.style("visibility", "hidden");});
		  
  var variants = svg.selectAll(".variant");
  variants.selectAll("circle").style("stroke", "#999");
  variants.selectAll("circle").on("mouseover", function(d){
  			 d34.select("#" + hovIdStub + "_popHover_title").html("Variant")
             updateTable(hoverTab, [{"SeqName":d.name, "ReadCnt":d.cnt, "Relative Abundance":d.frac}], ["SeqName", "ReadCnt", "Relative Abundance"]);
  			//tooltip.node().innerHTML = "<strong>Variant</strong>: " + d.name 
	      	 return tooltip.style("visibility", "visible");})
		  .on("mousemove", function(){return tooltip.style("top", (d34.event.layerY-10)+"px").style("left",(d34.event.layerX+10)+"px");})
		  .on("mouseout", function(){return tooltip.style("visibility", "hidden");});
  
  variants.append("text")
	  .attr("x", 12)
	  .attr("dy", ".35em")
	  .style("fill","#FFF")
	  .style("font-family", "\"HelveticaNeue-Light\", \"Helvetica Neue Light\", \"Helvetica Neue\", Helvetica, Arial, \"Lucida Grande\", sans-serif")
	  .style("font-size", "12px")
	  .style("font-weight","900")
	  .style("pointer-events", "none")
	  .style("stroke", "#FFF")
	  .style("stroke-width", "1px")
	  .text(function(d) {return d.name;});;

	function ticked() {
	    link.attr("x1", function(d) { return d.source.x; })
	        .attr("y1", function(d) { return d.source.y; })
	        .attr("x2", function(d) { return d.target.x; })
	        .attr("y2", function(d) { return d.target.y; });
	    
	  
	    node.attr("transform", function(d) { return "translate(" + d.x + "," + d.y + ")"; });
	    
	    /*node.selectAll("circle")
		    .attr("cx", function(d) { return d.x; })
		    .attr("cy", function(d) { return d.y; });*/
	};

	function dragstarted(d) {
	  if (!d34.event.active) simulation.alphaTarget(0.3).restart();
	  d.fx = d.x;
	  d.fy = d.y;
	};

	function dragged(d) {
	  d.fx = d34.event.x;
	  d.fy = d34.event.y;
	};

	function dragended(d) {
	  if (!d34.event.active) simulation.alphaTarget(0);
	  d.fx = null;
	  d.fy = null;
	};

}