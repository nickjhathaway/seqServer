//
// SeekDeep - A library for analyzing amplicon sequence data
// Copyright (C) 2012-2016 Nicholas Hathaway <nicholas.hathaway@umassmed.edu>,
// Jeffrey Bailey <Jeffrey.Bailey@umassmed.edu>
//
// This file is part of SeekDeep.
//
// SeekDeep is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SeekDeep is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SeekDeep.  If not, see <http://www.gnu.org/licenses/>.
//
//


function njhMenuItem(idName, displayName, func){
	this.idName = idName;
	this.displayName = displayName;
	this.func = func;
};

function createSeqMenu(idNameOfParentDiv, menuContent){
	//console.log(idNameOfParentDiv);
	var nav = d34.select(idNameOfParentDiv)
				.append("nav")
					.attr("id", "myNavBar")
					.attr("class", "navbar navbar-default")
					.attr("role", "navigation")
						.append("div")
							.attr("class", "container")
							.style("margin-left", 0)
								.append("div")
									.attr("class", "navbar-header")
										.append("ul")
										.attr("class", "nav navbar-nav");
	var menuKeys = Object.keys(menuContent);
	for (mk in menuKeys){
		var currentMenuItem = nav.append("li").attr("class", "dropdown");
		currentMenuItem.append("a")
			.attr("href", "#")
			.attr("data-toggle", "dropdown")
			.attr("class", "dropdown-toggle")
			.text(menuKeys[mk])
				.append("b").attr("class", "caret");
		var currentMenuItemOptions = currentMenuItem.append("ul")
			.attr("class", "dropdown-menu")
			.attr("id", menuKeys[mk] + "Drops");
		for (it in menuContent[menuKeys[mk]]){
			currentMenuItemOptions.append("li")
				.append("a")
					.attr("href", "javascript:void(0)")
					.attr("id",menuContent[menuKeys[mk]][it].idName)
					.text(menuContent[menuKeys[mk]][it].displayName)
					.on("click", menuContent[menuKeys[mk]][it].func);
		}
	}
}




function njhSeqView(viewName, seqData, params){
	//need to add style and html, currently just there
	//retrieve html elements
	this.topDivName = viewName;
	this.topDiv = document.getElementById(viewName);
	$(this.topDiv).addClass("SeqView");
	this.seqStart = 0;
	this.baseStart = 0;
	this.currentSeq = 0;
	this.currentBase = 0;
	this.needToPaint = true;
	if(typeof params === 'undefined'){
		params = {}
	}
	this.cw = (params.hasOwnProperty("cellWidth")) ?   params["cellWidth"] : 20;
	this.ch = (params.hasOwnProperty("cellHeight")) ?  params["cellHeight"] : 25;
	this.showingQualChart = (params.hasOwnProperty("addQualChart")) ?  params["addQualChart"] : false;
    //setting name offset for displaying sequence names
	this.nameOffSet = 10 * this.cw;

	var self = this;

	//set up input seqData;
	this.seqData = seqData;
	this.sessionUID = seqData["sessionUID"];
	this.uid = seqData["uid"];
	this.selected = new Set(seqData["selected"]);
	this.protein = seqData["seqType"] === "protein";
	this.bColors = seqData["baseColor"];

	//order for the next three is important
	//initiate the menu
	this.initDefaultMenu();
	//initiate the action buttons
	this.initActionButtons();
	//initiate the drawing area
	this.initDrawArea();
	//add quality chart if indicated
	if(this.showingQualChart){
		this.addQualChart();
	}

	//bind the resize of the window to update the viewer as well
	$(window).bind("resize", function(){
		self.updateOnResize();
	});
	self.setUp();
	self.paint();
};

njhSeqView.prototype.removeQualChart = function(){
	this.showingQualChart = false;
	d34.select(this.topDivName +  " .njhSeqViewMenu #ShowQual").text("Show Qual Graph");
	d34.select(this.topDivName + " .qualChart").selectAll("*").remove();
	this.qualChart = null;
}

njhSeqView.prototype.getSelectedQualData = function(){
	var self = this;
	var qualData = [];
	if (self.selected.size > 0){
    	var sels = setToArray(self.selected);
	    sels.forEach(function(sel){
	    	qualData.push({
	    		name:self.seqData["seqs"][sel]["name"],
	    		quals:self.seqData["seqs"][sel]["qual"]});
	    });
    }
	return qualData;
}


njhSeqView.prototype.addQualChart = function(){
	var self = this;
	var qualData = this.getSelectedQualData();

	this.qualChart = d34.njh.LineChart();
	this.qualChart.
		width(parseFloat(self.seqSvgMaster.style("width")));

	d34.select(this.topDivName + " .qualChart")
		.datum(qualData)
		.call(self.qualChart);

	this.showingQualChart = true;
	d34.select(this.topDivName +  " .njhSeqViewMenu #ShowQual").text("Hide Qual Graph");
}

njhSeqView.prototype.toggleQualChart = function(){
	if(this.showingQualChart){
		this.removeQualChart();
	}else{
		this.addQualChart();
	}
}

