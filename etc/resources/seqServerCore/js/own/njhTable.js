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

//instanceof d3.selection

function njhTable(masterDivId, tableMasterData, tableDownloadStubName, addChart){
	var self = this;
	this.masterDivId = masterDivId;
	this.tableMasterData = tableMasterData;
	this.tableDownloadStubName = tableDownloadStubName;

	this.poorManHeatmapColoring = false;
	d3.select(masterDivId)
		.style("margin-top", "10px")
		.style("margin-bottom", "10px")
		.attr("class", "njhTable");
	//create internal table divs
	d3.select(masterDivId).append("div").attr("class", "njhTableMenuOrganized");
	this.masterTabDiv = d3.select(masterDivId)
		.append("div")
		.attr("class", "njhMasterTableDiv");
	this.actualTabDiv = this.masterTabDiv.append("div")
		.attr("class", "njhTableDiv");
	this.datTabsD3Dom = this.actualTabDiv.append("table")
		.attr("id", this.masterDivId + "_datTabs" )
		.attr("class", "table table-condensed table-hover nowrap table-nowrap");
	//add data table
	this.datTable = $(this.datTabsD3Dom.node()).DataTable( {
	    "data": self.tableMasterData["tab"],
	    "columns": self.tableMasterData["columnNames"].map(function(col){return {data:col, title:col, name:col}}),
	    "scrollY":        window.innerHeight * 3/4.0 + "px",
	    "scrollX":        true,
//        "scrollCollapse": true,
	    "pageLength":     50,
        "paging":         true,
        "deferRender":    true,
//        "scroller:":      true,
        "buttons": ["csvHtml5"]
	});

	this.colorTable();

	this.datTable.on("order", function(){
		self.colorTable();
	});
	this.datTable.on("draw", function(){
		self.colorTable();
	});

	//
	//add menu
	this.menuOrganizedDiv = d3.select(this.masterDivId + " .njhTableMenuOrganized");
	this.menuOrganized = new njhCheckboxMenuOrganized(this.masterDivId + " .njhTableMenuOrganized", this.tableMasterData["columnNames"], this.toggleColumns.bind(this) );
	//add download button for table
	this.menuOrganizedDiv.append("br");
	this.menuOrganizedDiv.append("button")
		.style("margin-top", "5px")
		.style("margin-bottom", "5px")
		.attr("class", "njhTabSaveButton btn btn-success");
	d34.select(this.masterDivId + " .njhTabSaveButton")
		.append("i")
			.attr("class", "fa fa-download fa-lg");
	d34.select(this.masterDivId + " .njhTabSaveButton")
		.append("span")
			.text(" Download Table");
	this.menuOrganizedDiv.select(".njhTabSaveButton").append("a").attr("class", "njhTabDownLink");
	this.menuOrganizedDiv.select(".njhTabSaveButton").on("click", function() {
		var dataSrc = self.tableToDownloadData();
		var downLink = self.menuOrganizedDiv.select(".njhTabDownLink");
		downLink.attr("download", self.tableDownloadStubName + ".tab.txt");
		downLink.attr("href", dataSrc);
		downLink.node().click();
	});
	//create chart of numeric columns if needed
	d3.select(masterDivId).append("div")
		.attr("class", "njhTableChart");
	this.chart = null;
	if (addChart) {
		this.addChart();
	}

	
	//uncheck the hidden columns
	if(this.tableMasterData["hideOnStartColNames"].length > 0){
		this.tableMasterData["hideOnStartColNames"].forEach(function(d){
			self.menuOrganizedDiv.select("#" + escapeSpecialChars(d))
				.property("checked", false);
		});
		this.updateAllColumnsVisibility();
	}
}

njhTable.prototype.colorTable = function(){

	var rowCount = 0;
	var currentColor = "#e9e9e9";
	var currentValue = "";
	this.datTabsD3Dom.selectAll("tbody tr .sorting_1").each(function(d){
		if(0 != rowCount){
			if(currentValue != d3.select(this).html()){
				if("#e9e9e9" == currentColor){
					currentColor = "#c9c9c9";
				}else{
					currentColor = "#e9e9e9";
				}
			}
		}
		d3.select(this.parentNode).style("background-color", currentColor);
		currentValue = d3.select(this).html();
		++rowCount;
	});

	if(this.poorManHeatmapColoring){
		this.enactPoorMansHeatMap();
	}
}


