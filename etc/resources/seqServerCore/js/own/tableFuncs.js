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
    function tabulate(data, columns, divId) {
		/** Add json data organized row wise into a table in the div with the divId with only the columns given
		 *
		 */
		//adapted from http://stackoverflow.com/questions/9268645/creating-a-table-linked-to-a-csv-file
	    var table = d34.select(divId).append("table"),
	        thead = table.append("thead"),
	        tbody = table.append("tbody");
	
	    // append the header row
	    thead.append("tr")
	        .selectAll("th")
	        .data(columns)
	        .enter()
	        .append("th")
	            .attr("style", "font-weight: bold; padding: 2px 4px;")
	            .html(function(column) { return "<nobr>" + column + "</nobr>"; });
	
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
	            .html(function(d) { return "<nobr>" + d.value + "</nobr>";});
	    
	    return table;
	}
	
	function createTable(divId, id) {
		/**create the table and return it to be manipulate*/
		var divSel = divId;
		if(! (divId instanceof d3.selection) && !(divId instanceof d34.selection)){
			divSel =  d34.select(divId);
		}
		var table = divSel.append("table");
		if(id !== null){
			table.attr("id", id);
		}
		table.append("thead").append("tr");
		table.append("tbody");
		return table;
	}
	
	function updateTable(tab, data, columns){
		//ensure header row
		var headerRow = tab.select("thead")
			.selectAll("tr")
			.data([true])
			.enter();
		//attach column name data to header
		var header = tab.select("thead").select("tr")
	        .selectAll("th")
	        .data(columns).text(function(column) { return column; });
	    header
	        .enter()
			.append("th")
				.style("font-weight", "bold")
				.style("padding", "2px 4px")
				.style("white-space", "nowrap")
	            .text(function(column) { return column; });;

	  //remove any headers that don't have data attached to them
	  header.exit()
        		.remove();
		
	    // create a row for each object in the data
	    var newRows = tab.select("tbody")
	    	.selectAll("tr")
		        .data(data)
		        .enter()
		         .append("tr");
	    //remove
	    tab.select("tbody").selectAll("tr")
	        .data(data)
	        .exit()
	        	.remove();
	    var currentColor = "#e9e9e9";
		var rows = tab.select("tbody").selectAll("tr").style("background-color",function(d,i){
	        		if(i == 0){
	        			return currentColor;
	        		}else {
	        			if (d[columns[0]] != data[i-1][columns[0]]){
	        				if(currentColor == "#e9e9e9"){
	        					currentColor = "#c9c9c9";
	        				}else{
	        					currentColor = "#e9e9e9";
	        				}
	        			}
	        		}
	        		return currentColor;
	        		});;
	    //create a cell in each row for each column	    
	    var cells = rows.selectAll("td")
	        .data(function(row) {
	        	var ret = columns.map(function(column) {
	                return {column: column, value: row[column]};
	            });         
	            return ret;
	        });
	    //remove cells as needed
	    cells.exit()
        	 .remove();
	   	cells.enter()
	        .append("td")
	        .merge(cells)
		        .style("padding", "2px 4px")
				.style("white-space", "nowrap")
		        .text(function(d) { return d.value; });

       
	}
	function updateTableWithColors(tab, data, columns){
		//console.log(data);
		//ensure header row
		var headerRow = tab.select("thead")
			.selectAll("tr")
			.data([true])
			.enter();
		//attach column name data to header
		var header = tab.select("thead").select("tr")
	        .selectAll("th")
	        .data(columns).text(function(column) { return column; });
	    header
	        .enter()
			.append("th")
				.style("font-weight", "bold")
				.style("padding", "2px 4px")
				.style("white-space", "nowrap")
	            .text(function(column) { return column; });;
	   //create headers as needed and add bolding 

	  //remove any headers that don't have data attached to them
	  //console.log(columns);
	  header.exit()
        		.remove();
		
	    // create a row for each object in the data
	    var newRows = tab.select("tbody").selectAll("tr")
	        .data(data)
	        .enter()
	        .append("tr");
	    //remove
	    tab.select("tbody").selectAll("tr")
	        .data(data)
	        .exit()
	        	.remove();
		var rows = tab.select("tbody")
		.selectAll("tr")
		.style("background-color",function(d){
	        		return d.color;});
	    //create a cell in each row for each column
	    //console.log(rows);
	    
	    var cells = rows.selectAll("td")
	        .data(function(row) {
	        	var ret = columns.map(function(column) {
	                return {column: column, value: row[column]};
	            });         
	            return ret;
	        });
	    //remove cells as needed
	    cells.exit()
        	 .remove();
	   	cells.enter()
	        .append("td")
	        .merge(cells)
		        .style("padding", "2px 4px")
				.style("white-space", "nowrap")
		        .text(function(d) { return d.value; });

	}
	