njhSeqView.prototype.initDrawArea = function(){
	var self = this;

	this.masterDivd34 = d34.select(this.topDivName)
		.append("div")
		.attr("class", "SeqViewDrawAreaDiv");

	this.masterDivd34
		.append("div")
		.attr("class", "rightSliderDiv")
		.append("input")
			.attr("class", "rightSlider")
			.attr("data-slider-id", "rightSliderCon");

	this.seqSvgMaster = this.masterDivd34
		.append("svg")
		.attr("class", "NjhSeqViewerMainSvg")

	this.seqSvgMasterG = this.seqSvgMaster
		.append("g")
		    .style("font-family", "Arial")
			.style("font-size", "15px")
			.style("font-weight", "bold");

	this.masterDivd34
		.append("div")
		.attr("class", "bottomSliderDiv")
		.append("input")
			.attr("class", "bottomSlider")
			.attr("data-slider-id", "bottomSliderCon");

	this.masterDivd34
		.append("div")
		.attr("class", "pop-up");

	this.popTab = createTable(this.topDivName + " .pop-up")
		.style("border", "1px solid black")
		.style("box-shadow", "3px 3px 1.5px rgba(0, 0, 0, 0.5)")
		.attr("id", "pop-up-tab");

	this.masterDivd34
		.append("div")
		.attr("class", "select")
		.style("width", this.cw + 2 + "px")
		.style("height", this.ch + 2 + "px");

	this.chart = d34.select(this.topDivName).append("div")
		.attr("class", "qualChart");

	d34.select(this.topDivName).append("div")
		.attr("id", "minTreeChartTop");


	this.masterDiv = this.masterDivd34.node();

	this.rSlider = $(".rightSlider", this.masterDiv)[0];
	this.bSlider = $(".bottomSlider", this.masterDiv)[0];
	this.rSliderDiv = $(".rightSliderDiv", this.masterDiv)[0];
	this.bSliderDiv = $(".bottomSliderDiv", this.masterDiv)[0];
	this.popUp = $(".pop-up", this.masterDiv)[0];
	this.sel = $(".select", this.masterDiv)[0];
}

njhSeqView.prototype.initActionButtons = function(){
	var self = this;
	//add download fasta button
	d34.select(this.topDivName).append("div")
		.attr("class", "downFastaDiv")
		.style("margin", "5px")
		.style("float", "left");

	var linkButton = d34.select(this.topDivName + " .downFastaDiv")
		.append("button")
			.attr("class", "fastaSaveButton btn btn-success");
	d34.select(this.topDivName + " .downFastaDiv button")
		.append("i")
			.attr("class", "fa fa-download fa-lg");
	d34.select(this.topDivName + " .downFastaDiv button")
		.append("span")
			.text(" Download Fasta");
	linkButton.append("a")
			.attr("class", "fastaDownLink");

	linkButton.on("click", function(){
	    var mainTable = [];
	    if (self.selected.size > 0){
	    	var sels = setToArray(self.selected);
		    for (i in sels) {
		    	//console.log(sels[i]);
				mainTable.push([">" + self.seqData["seqs"][sels[i]]["name"]]);
				mainTable.push([self.seqData["seqs"][sels[i]]["seq"]]);
			}
	    }else{
		    for (i = 0; i <self.seqData["seqs"].length ; i++) {
				mainTable.push([">" + self.seqData["seqs"][i]["name"]]);
				mainTable.push([self.seqData["seqs"][i]["seq"]]);
			}
	    }
	    mainTable.push([]);
	  	var fastaData = 'data:text/plain;base64,'+ btoa(d34.tsvFormatRows(mainTable));
	  	linkButton.select(".fastaDownLink").attr("download", self.seqData["uid"] + ".fasta");
	  	linkButton.select(".fastaDownLink").attr("href", fastaData).node().click();
	});

	if(!this.protein){
		//add fastq download button
		d34.select(this.topDivName).append("div")
			.attr("class", "downFastqDiv")
			.style("margin", "5px")
			.style("float", "left");
		var fastqLinkButton = d34.select(this.topDivName + " .downFastqDiv")
			.append("button")
				.attr("class", "fastqSaveButton btn btn-primary");
		d34.select(this.topDivName + " .downFastqDiv button")
			.append("i")
				.attr("class", "fa fa-download fa-lg")
		d34.select(this.topDivName + " .downFastqDiv button")
			.append("span")
				.text(" Download Fastq")
		fastqLinkButton.append("a")
			.attr("class", "fastqDownLink");
		fastqLinkButton.on("click", function(){
		    var mainTable = [];
		    if (self.selected.size > 0){
		    	var sels = setToArray(self.selected);
			    for (i in sels) {
			    	//console.log(sels[i]);
					mainTable.push(["@" + self.seqData["seqs"][sels[i]]["name"]]);
					mainTable.push([self.seqData["seqs"][sels[i]]["seq"]]);
					mainTable.push(["+"]);
					mainTable.push([self.seqData["seqs"][sels[i]]["qual"].map(function(q){return String.fromCharCode(33 + q);}).join("")]);
				}
		    }else{
			    for (i = 0; i <self.seqData["seqs"].length ; i++) {
					mainTable.push(["@" + self.seqData["seqs"][i]["name"]]);
					mainTable.push([self.seqData["seqs"][i]["seq"]]);
					mainTable.push(["+"]);
					mainTable.push([self.seqData["seqs"][i]["qual"].map(function(q){return String.fromCharCode(33 + q);}).join("")]);
				}
		    }
		    mainTable.push([]);
		  	var fastqData = 'data:text/plain;base64,'+ btoa(d34.tsvFormatRows(mainTable));
		  	fastqLinkButton.select(".fastqDownLink").attr("download", self.seqData["uid"] + ".fastq");
		  	fastqLinkButton.select(".fastqDownLink").attr("href", fastqData).node().click();
		});
	}
	//add de-select all button
	d34.select(this.topDivName).append("div")
		.attr("class", "deselectDiv")
		.style("margin", "5px")
		.style("float", "left");



	var deselectButton = d34.select(this.topDivName + " .deselectDiv")
		.append("button")
			.text("Clear Selection")
			.attr("class", "deselectAllBut btn btn-info");

	deselectButton.on("click", function(){
		self.clearSelected();
	});
}

njhSeqView.prototype.clearSelected = function(){
	var self = this
	self.selected.clear();
	self.updateHighlightedSeqs();
	if(self.qualChart){
		d34.select(self.topDivName + " .qualChart")
			.datum([])
			.call(self.qualChart);
	}
}

