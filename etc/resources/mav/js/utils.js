function createNavBarSkeleton(wrappingNavSelector, rName){
	addFixedTopNavSkeleton(wrappingNavSelector, "mips", "mainNav", "projectNav");
 	addNavLink("#mainNav", "Home", "/" + rName, "#siteHomeLink");
	addNavDrop("#mainNav", "Genes", "geneDrop");
	addNavDrop("#mainNav", "Samples All Mips", "samplesDrop");
	addNavDrop("#mainNav", "Pre-clustering Extraction Stats", "extractionDrop");
}

function populateNavBar(wrappingNavSelector, rName, geneName, sampName, mipName){
   	//get genes names
	var groupNames;
	ajax("/" + rName + "/allGroupingNames", function(mn){ groupNames = mn; });
	var geneLinkPre = "/" + rName + "/showGeneInfo/";
	//get samples names
	var sampleNames;
	ajax("/" + rName + "/allSampNames", function(mn){ sampleNames = mn; });
	var sampleLinkPre = "/" + rName + "/showOneSampAllMipData/";
	var sampleExtractionLinkPre = "/" + rName + "/showInitialReadStatsPerSample/";
	
	
    d3.select("#geneDrop")
		.selectAll("li")
		.data(groupNames)
		.enter()
			.append("li")
				.attr("class", function(d){
					if(d == geneName){
						return "active";
					}else{
						return "";
					}
				})
			.append("a")
				.attr("href", function(d){ return geneLinkPre + d;})
				.text(function(d){return d;});
    
	d3.select("#samplesDrop")
		.selectAll("li")
		.data(sampleNames)
		.enter()
			.append("li")
				.attr("class", function(d){
					if(d == sampName){
						return "active";
					}else{
						return "";
					}
				})
			.append("a")
				.attr("href",function(d){return sampleLinkPre + d;} )
				.text(function(d){return d;});
	
	d3.select("#extractionDrop")
	.selectAll("li")
	.data(sampleNames)
	.enter()
		.append("li")
			.attr("class", function(d){
				if(d == sampName){
					return "active";
				}else{
					return "";
				}
			})
		.append("a")
			.attr("href",function(d){return sampleExtractionLinkPre + d;} )
			.text(function(d){return d;});
   
}


function createNavBar(wrappingNavSelector, rName, geneName, sampName, mipName){
	createNavBarSkeleton(wrappingNavSelector, rName);
	populateNavBar(wrappingNavSelector, rName, geneName, sampName, mipName);
}

