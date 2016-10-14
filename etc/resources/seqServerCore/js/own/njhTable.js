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



function njhTable(masterDivId, tableMasterData, tableDownloadStubName, addChart){
	this.masterDivId = masterDivId;
	this.tableMasterData = tableMasterData;
	this.tableDownloadStubName = tableDownloadStubName;
	d3.select(masterDivId)
		.attr("style", "margin-top:10px; margin-bottom:10px;")
		.attr("class", "njhTable");
	//create internal table divs
	//d3.select(masterDivId).attr("class", "njhTableMenu");
	d3.select(masterDivId).append("div").attr("class", "njhTableMenuOrganized");
	this.masterTabDiv = d3.select(masterDivId)
		.append("div")
		.attr("class", "njhMasterTableDiv")
		.style("overflow", "auto")
		.style("max-height", window.innerWidth/2.0 + "px" );
	//add header table so headers can float
	this.masterTabDiv
		.append("div")
		.attr("class", "njhHeadersDiv")
		.style("position", "absolute")
		.style("background-color", "#AAA")
			.append("table")
			.attr("class", "njhNewHeaders")
			.append("thead").append("tr");
	
	//add the actual table 
	this.masterTabDiv.append("div")
		.attr("class", "njhTableDiv");
		//.style("overflow-y", "hidden")
		//.style("max-height", window.innerWidth/2.0 + "px" );;
	
	this.tabDiv = createTable(this.masterDivId + " ." + "njhTableDiv");
	d3.select(masterDivId).append("div").attr("class", "njhTableChart");
	//populate table 
	updateTable(this.tabDiv,this.tableMasterData["tab"],this.tableMasterData["columnNames"]);
	

	var menuOrganized = d3.select(this.masterDivId + " .njhTableMenuOrganized");
	this.menuOrganized = new njhCheckboxMenuOrganized(this.masterDivId + " .njhTableMenuOrganized", this.tableMasterData["columnNames"],this.updateTableOnClickOrganized.bind(this) );
	
	var self = this;
	//add download button for table 
	menuOrganized.append("br");
	menuOrganized.append("button")
		.style("margin", "5px")
		.text("Download Table")
		.attr("class", "njhTabSaveButton btn btn-success");
	menuOrganized.select(".njhTabSaveButton").append("a").attr("class", "njhTabDownLink");
	menuOrganized.select(".njhTabSaveButton").on("click", function() {
		var allVals = [];
		menuOrganized.selectAll("input:checked").each(function() {
			allVals.push($(this).val());
		});
		var currentColumnNames = _.intersection(self.tableMasterData["columnNames"], allVals);
		var mainTable = [];
		mainTable.push(currentColumnNames);
		//
		for ( i = 0; i < self.tableMasterData["tab"].length; i++) {
			var currentRow = [];
			for (colNum in currentColumnNames) {
				currentRow.push(self.tableMasterData["tab"][i][currentColumnNames[colNum]]);
			}
			mainTable.push(currentRow);
		}
		var dataSrc = 'data:text/csv;base64,' + btoa(d3.tsv.format(mainTable));
		var downLink = menuOrganized.select(".njhTabDownLink");
		downLink.attr("download", self.tableDownloadStubName + ".tab.csv");
		downLink.attr("href", dataSrc);
		downLink.node().click();
	});
	//create chart of numeric columns if needed
	this.chart;
	if (addChart) {
		this.addChart();
	}
	if(this.tableMasterData["hideOnStartColNames"].length > 0){
		this.tableMasterData["hideOnStartColNames"].forEach(function(d){
			menuOrganized.select("#" + String(d).replaceAll(".", "\\.").replaceAll("(", "\\(").replaceAll(")", "\\)").replaceAll("<", "\\<").replaceAll(">", "\\>")).property("checked", false);
		});
		this.updateTableOnClickOrganized();
	}else{
		this.updateHeaders();
	}	
}

njhTable.prototype.updateHeaders = function(){
	/*
	var self = this;
	var headersWidths = [];
	var tab = $(self.masterDivId + " ." + "njhTableDiv table")[0];
	console.log(tab.rows[0]);
	this.tabDiv.selectAll("th").each(function(d,i){
		console.log(d);
		console.log(d3.select(this).style("width"));
		for(var row=0; row<tab.rows.length; ++row){
			console.log(d3.select(tab.rows[row].cells[i]).style("width"));
		}
		headersWidths.push({"head":d, "width":d3.select(this).style("width")});
	});
	console.log(headersWidths);
	var headers = d3.select(this.masterDivId + " .njhNewHeaders").select("thead").select("tr")
	    .selectAll("th")
	    .data(headersWidths);
	//create headers as needed and add bold 
	headers
	    .enter()
		.append("th")
			.style("font-weight", "bold")
			.style("padding", "2px 4px")
			.style("white-space", "nowrap");
	//remove any headers that don't have data attached to them
	headers.exit()
			.remove();
	//update the text and width
	headers
		.text(function(column) { return column.head; })
		.style("width",function(column){ return column.width; });
	*/
}

njhTable.prototype.updateTableOnClickOrganized = function() {
	var allVals = [];
	d3.selectAll(this.masterDivId + " .njhTableMenuOrganized input:checked").each(function() {
		allVals.push($(this).val());
	});
	var currentColumnNames = _.intersection(this.tableMasterData["columnNames"], allVals);
	
	updateTable(this.tabDiv, this.tableMasterData["tab"], currentColumnNames);
	if(this.chart){
		var showCols = [];
		var hidCols = [];
		for(col in this.tableMasterData["numericColNames"]){
			//console.log(this.tableMasterData["numericColNames"][col]);
			if(arrayContains(currentColumnNames,this.tableMasterData["numericColNames"][col])){
				showCols.push(this.tableMasterData["numericColNames"][col]);
				//this.chart.show([this.tableMasterData["numericColNames"][col]]);
			}else{
				hidCols.push(this.tableMasterData["numericColNames"][col]);
				//this.chart.hide([this.tableMasterData["numericColNames"][col]]);
			}
		}
		this.chart.show(showCols);
		this.chart.hide(hidCols);
	}
	this.updateHeaders();

}; 

njhTable.prototype.addChart = function(){
	this.addedChart = true;
	var xLabs = [];
	for (obj in this.tableMasterData["tab"]) {
		xLabs.push(this.tableMasterData["tab"][obj][this.tableMasterData["mainColName"]]);
	}
	this.chart = c3.generate({
		data : {
			json : this.tableMasterData["tab"],
			keys : {
				value : this.tableMasterData["numericColNames"],
			},
			type : 'bar'
		},
		axis : {
			x : {
				type : 'category',
				categories : xLabs,
				tick : {
					rotate : 75
				},
				height : 130
			}
		},
		bindto : this.masterDivId + " .njhTableChart"
	});
	if(this.tableMasterData["hideOnStartColNames"].length > 0){
		this.chart.hide(this.tableMasterData["hideOnStartColNames"]);
	}
};

njhTable.prototype.updateWithData = function(updatedDataTab){
	//as long as there isn't different column names this should work, other wise the table menu has to change
	/**@todo might want to add in the ability to update with new column names*/
	this.tableMasterData = updatedDataTab;
	//this.updateTableOnClick();
	this.updateTableOnClickOrganized();
	if(this.addedChart){
		this.addChart();
	}
};