njhSeqView.prototype.initDefaultMenu = function(){

	this.menuDiv = d34.select(this.topDivName).append("div")
		.attr("class", "njhSeqViewMenu");
	var locSplit = window.location.toString().split(/[\/]+/);
	var rName = locSplit[2];
	var menuItems = {};
	var sortOptions = [];
	var self = this;
	sortOptions.push(new njhMenuItem("sortSeq", "Sequence",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/seq', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	sortOptions.push(new njhMenuItem("sortSeqCondensed", "Sequence Condensed",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/seqCondensed', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	sortOptions.push(new njhMenuItem("sortTotalCount", "Total Read Count",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/totalCount', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	sortOptions.push(new njhMenuItem("sortSize", "Length",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/size', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	sortOptions.push(new njhMenuItem("sortName", "Name",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/name', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	sortOptions.push(new njhMenuItem("sortName", "Reverse",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/sort/reverse', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	menuItems["Sort"] = sortOptions;
	var alnOptions = [];
	alnOptions.push(new njhMenuItem("muscle", "muscle",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/muscle', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	alnOptions.push(new njhMenuItem("removeGaps", "remove gaps",function(){
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/removeGaps', postData).then(function (seqData) {
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	menuItems["Aln"] = alnOptions;

	var countOptions = [];
	countOptions.push(new njhMenuItem("countBases", "Count Bases",function(){
		var mainData;
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/countBases', postData).then(function (baseCounts) {
	  		console.log(baseCounts);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));
	menuItems["Counts"] = countOptions;

	var editOptions = [];
	if(!this.protein){

		editOptions.push(new njhMenuItem("complement", "Reverse Complement",function(){
			var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
			if (self.selected.size > 0){
				postData["selected"] = setToArray(self.selected);
				postData["positions"] = [];
				for(selPos in postData["selected"]){
					postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
				}
			}
			var gifLoading = prsentDivGifLoading();
			postJSON('/' + rName + '/complement', postData).then(function (seqData) {
				//console.log(seqData);
		  		self.updateData(seqData);
		  		gifLoading.remove();
		  	}).catch(logRequestError);
		}));
	}

	editOptions.push(new njhMenuItem("deleteSeqs", "Delete Sequences",function(){
		var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
		if (self.selected.size > 0){
			postData["selected"] = setToArray(self.selected);
			postData["positions"] = [];
			for(selPos in postData["selected"]){
				postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
			}
		}
		var gifLoading = prsentDivGifLoading();
		postJSON('/' + rName + '/deleteSeqs', postData).then(function (seqData) {
			//console.log(seqData);
			  self.currentSeq = 0;
			  self.currentBase = 0;
			  if(seqData["numReads"] < self.seqStart){
			  	self.seqStart = Math.max(seqData["numReads"] - self.nSeqs, 0);
			  }
			  if(seqData["maxLen"] < self.baseStart){
			  	self.baseStart = 0;
			  }
				self.clearSelected();
	  		self.updateData(seqData);
	  		gifLoading.remove();
	  	}).catch(logRequestError);
	}));

	menuItems["Edit"] = editOptions;

	if(!this.protein){
		var translateOptions = [];
		translateOptions.push(new njhMenuItem("translate", "Translate",function(){
			//console.log($("#startSiteInput", self.topDivName).val());
			var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID, "start" : $("#startSiteInput", self.topDivName).val()};
			if (self.selected.size > 0){
				postData["selected"] = setToArray(self.selected);
				postData["positions"] = [];
				for(selPos in postData["selected"]){
					postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
				}
			}
			var gifLoading = prsentDivGifLoading();
			postJSON('/' + rName + '/translate', postData).then(function (seqData){
			    if($("#" + self.topDivName.substring(1) + "_protein").length){
			    	self.proteinViewer.updateData(seqData);
			    }else{
			    	$( "<div id = \"" + self.topDivName.substring(1) + "_protein" +   "\"></div>" ).insertAfter( self.topDivName );
			    	self.proteinViewer = new njhSeqView("#" + self.topDivName.substring(1) + "_protein", seqData, self.cw, self.ch,false);
			    	self.proteinViewer.setUp();
			    	self.proteinViewer.paint();
			    }
			    gifLoading.remove();
			    $("#" + self.topDivName.substring(1) + "_protein").scrollView();

		  	}).catch(logRequestError);
		}));
		menuItems["Translate"] = translateOptions;
	}

	if(!this.protein){
		var qualityOptions = [];
		var qualMenuTitle = "Hide Qual Graph";
		if(!self.showingQualChart){
			qualMenuTitle = "Show Qual Graph";
		}
		qualityOptions.push(new njhMenuItem("ShowQual", qualMenuTitle,function(){
			self.toggleQualChart();
		}));
		menuItems["Quality"] = qualityOptions;
		var windowOptions = [];
		windowOptions.push(new njhMenuItem("GenTree", "Gen Difference Graph",function(){
			if(!($(self.topDivName + " #minTreeChartTop #saveButton").length)){
				d34.select(self.topDivName + " #minTreeChartTop").append("button")
					.style("float", "top")
					.attr("class", "btn btn-success")
					.attr("id", "saveButton")
					.style("margin", "2px")
					.text("Save As Svg");
				addSvgSaveButton(self.topDivName + " #minTreeChartTop #saveButton", self.topDivName + " #minTreeChartTop #minTreeChart #chart", self.seqData["uid"])
			}
			if(!($(self.topDivName + " #minTreeChartTop #minTreeChart").length)){
				d34.select(self.topDivName + " #minTreeChartTop").append("svg").attr("id", "minTreeChart")
					.attr("width", "0px")
					.attr("height", "0px")
					.style("margin-left", "10px")
			}else{
				d34.select(self.topDivName + " #minTreeChart").selectAll("*").remove();
			}
			var jsonData;
			var postData = {"uid" : self.uid, "sessionUID" : self.sessionUID};
			if (self.selected.size > 0){
				postData["selected"] = setToArray(self.selected);
				postData["positions"] = [];
				for(selPos in postData["selected"]){
					postData["positions"].push(self.seqData["seqs"][postData["selected"][selPos]]["position"])
				}
			}
		//
			postData["numDiff"] = $("#numDiffInput", self.topDivName).val();
			var numThreads = 2,gapOpenPen = 5,gapExtPen = 1,match = 2,mismatch = -2;
			postData["numThreads"] = numThreads;
			postData["gapOpenPen"] = parseInt($("#gapOpenInput", self.topDivName).val());
			postData["gapExtPen"] = parseInt($("#gapExtInput", self.topDivName).val());
			postData["match"] = parseInt($("#matchInput", self.topDivName).val());
			postData["mismatch"] = parseInt($("#mismatchInput", self.topDivName).val());
			postData["justBest"] = $("#justBestInput", self.topDivName)[0].checked;

			var gifLoading = prsentDivGifLoading();
			postJSON('/' + rName + '/minTreeDataDetailed', postData).then(function (seqData){
				njhDiffTree(seqData, self.topDivName + " #minTreeChart", "minTreeChart",
						$("#treeWidthInput", self.topDivName).val(),
						$("#treeHeightInput", self.topDivName).val());
				if(($(self.topDivName + " #minTreeChartTop #diffInfoTable").length)){
					d34.select(self.topDivName + " #minTreeChartTop #diffInfoTable").remove();
				}
				addDiv(self.topDivName + " #minTreeChartTop", "diffInfoTable");
				var diffInfoTable = new njhTable("#diffInfoTable", seqData["infoTab"], self.uid + "_diffInfo", false);
				$('#minTreeChart').scrollView();
		  		gifLoading.remove();
		  	}).catch(logRequestError);
		}));
		windowOptions.push(new njhMenuItem("HideTree", "Hide Difference Graph",function(){
			d34.select(self.topDivName + " #minTreeChartTop").selectAll("*").remove();
		}));
		menuItems["Diff-Graph"] = windowOptions;
	}


	createSeqMenu(this.topDivName + " .njhSeqViewMenu", menuItems);

	if(!this.protein){
		var startSiteInput = d34.select(self.topDivName +  " .njhSeqViewMenu #TranslateDrops")
		.append("li")
			.append("div")
				.attr("style", "padding: 3px 20px;")
			.append("form")
				.attr("class", "form-inline")
				.attr("id", "startSiteForm");
		startSiteInput
			.append("label")
				.attr("id", "startSiteLabel")
				.attr("for","startSiteInput")
				.attr("class", "control-label")
				.text("Start")
				.style("margin-right", "5px");
		var divInputGroup = startSiteInput
			.append("div")
				.attr("class", "input-group");
		divInputGroup.append("input")
			.attr("type", "number")
			.attr("class", "form-control")
			.attr("id", "startSiteInput")
			.attr("step", "1")
			.attr("min", "0")
			.attr("max", "2")
			.attr("value", "0");
		$('#startSiteForm').submit(function(e){
	        e.preventDefault();
	    });
		var treeWidthInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
			.append("li")
				.append("div")
					.attr("style", "padding: 3px 20px;")
				.append("form")
					.attr("class", "form-inline")
					.attr("id", "treeWidthForm");
		treeWidthInput
			.append("label")
				.attr("id", "treeWidthLabel")
				.attr("for","treeWidthInput")
				.attr("class", "control-label")
				.text("Graph Window Width")
				.style("margin-right", "5px");;
		var divtreeWidthInputGroup = treeWidthInput
			.append("div")
			.attr("class", "input-group");
		divtreeWidthInputGroup.append("input")
			.attr("type", "number")
			.attr("class", "form-control")
			.attr("id", "treeWidthInput")
			.attr("step", "100")
			.attr("min", "0")
			.attr("value", "1000");
		$('#treeWidthForm').submit(function(e){
	        e.preventDefault();
	    });
		var treeHeightInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
			.append("li")
				.append("div")
					.attr("style", "padding: 3px 20px;")
				.append("form")
					.attr("class", "form-inline")
					.attr("id", "treeHeightForm");
		treeHeightInput
			.append("label")
				.attr("id", "treeHeightLabel")
				.attr("for","treeHeightInput")
				.attr("class", "control-label")
				.text("Graph Window Height")
				.style("margin-right", "5px");
		var divTreeHeightInputGroup = treeHeightInput
			.append("div")
				.attr("class", "input-group");
		divTreeHeightInputGroup.append("input")
			.attr("type", "number")
			.attr("class", "form-control")
			.attr("id", "treeHeightInput")
			.attr("step", "100")
			.attr("min", "0")
			.attr("value", "1000");
		$('#treeHeightForm').submit(function(e){
	        e.preventDefault();
	    });

		var numDiffInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
			.append("li")
				.append("div")
					.attr("style", "padding: 3px 20px;")
				.append("form")
					.attr("class", "form-inline")
					.attr("id", "numDiffForm");
		numDiffInput
			.append("label")
				.attr("id", "numDiffLabel")
				.attr("for","numDiffInput")
				.attr("class", "control-label")
				.text("Num Diff\n0=min to connect all")
				.style("margin-right", "5px");;
		var divNumDiffInputGroup = numDiffInput
			.append("div")
				.attr("class", "input-group");
		divNumDiffInputGroup.append("input")
			.attr("type", "number")
			.attr("class", "form-control")
			.attr("id", "numDiffInput")
			.attr("step", "1")
			.attr("min", "0")
			.attr("value", "0");
		$('#numDiffForm').submit(function(e){
	        e.preventDefault();
	    });

	// gap open
	var gapOpenInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
	.append("li")
		.append("div")
			.attr("style", "padding: 3px 20px;")
		.append("form")
			.attr("class", "form-inline")
			.attr("id", "gapOpenForm");
	gapOpenInput
		.append("label")
			.attr("id", "gapOpenLabel")
			.attr("for","gapOpenInput")
			.attr("class", "control-label")
			.text("Gap Open Penaltiy")
			.style("margin-right", "5px");;
	var divgapOpenInputGroup = gapOpenInput
		.append("div")
			.attr("class", "input-group");
	divgapOpenInputGroup.append("input")
		.attr("type", "number")
		.attr("class", "form-control")
		.attr("id", "gapOpenInput")
		.attr("step", "1")
		.attr("min", "1")
		.attr("value", "5");
	$('#gapOpenForm').submit(function(e){
        e.preventDefault();
  });
	// gap Ext
	var gapExtInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
	.append("li")
		.append("div")
			.attr("style", "padding: 3px 20px;")
		.append("form")
			.attr("class", "form-inline")
			.attr("id", "gapExtForm");
	gapExtInput
		.append("label")
			.attr("id", "gapExtLabel")
			.attr("for","gapExtInput")
			.attr("class", "control-label")
			.text("Gap Extension Penaltiy")
			.style("margin-right", "5px");;
	var divgapExtInputGroup = gapExtInput
		.append("div")
			.attr("class", "input-group");
	divgapExtInputGroup.append("input")
		.attr("type", "number")
		.attr("class", "form-control")
		.attr("id", "gapExtInput")
		.attr("step", "1")
		.attr("min", "1")
		.attr("value", "1");
	$('#gapExtForm').submit(function(e){
        e.preventDefault();
  });

	// match
	var matchInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
	.append("li")
		.append("div")
			.attr("style", "padding: 3px 20px;")
		.append("form")
			.attr("class", "form-inline")
			.attr("id", "matchForm");
	matchInput
		.append("label")
			.attr("id", "matchLabel")
			.attr("for","matchInput")
			.attr("class", "control-label")
			.text("Match Score")
			.style("margin-right", "5px");;
	var divmatchInputGroup = matchInput
		.append("div")
			.attr("class", "input-group");
	divmatchInputGroup.append("input")
		.attr("type", "number")
		.attr("class", "form-control")
		.attr("id", "matchInput")
		.attr("step", "1")
		.attr("min", "1")
		.attr("value", "2");
	$('#matchForm').submit(function(e){
        e.preventDefault();
  });
	// mistmatch
	var mismatchInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
	.append("li")
		.append("div")
			.attr("style", "padding: 3px 20px;")
		.append("form")
			.attr("class", "form-inline")
			.attr("id", "mismatchForm");
	mismatchInput
		.append("label")
			.attr("id", "mismatchLabel")
			.attr("for","mismatchInput")
			.attr("class", "control-label")
			.text("mismatch Score")
			.style("margin-right", "5px");;
	var divmismatchInputGroup = mismatchInput
		.append("div")
			.attr("class", "input-group");
	divmismatchInputGroup.append("input")
		.attr("type", "number")
		.attr("class", "form-control")
		.attr("id", "mismatchInput")
		.attr("step", "1")
		.attr("max", "0")
		.attr("value", "-2");
	$('#mismatchForm').submit(function(e){
        e.preventDefault();
  	});
  	// just best
	var justBestInput = d34.select(self.topDivName +  " .njhSeqViewMenu #Diff-GraphDrops")
		.append("li")
			.append("div")
				.attr("style", "padding: 3px 20px;")
			.append("form")
				.attr("class", "form-inline")
				.attr("id", "justBestForm");
	justBestInput
		.append("label")
			.attr("id", "justBestLabel")
			.attr("for","justBestInput")
			.attr("class", "control-label")
			.text("Only Show Best Connections")
			.style("margin-right", "5px");;
	var divjustBestInputGroup = justBestInput
		.append("div")
			.attr("class", "input-group");
	divjustBestInputGroup.append("input")
		.attr("type", "checkbox")
		//.attr("class", "form-control")
		.attr("id", "justBestInput");
	$('#justBestForm').submit(function(e){
        e.preventDefault();
  	});

	}

};

njhSeqView.prototype.updateData = function(inputSeqData){
	var self = this;
	if(inputSeqData["selected"].length > 0){
		if(this.seqData["maxLen"] < inputSeqData["maxLen"]){
			this.seqData["maxLen"] = inputSeqData["maxLen"];
		}
		this.seqData["uid"] = inputSeqData["uid"];
		this.seqData["sessionUID"] = inputSeqData["sessionUID"];
		for(seq in inputSeqData["seqs"]){
			this.seqData["seqs"][inputSeqData["seqs"][seq]["selected"]] = inputSeqData["seqs"][seq];
		}
	}else{
		this.seqData = inputSeqData;
	}

	this.uid = inputSeqData["uid"];
	this.needToPaint = true;
	this.setUp();
	this.paint();
  if(this.qualChart){
  	var qualData = this.getSelectedQualData();
  	d34.select(this.topDivName + " .qualChart")
  		.datum(qualData)
  		.call(self.qualChart);
  }
};

njhSeqView.prototype.resetSelection = function(){
	this.seqStart = 0;
	this.baseStart = 0;
	this.currentSeq = 0;
	this.currentBase = 0;
}

njhSeqView.prototype.setUp = function(){
	this.updateDrawArea();
	this.setUpSliders();
	this.setUpListeners();
	this.paintSelectedSeq();
};

//should be called after a resize of the drawing area or a change of data
njhSeqView.prototype.updateSeqDims = function(){
	this.nBases = Math.min(Math.floor((parseInt(this.seqSvgMaster.style("width")) - this.cw * 3 - this.nameOffSet)/this.cw),this.seqData["maxLen"] );
	this.nSeqs = Math.min(Math.floor((parseInt(this.seqSvgMaster.style("height")) - this.ch)/this.ch), this.seqData["seqs"].length);
}

//set the drawing area size based on window size
njhSeqView.prototype.setDrawAreaSize = function(){
	$(this.masterDiv).width(Math.max((window.innerWidth - 10) * 0.98, this.nameOffSet + this.cw * 40));
	var maxPossHeight = this.ch * (this.seqData["seqs"].length + 4);
	$(this.masterDiv).height(Math.min(Math.max((window.innerHeight - 60) * 0.80 , this.ch * 8), maxPossHeight));
	this.seqSvgMaster.attr("width",  $(this.masterDiv).width() - 20);
	this.seqSvgMaster.attr("height", $(this.masterDiv).height() - 25);
}

//only setups up the drawing area size and the number of seqs and bases to display
njhSeqView.prototype.updateDrawAreaSize = function(){
	var changingHeight = (window.innerHeight - 60) * 0.80;
	var changingWidth =  (window.innerWidth  - 10) * 0.98;

	this.setDrawAreaSize();

	if(changingHeight > parseInt(this.seqSvgMaster.style("height"))){
		this.needToPaint = true;
	}
	if(changingWidth > parseInt(this.seqSvgMaster.style("width"))){
		this.needToPaint = true;
	}
}

njhSeqView.prototype.updateDrawArea = function(){
	this.updateDrawAreaSize();
	this.updateSeqDims();
};


njhSeqView.prototype.paint = function(){
	this.paintSeqs();
	this.updateBaseSeqPos();
	this.setSelector();
	this.updateHighlightedSeqs();
};

njhSeqView.prototype.setSelector = function(){
	if(this.currentBase >= this.baseStart &&
			this.currentBase < this.baseStart + this.nBases &&
			this.currentSeq >= this.seqStart &&
			this.currentSeq < this.seqStart + this.nSeqs){
		$(this.sel).css('top', (this.currentSeq - this.seqStart) * this.ch - 1);
		$(this.sel).css('left', (this.currentBase - this.baseStart) * this.cw + this.nameOffSet - 1);
		$(this.sel).show();
	}else{
		$(this.sel).hide();
	}
};

njhSeqView.prototype.updateHighlightedSeqs = function(){
	var self = this;
	var selectors = d34.select(this.topDivName + " .SeqViewDrawAreaDiv")
		.selectAll(".seqHighlight")
		.data(setToArray(this.selected));

	selectors.exit()
		.remove();



	var lowerBound = this.seqStart;
	var upperBound = this.seqStart + this.nSeqs;
	selectors.enter()
		.append("div")
		.attr("class", "seqHighlight")
		.style("width", function(d){
			return self.nameOffSet.toString() + "px";})
		.style("height",function(d){ return self.ch.toString() + "px";})
		.merge(selectors)
			.style("visibility",function(d){
				if(d >= lowerBound && d < upperBound){
					return "visible";
				}else{
					return "hidden";
				}})
			.style("top", function(d){
				if(d >= lowerBound && d < upperBound){
					return ((d - self.seqStart) *self.ch).toString() + "px";
				}else{
					return 0;
				}})
			.style("left", 0);
};

njhSeqView.prototype.mouseWheelUp = function(steps){
    if(this.seqStart > 0){
    	--this.seqStart;
    	$(this.rSlider).bootstrapSlider('setValue', this.seqStart);
    	this.needToPaint = true;
    	this.paint();
    }else{
    	console.log(steps);
    }
};

njhSeqView.prototype.mouseWheelDown = function(steps){
    if(this.seqStart < Math.max(this.seqData["numReads"]- this.nSeqs, 0)){
    	++this.seqStart;
    	$(this.rSlider).bootstrapSlider('setValue', this.seqStart);
    	this.needToPaint = true;
    	this.paint();
    }
};

njhSeqView.prototype.setUpSliders = function(){
	var self = this;
	//set up base slider
	$(this.bSliderDiv).css("left", this.nameOffSet);
	$(this.bSliderDiv).css("width", this.nBases * this.cw);
	$(this.bSlider).css("width", this.nBases * this.cw);
	//create base slider using bootstrapSlider class
	$(this.bSlider).bootstrapSlider({
	  min: 0,
	  max: Math.max(self.seqData["maxLen"] - self.nBases, 0),
	  value: 0
	});
	//set up update function on slider change
	$( this.bSlider ).on("change", function(changeEvent){
		  self.needToPaint = true;
		  self.baseStart = changeEvent.value.newValue;
		  self.paint();
	});

	//setup seq slider
	$(this.rSliderDiv).css("height", this.nSeqs * this.ch);
	$(this.rSlider).css("height", this.nSeqs * this.ch);
	//create seq slider using bootstrapSlider class
	$( this.rSlider ).bootstrapSlider({
	  min: 0,
	  tooltip_position:'left',
	  max: Math.max(self.seqData["numReads"]- self.nSeqs, 0),
	  value: 0,
	  orientation: "vertical"
	});

	//set up update function on slider change
	$( this.rSlider ).on("change", function(changeEvent){
	  self.needToPaint = true;
	  self.seqStart = changeEvent.value.newValue;
	  self.paint();
	});

};

njhSeqView.prototype.updateOnResize = function(){
	var self = this;
  this.updateDrawArea();
  this.setUpSliders();
  this.paint();
  this.paintSelectedSeq();
  if(this.qualChart){
    this.qualChart.
			width(parseFloat(self.seqSvgMaster.style("width")));
    d34.select(this.topDivName + " .qualChart")
      .call(self.qualChart);
  }

};

njhSeqView.prototype.clicked = function(e){
	var self = this;
  var pt = getRelCursorPosition(e, this.seqSvgMaster.node());
  if(pt[1] <= this.nSeqs * this.ch &&
  		pt[0] <= this.nBases * this.cw + this.nameOffSet){
  	this.currentSeq = Math.ceil(pt[1]/this.ch) + this.seqStart - 1;
    this.currentBase = Math.ceil(pt[0]/this.cw) - this.nameOffSet/this.cw + this.baseStart -1;
    this.paintSelectedSeq();
    this.setSelector();
  	if(pt[0] <= this.nameOffSet){
      if(this.qualChart){
      	var qualData = this.getSelectedQualData();
      	self.qualChart.selectedPos(null);
      	d34.select(this.topDivName + " .qualChart")
      		.datum(qualData)
      		.call(self.qualChart);
      	self.qualChart.selectline()
  				.style("opacity", 0)
      }
  	}else{
  		if(this.qualChart){
      	//draw line on current base position
      	//this.qualChart. chart.setSelectLine(this.currentBase);
      	self.qualChart.selectedPos(self.currentBase);//
      	self.qualChart.selectline()
    			.attr("x1", self.qualChart.xScale()(self.currentBase))
    			.attr("x2", self.qualChart.xScale()(self.currentBase))
    			.style("opacity", 1)
    			.style("stroke-width", "2px")
    			.style("stroke", function(d){ return "#000000";})
  		}
  	}
  }
};

njhSeqView.prototype.setUpListeners = function(){
	var self = this;
   	// add scrolling listener
	// addMouseScrollListener(this.seqSvgMaster.node(), this.mouseWheelUp.bind(this), this.mouseWheelDown.bind(this));
	var mouseScroll = function(deltas){
		//change in x
		if(Math.abs(deltas["deltaX"]) > 0){
			//console.log("change in x");
			//console.log(deltas);
			var changeInBaseStart = false;
			if(deltas["deltaX"] > 0){
				//positive x delta, decrease base start
				if(self.baseStart > 0){
					changeInBaseStart = true;
					self.baseStart = Math.max(0, self.baseStart - deltas["deltaX"]);
				}
			}else{
				//negative x delta, increase base start
				if(self.baseStart < self.seqData["maxLen"] - self.nBases){
					changeInBaseStart = true;
					self.baseStart = Math.min(self.seqData["maxLen"] - self.nBases, self.baseStart - deltas["deltaX"]);
				}
			}
			if(changeInBaseStart){
				//console.log("change in baseStart");
				d34.event.preventDefault();
				$(self.bSlider).bootstrapSlider('setValue', self.baseStart);
		    	self.needToPaint = true;
			}
		}
		//change in y
		if(Math.abs(deltas["deltaY"]) > 0){
			//console.log("change in y");
			//console.log(deltas);
			var changeInSeqStart = false;
			if(deltas["deltaY"] > 0){
				//positive y delta, decrease seq start
				if(self.seqStart > 0){
					changeInSeqStart = true;
					self.seqStart = Math.max(0, self.seqStart - deltas["deltaY"]);
				}
			}else{
				//negative y delta, increase seq start
				if(self.seqStart < self.seqData["numReads"] - self.nSeqs){
					changeInSeqStart = true;
					self.seqStart = Math.min(self.seqData["numReads"] - self.nSeqs, self.seqStart - deltas["deltaY"]);
				}
			}
			if(changeInSeqStart){
				//console.log("change in seqStart");
				d34.event.preventDefault();
				$(self.rSlider).bootstrapSlider('setValue', self.seqStart);
		    	self.needToPaint = true;
			}
		}
		self.paint();
	};
	self.seqSvgMaster.on("mousewheel.zoom", function(){
			var deltas = getDeltasFromEvent(d34.event);
			var modDeltas = {deltaX : Math.round(deltas["deltaX"]/3.0) , deltaY:Math.round(deltas.deltaY/3.0) };
			mouseScroll(modDeltas);
		});
	self.seqSvgMaster.on("DOMMouseScroll", function(){
			var deltas = getDeltasFromEvent(d34.event);
			mouseScroll(deltas);
		});

	this.seqSvgMaster.node().addEventListener("mousedown", this.clicked.bind(this), false);
	//add hover box listening
	var moveLeft = 20;
	var moveDown = 10;
	$(this.seqSvgMaster.node()).hover(function(e) {
	  $(self.popUp).fadeIn(500);
	}, function() {
	  $(self.popUp).hide();
	});

	$(this.seqSvgMaster.node()).mouseleave(function(e) {
		$(self.popUp).hide();
	});
	$(this.seqSvgMaster.node()).mousemove(function(e) {
		var currentPoint = getRelCursorPosition(e, self.seqSvgMaster.node());
	    if(currentPoint[1] <= self.nSeqs * self.ch &&
	    		currentPoint[0] <= self.nBases * self.cw + self.nameOffSet){
	    	var viewWidth = self.nBases * self.cw + self.nameOffSet;

	    	if(currentPoint[0] > viewWidth/2){
	    		moveLeft = - parseFloat(d34.select(self.popUp).style("width")) - 20
	    	}else{
	    		moveLeft = 20
	    	}
	    	$(self.popUp).css('top', currentPoint[1] + moveDown).css('left', currentPoint[0] + moveLeft);
          	var currentBaseHover = Math.ceil(currentPoint[0]/self.cw) - self.nameOffSet/self.cw + self.baseStart -1;
            var currentSeqHover = Math.ceil(currentPoint[1]/self.ch) + self.seqStart - 1;
            if(currentPoint[0] > self.nameOffSet &&
            		undefined == self.seqData["seqs"][currentSeqHover]["seq"][currentBaseHover]){
            	$(self.popUp).hide();
            } else {
            	$(self.popUp).show();
            }
	    } else {
        	$(self.popUp).hide();
        }
	});
};

//draw all necessary seqs
njhSeqView.prototype.paintSeqs = function(){
	if(this.needToPaint){
		var self = this;

		//add any extra seq groups that need to be added
		var sGroups = self.seqSvgMasterG
			.selectAll(".seqGroup")
			.data(self.seqData["seqs"].slice(self.seqStart, self.seqStart + this.nSeqs));
		sGroups.exit()
			.remove();
		var enteringSeqGroups =
			sGroups
			.enter()
				.append("g")
					.attr("class", "seqGroup")
					.attr("transform", function(d,i){
						return "translate(0," + i * self.ch + ")"});

		enteringSeqGroups.append("rect")
			.attr("class", "nameRects")
			.attr("width", this.nameOffSet)
			.attr("height", this.ch)
			.attr("fill", "#EEEEEE")
			.attr("stroke", "#000000")

		enteringSeqGroups.append("text")
			.attr("class", "nameText")
			.attr("x", 4)
			.attr("y", self.ch/1.5)
			.attr("fill", "#000000")
			.style("-webkit-user-select","none")
			.style("-moz-user-select","none")
			.style("-ms-user-select","none")
			.style("-webkit-touch-callout","none")
			.style("-khtml-user-select","none")
			.style("user-select","none")
			.style("pointer-events", "none");


		//get rid of any groups that no longer have data
		var seqGroups = self.seqSvgMasterG
			.selectAll(".seqGroup")

		//set the names
		seqGroups.selectAll(".nameText")
			.text(function(d){
				return d34.select(this.parentNode).datum()["name"];});

		seqGroups.selectAll(".nameRects")
			.on("mouseenter", function(d, i){
				updateTable(self.popTab,[{"name": d34.select(this.parentNode).datum()["name"]}], ["name"]);
			}).on("mousedown", function(d,i){
				self.currentSeq = d34.select(this.parentNode).datum()["selected"];
				if(d34.event.shiftKey ){
	    			if(self.selected.size > 0){
	    				var lastAdded = getLastValue(self.selected);
	    				if(lastAdded == self.currentSeq){
	    					self.selected.delete(self.currentSeq);
	    				}else{
		    				var seqs;
		    				if(lastAdded > self.currentSeq){
		    					seqs = range(self.currentSeq, lastAdded - 1);
		    				}else{
		    					seqs = range(lastAdded + 1, self.currentSeq);
		    				}
	    					for(seqPos in seqs){
	    	            		if(self.selected.has(seqs[seqPos])){
	    	            			self.selected.delete(seqs[seqPos]);
	    	            		}else{
	    	            			self.selected.add(seqs[seqPos]);
	    	            		}
	    					}
	    				}
	    			}else{
	            		if(self.selected.has(self.currentSeq)){
	            			self.selected.delete(self.currentSeq);
	            		}else{
	            			self.selected.add(self.currentSeq);
	            		}
	    			}
				}else{
	        		if(self.selected.has(self.currentSeq)){
	        			self.selected.delete(self.currentSeq);
	        		}else{
	        			self.selected.add(self.currentSeq);
	        		}
				}
				self.updateHighlightedSeqs();
			});
		//add bases rects that need to be added
		var seqGroupsBaseRects = seqGroups.selectAll(".baseRects")
			.data(function(d){ return d["seq"].slice(self.baseStart, self.baseStart + self.nBases);});

		//get rid of any base that no longer have data
		seqGroupsBaseRects
			.exit()
			.remove();

		seqGroupsBaseRects
			.enter()
				.append("rect")
				.attr("class", "baseRects")
				.attr("x", function(d,i){ return self.nameOffSet + i * self.cw})
				.attr("width", this.cw)
				.attr("height", this.ch)
				.on("mouseenter", function(d,i){
					updateTable(self.popTab,[{"name": d34.select(this.parentNode).datum()["name"],
											 "base": d,
											 "qual": d34.select(this.parentNode).datum()["qual"][self.baseStart + i],
											 "pos" : (self.baseStart + i)}], ["name", "base", "qual", "pos"]);
				}).merge(seqGroupsBaseRects)
					.attr("fill", function(d){ return self.bColors[d]})
					.attr("stroke",function(d){ return self.bColors[d]});


		//add base texts that need to be added;
		var seqGroupsBaseTexts = seqGroups.selectAll(".baseTexts")
			.data(function(d){ return d["seq"].slice(self.baseStart, self.baseStart + self.nBases);});
		//get rid of any text that no longer have data
		seqGroupsBaseTexts
			.exit()
			.remove();
		seqGroupsBaseTexts
			.enter()
				.append("text")
				.attr("class", "baseTexts")
				.style("-webkit-user-select","none")
				.style("-moz-user-select","none")
				.style("-ms-user-select","none")
				.style("-webkit-touch-callout","none")
				.style("-khtml-user-select","none")
				.style("user-select","none")
				.style("pointer-events", "none")
				.attr("x", function(d,i){ return self.nameOffSet + i * self.cw + self.cw/4;})
				.attr("y", self.ch * .75)
				.attr("fill", "#000000")
			.merge(seqGroupsBaseTexts)
				.text(function(d,i){ return d;});
    	this.needToPaint = false;
	}
};

//paint seq position
njhSeqView.prototype.updateBaseSeqPos = function(){
	var baseStartPos = this.seqSvgMasterG.selectAll("#baseStartPos")
		.data([this.baseStart]);

	baseStartPos.enter()
		.append("text")
			.attr("id","baseStartPos" )
			.attr("fill", "#000000");


	baseStartPos.attr("x", this.nameOffSet)
		.attr("y", this.nSeqs * this.ch + 2 + this.ch/2.0)
		.text(function(d){return d;});

	var baseStopPos = this.seqSvgMasterG.selectAll("#baseStopPos")
		.data([this.baseStart + this.nBases - 1]);

	baseStopPos.enter()
		.append("text")
			.attr("id","baseStopPos" )
			.attr("fill", "#000000");


	baseStopPos.attr("x", this.nameOffSet + this.nBases * this.cw - this.cw )
		.attr("y", this.nSeqs * this.ch + 2 + this.ch/2.0)
		.text(function(d){return d;});

	var seqStartPos = this.seqSvgMasterG.selectAll("#seqStartPos")
		.data([this.seqStart]);

	seqStartPos.enter()
		.append("text")
			.attr("id","seqStartPos" )
			.attr("fill", "#000000");

	seqStartPos
		.attr("x", this.nameOffSet + this.nBases * this.cw + this.cw/5.0)
		.attr("y", this.ch/1.5)
		.text(function(d){return d;});

	var seqStopPos = this.seqSvgMasterG.selectAll("#seqStopPos")
		.data([this.seqStart + this.nSeqs - 1]);

	seqStopPos.enter()
		.append("text")
			.attr("id","seqStopPos" )
			.attr("fill", "#000000")
			;

	seqStopPos.attr("x", this.nameOffSet + this.nBases * this.cw + this.cw/5.0 )
		.attr("y", this.nSeqs * this.ch - this.ch/4.0)
			.text(function(d){return d;});
};

njhSeqView.prototype.paintSelectedSeq = function(){
  	var logInfo ="";
	if(this.currentBase < 0){
   		logInfo = "name: "
   			+ this.seqData["seqs"][this.currentSeq]["name"];
	} else {
   		logInfo = "name: "
	        	+ this.seqData["seqs"][this.currentSeq]["name"]
	        	+ " base: "  + this.seqData["seqs"][this.currentSeq]["seq"][this.currentBase]
	        	+ " qual: " +  this.seqData["seqs"][this.currentSeq]["qual"][this.currentBase]
	        	+ " pos: " + this.currentBase;
	}

	var selectedSeqInfo = this.seqSvgMasterG.selectAll("#selectedSeqInfo")
		.data([logInfo]);

	selectedSeqInfo.enter()
		.append("text")
			.attr("id","selectedSeqInfo" )
			.attr("fill", "#000000");

	selectedSeqInfo
		.attr("x", this.nameOffSet + this.cw + 20 )
		.attr("y", (this.nSeqs)*this.ch + 2  + this.ch/2)
			.text(function(d){return d;});
};