//a grid of links, colNum per row, shown 200 at a time with a filter when there are more (njhPagedList below)
function createLinksTable(addToSelector, linkPrefix, links, colNum, mouseOverColor, mouseLeaveColor){
	return new njhPagedList(addToSelector, {
		names: links,
		hrefs: links.map(function(link){ return linkPrefix + link; }),
		columns: colNum,
		mouseOverColor: mouseOverColor,
		mouseLeaveColor: mouseLeaveColor
	});
}

//createLinksTable() with each link's url part given in encodedLinks
function createLinksTableDifferentLinks(addToSelector, linkPrefix, links, encodedLinks, colNum, mouseOverColor, mouseLeaveColor){
	return new njhPagedList(addToSelector, {
		names: links,
		hrefs: encodedLinks.map(function(link){ return linkPrefix + link; }),
		columns: colNum,
		mouseOverColor: mouseOverColor,
		mouseLeaveColor: mouseLeaveColor
	});
}

//
// njhPagedList: a list of names shown a page at a time with a filter, as links (createLinksTable) or checkboxes,
// only the current page is drawn so lists of tens of thousands of names stay fast
//

/**
 * @param parentSelector where to add the list
 * @param opts
 *   names: the names
 *   mode: "links" or "checkbox"
 *   hrefs: links mode, the link for each name
 *   columns: links mode, links per row, default 8
 *   searchTexts: optional text the filter matches for each name, e.g. the name and its meta, defaults to the names
 *   pageSize: names per page, default 200, a menu changes it when there are more than 100 names
 *   onChange: checkbox mode, called when what's checked changes
 *   title: checkbox mode, shown before the checked count, e.g. Samples
 *   mouseOverColor, mouseLeaveColor: links mode cell colors, default #999 and #FFF
 */
function njhPagedList(parentSelector, opts){
	var self = this;
	self.names = opts.names;
	self.mode = opts.mode || "links";
	self.hrefs = opts.hrefs || self.names;
	self.columns = opts.columns || 8;
	self.pageSize = opts.pageSize || 200;
	self.onChange = opts.onChange || function(){};
	self.mouseOverColor = opts.mouseOverColor || "#999";
	self.mouseLeaveColor = opts.mouseLeaveColor || "#FFF";
	self.searchLower = (opts.searchTexts || self.names).map(function(text){ return String(text).toLowerCase(); });
	self.checked = new Uint8Array(self.names.length).fill(1);
	self.matching = d3.range(self.names.length);
	self.page = 0;
	self.isCheckbox = "checkbox" === self.mode;

	var root = d3.select(parentSelector).append("div").attr("class", "njhPagedList");
	var controls = root.append("div").style("margin-bottom", "4px");
	if(self.isCheckbox){
		var head = controls.append("div").style("margin-bottom", "4px");
		if(opts.title){
			head.append("b").style("margin-right", "8px").text(opts.title);
		}
		self.countText = head.append("span");
	}
	var tools = controls.append("div");
	//the filter and paging only show up when there's enough to need them
	if(self.names.length > 30){
		tools.append("input").attr("type", "search").attr("placeholder", "filter" + (opts.title ? " " + opts.title.toLowerCase() : ""))
			.style("width", "220px").style("margin-right", "8px")
			.on("input", function(){
				self.setFilter(this.value);
			});
	}
	if(self.isCheckbox){
		var addButton = function(text, scope, value){
			tools.append("button").attr("class", "btn btn-default btn-xs").style("margin-right", "4px").text(text)
				.on("click", function(){ self.setAll(value, scope); });
		};
		addButton("Check all", "all", true);
		addButton("Uncheck all", "all", false);
		if(self.names.length > 30){
			addButton("Check filtered", "filtered", true);
			addButton("Uncheck filtered", "filtered", false);
		}
		if(self.names.length > self.pageSize){
			addButton("Check page", "page", true);
			addButton("Uncheck page", "page", false);
		}
	}
	if(self.names.length > 100){
		var sizes = [100, 200, 500, 1000].filter(function(size){ return size < self.names.length; }).concat([self.names.length]);
		if(sizes.indexOf(self.pageSize) < 0){
			sizes.push(self.pageSize);
			sizes.sort(function(first, second){ return first - second; });
		}
		tools.append("span").style("margin", "0 4px 0 8px").text("per page");
		tools.append("select")
			.on("change", function(){
				var firstShown = self.page * self.pageSize;
				self.pageSize = parseInt(this.value);
				self.setPage(Math.floor(firstShown / self.pageSize));
			})
			.selectAll("option").data(sizes).enter().append("option")
				.attr("value", function(size){ return size; })
				.property("selected", function(size){ return size === self.pageSize; })
				.text(function(size){ return size === self.names.length ? "all" : size; });
	}
	self.pager = controls.append("div").style("margin-top", "4px");
	self.body = root.append("div");
	if(self.isCheckbox){
		self.body.style("max-height", "320px").style("overflow", "auto")
			.style("border", "1px solid #ccc").style("padding", "4px");
	} else {
		self.body.style("overflow", "auto");
	}
	self.render();
}

njhPagedList.prototype.pageCount = function(){
	return Math.max(1, Math.ceil(this.matching.length / this.pageSize));
};