njhTable.prototype.tableToDownloadData = function(){
	var self = this;
	var allVals = [];
	this.menuOrganizedDiv.selectAll("input:checked").each(function() {
		allVals.push($(this).val());
	});
	var currentColumnNames = _.intersection(self.tableMasterData["columnNames"], allVals);
	var mainTable = [];
	mainTable.push(currentColumnNames);
	//
	for ( i = 0; i < self.tableMasterData["tab"].length; ++i) {
		var currentRow = [];
		for (colNum in currentColumnNames) {
			currentRow.push(self.tableMasterData["tab"][i][currentColumnNames[colNum]]);
		}
		mainTable.push(currentRow);
	}
	mainTable.push([]);
	var dataSrc = 'data:text/csv;base64,' + btoa(d34.tsvFormatRows(mainTable));
	return dataSrc;
}


njhTable.prototype.toggleColumns = function(columns) {
	//columns should be an array of objects with at least two fields, a name field for the column name and a visible boolean field
	var self = this;
	var showCols = [];
	var hideCols = [];
	var chartShowCols = [];
	var chartHideCols = [];
	columns.forEach(function(col){
		if(col["on"]){
			showCols.push(col["name"]);
			if(arrayContains(self.tableMasterData["numericColNames"], col["name"])){
				chartShowCols.push(col["name"]);
			}
		}else{
			hideCols.push(col["name"]);
			if(arrayContains(self.tableMasterData["numericColNames"], col["name"])){
				chartHideCols.push(col["name"]);
			}
		}
	});

	self.datTable.columns(showCols.map(function(col){return col + ":name";}) ).visible(true);
	self.datTable.columns(hideCols.map(function(col){return col + ":name";}) ).visible(false);

	if(this.chart){
		this.chart.show(chartShowCols);
		this.chart.hide(chartHideCols);
	}
};

njhTable.prototype.updateAllColumnsVisibility = function() {
	var self = this;
	var allVals = [];

	d3.selectAll(this.masterDivId + " .njhTableMenuOrganized input:checked").each(function() {
		allVals.push($(this).val());
	});

	var currentOnCols = _.intersection(this.tableMasterData["columnNames"], allVals);
	var currentOffCols = _.difference(this.tableMasterData["columnNames"], allVals);

	self.datTable.columns(currentOnCols.map(function(col){return col + ":name";}) ).visible(true);
	self.datTable.columns(currentOffCols.map(function(col){return col + ":name";}) ).visible(false);

	if(this.chart){
		var showCols = [];
		var hidCols = [];
		for(col in this.tableMasterData["numericColNames"]){
			if(arrayContains(currentOnCols,this.tableMasterData["numericColNames"][col])){
				showCols.push(this.tableMasterData["numericColNames"][col]);
			}else{
				hidCols.push(this.tableMasterData["numericColNames"][col]);
			}
		}
		this.chart.show(showCols);
		this.chart.hide(hidCols);
	}
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
};

njhTable.prototype.updateWithData = function(updatedDataTab){
	var self = this;
	//as long as there isn't different column names this should work, other wise the table menu has to change
	/**@todo might want to add in the ability to update with new column names*/
	this.tableMasterData = updatedDataTab;
	this.datTable.clear().rows.add(this.tableMasterData["tab"]);
	this.datTable.draw();
	this.updateAllColumnsVisibility();
	if(this.addedChart){
		this.addChart();
	}
};


njhTable.prototype.enactPoorMansHeatMap = function(){
	var rowCount = 0;
	this.poorManHeatmapColoring = true;
	var range = []
	this.datTabsD3Dom.selectAll("tbody td").each(function(d){
		if(!isNaN(d34.select(this).html())){
			range.push(parseFloat(d34.select(this).html()))
		}
	});
	var color = d34.scaleLinear()
	   .domain([Math.min.apply(null, range), Math.max.apply(null, range)])
	   .range(["#ffeda0",  "#f03b20"])
	   .interpolate(d34.interpolateHcl);
	this.datTabsD3Dom.selectAll("tbody td").each(function(d){
		if(!isNaN(d34.select(this).html())){
			d34.select(this).style("background-color", color(parseFloat(d34.select(this).html())))
		}
	});

};