njhPagedList.prototype.setFilter = function(query){
	var self = this;
	var lower = query.trim().toLowerCase();
	self.matching = [];
	for(var pos = 0; pos < self.names.length; ++pos){
		if("" === lower || self.searchLower[pos].indexOf(lower) >= 0){
			self.matching.push(pos);
		}
	}
	self.page = 0;
	self.render();
};

njhPagedList.prototype.setPage = function(page){
	this.page = Math.max(0, Math.min(this.pageCount() - 1, page));
	this.render();
};

njhPagedList.prototype.pagePositions = function(){
	return this.matching.slice(this.page * this.pageSize, (this.page + 1) * this.pageSize);
};

/**
 * Check or uncheck names
 * @param value true to check
 * @param scope "all", "filtered" (the names matching the filter) or "page" (the names on this page)
 */
njhPagedList.prototype.setAll = function(value, scope){
	var self = this;
	var positions = "page" === scope ? self.pagePositions() : "filtered" === scope ? self.matching : d3.range(self.names.length);
	positions.forEach(function(pos){ self.checked[pos] = value ? 1 : 0; });
	self.render();
	self.onChange();
};

/**
 * Check only these names, unknown names are skipped
 * @param names the names to check
 * @return the number of names checked
 */
njhPagedList.prototype.setChecked = function(names){
	var self = this;
	var wanted = new Set(names.map(String));
	var count = 0;
	self.names.forEach(function(name, pos){
		self.checked[pos] = wanted.has(String(name)) ? 1 : 0;
		count += self.checked[pos];
	});
	self.render();
	self.onChange();
	return count;
};

/**
 * The checked names, in the order given
 */
njhPagedList.prototype.selected = function(){
	var self = this;
	return self.names.filter(function(name, pos){ return self.checked[pos]; });
};

njhPagedList.prototype.renderPager = function(){
	var self = this;
	self.pager.selectAll("*").remove();
	var total = self.matching.length;
	var pageCount = self.pageCount();
	if(self.names.length <= self.pageSize && total === self.names.length){
		return;
	}
	var first = total === 0 ? 0 : self.page * self.pageSize + 1;
	var last = Math.min(total, (self.page + 1) * self.pageSize);
	if(pageCount > 1){
		var addPageButton = function(text, page, active, disabled){
			self.pager.append("button").attr("class", "btn btn-xs " + (active ? "btn-primary" : "btn-default")).style("margin-right", "2px")
				.property("disabled", !!disabled).text(text)
				.on("click", function(){ self.setPage(page); });
		};
		addPageButton("prev", self.page - 1, false, 0 === self.page);
		//the first, last and nearby pages, with gaps between
		var shown = [];
		for(var page = 0; page < pageCount; ++page){
			if(page < 1 || page >= pageCount - 1 || Math.abs(page - self.page) <= 2){
				shown.push(page);
			}
		}
		shown.forEach(function(page, pos){
			if(pos > 0 && page - shown[pos - 1] > 1){
				self.pager.append("span").style("margin", "0 4px").text("...");
			}
			addPageButton(String(page + 1), page, page === self.page, false);
		});
		addPageButton("next", self.page + 1, false, self.page === pageCount - 1);
	}
	self.pager.append("span").style("margin-left", "8px")
		.text(first + "-" + last + " of " + total + (total < self.names.length ? " matching, " + self.names.length + " in all" : ""));
};

njhPagedList.prototype.render = function(){
	var self = this;
	self.renderPager();
	self.body.selectAll("*").remove();
	var positions = self.pagePositions();
	if(self.isCheckbox){
		var labels = self.body.selectAll("label").data(positions).enter().append("label")
			.style("display", "inline-block").style("margin", "1px 4px").style("font-weight", "normal").style("white-space", "nowrap");
		labels.append("input").attr("type", "checkbox").style("margin-right", "3px")
			.property("checked", function(pos){ return !!self.checked[pos]; })
			.on("change", function(pos){
				self.checked[pos] = this.checked ? 1 : 0;
				self.updateCount();
				self.onChange();
			});
		labels.append("span").text(function(pos){ return self.names[pos]; });
		self.updateCount();
	} else {
		var rows = [];
		for(var start = 0; start < positions.length; start += self.columns){
			rows.push(positions.slice(start, start + self.columns));
		}
		self.body.append("table")
			.style("border-collapse", "collapse")
			.style("border", "2px black solid")
			.selectAll("tr").data(rows).enter().append("tr")
			.selectAll("td").data(function(row){ return row; }).enter().append("td")
				.style("border", "1px black solid")
				.style("padding", "10px")
				.on("mouseover", function(){ d3.select(this).style("background-color", self.mouseOverColor); })
				.on("mouseout", function(){ d3.select(this).style("background-color", self.mouseLeaveColor); })
				.append("a")
					.attr("href", function(pos){ return self.hrefs[pos]; })
					.text(function(pos){ return self.names[pos]; });
	}
};

njhPagedList.prototype.updateCount = function(){
	if(this.countText){
		this.countText.text(this.selected().length + " of " + this.names.length + " checked");
	}
};
