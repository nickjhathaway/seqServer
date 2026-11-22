//
// njhServerTable: a table searched, sorted and paged on the server (seqServer TableQuery), so only the shown page comes to the browser,
// for tables too large to send whole; looks and searches like njhTable
//

/**
 * @param masterDivId where to put the table, e.g. "#myTable"
 * @param opts
 *   columnNames: the columns, in the order of the values in each row
 *   numericColNames: columns that take comparisons like >0.1 in their search box
 *   fetchPage: function(query) returning a promise of {recordsTotal, recordsFiltered, data: [[row values]]},
 *     query is {searches: {col: text}, search: text, order: [{col, dir}], start, length}
 *   fetchText: function(query) returning a promise of the matching rows as tab delimited text, query without start and length
 *   downloadStubName: the name for downloaded files
 *   pageLength: rows per page, default 50
 */
function njhServerTable(masterDivId, opts){
	var self = this;
	self.masterDivId = masterDivId;
	self.opts = opts;
	self.columnNames = opts.columnNames;
	self.numericColNames = opts.numericColNames || [];
	self.searches = {};
	self.order = [];
	self.search = "";
	var master = d3.select(masterDivId).attr("class", "njhTable")
		.style("margin-top", "10px").style("margin-bottom", "10px");
	var menu = master.append("div");
	var addDownloadButton = function(text, title, withSearches, fileSuffix){
		menu.append("button").attr("class", "btn btn-success").style("margin", "5px 5px 5px 0")
			.attr("title", title)
			.html('<i class="fa fa-download fa-lg"></i> ' + text)
			.on("click", function(){
				var button = this;
				button.disabled = true;
				self.opts.fetchText(withSearches ? self.currentQuery() : {}).then(function(text){
					var blobUrl = URL.createObjectURL(new Blob([text], {type: "text/plain"}));
					var link = document.createElement("a");
					link.href = blobUrl;
					link.download = self.opts.downloadStubName + fileSuffix + ".tab.txt";
					document.body.appendChild(link);
					link.click();
					document.body.removeChild(link);
					setTimeout(function(){ URL.revokeObjectURL(blobUrl); }, 1000);
				}).catch(logRequestError).then(function(){
					button.disabled = false;
				});
			});
	};
	addDownloadButton("Download Table", "download every row", false, "");
	addDownloadButton("Download Filtered", "download the rows matching the searches", true, "_filtered");
	self.message = menu.append("span").style("margin-left", "8px").style("color", "#C00000");
	var tableNode = master.append("div").append("table")
		.attr("class", "table table-condensed table-hover nowrap table-nowrap");
	var thead = tableNode.append("thead");
	var nameRow = thead.append("tr");
	var searchRow = thead.append("tr").attr("class", "njhTableColumnSearch");
	self.columnNames.forEach(function(col){
		nameRow.append("th").text(col);
		var isNumeric = self.numericColNames.indexOf(col) >= 0;
		searchRow.append("th").style("padding", "2px")
			.append("input")
				.attr("type", "search")
				.attr("data-col", col)
				.attr("placeholder", isNumeric ? "e.g. >0.1" : "search")
				.attr("title", isNumeric ? "text, or a comparison like >0.1, <=5, =0" : "search this column")
				.style("width", "100%").style("min-width", "4em").style("font-weight", "normal");
	});
	self.datTable = $(tableNode.node()).DataTable({
		"serverSide": true,
		"processing": true,
		"orderCellsTop": true,
		"order": [],
		"searchDelay": 400,
		"scrollX": true,
		"pageLength": opts.pageLength || 50,
		"columns": self.columnNames.map(function(col){ return {title: col, name: col}; }),
		"ajax": function(data, callback){
			self.order = data.order.map(function(order){
				return {col: self.columnNames[order.column], dir: order.dir};
			});
			self.search = data.search.value;
			var query = self.currentQuery();
			query.start = data.start;
			query.length = data.length;
			self.opts.fetchPage(query).then(function(page){
				self.message.text("");
				callback({draw: data.draw, recordsTotal: page.recordsTotal, recordsFiltered: page.recordsFiltered, data: page.data});
			}).catch(function(err){
				logRequestError(err);
				self.message.text("couldn't get the rows, see the console");
				callback({draw: data.draw, recordsTotal: 0, recordsFiltered: 0, data: []});
			});
		}
	});
	self.datTable.on("draw", function(){
		self.colorRows();
	});
	//column searches go to the server once typing pauses
	var searchTimer = null;
	$(self.datTable.table().header()).on("input search", ".njhTableColumnSearch input", function(){
		var col = $(this).attr("data-col");
		var val = this.value.trim();
		if(val === (self.searches[col] || "")){
			return;
		}
		if("" === val){
			delete self.searches[col];
		}else{
			self.searches[col] = val;
		}
		clearTimeout(searchTimer);
		searchTimer = setTimeout(function(){ self.datTable.draw(); }, 400);
	});
}

/**
 * The searches and sort shown now, as the query fetchPage and fetchText get (without start and length)
 */
njhServerTable.prototype.currentQuery = function(){
	return {searches: Object.assign({}, this.searches), search: this.search, order: this.order.slice()};
};

/**
 * Shade the rows on this page in bands of the same value in the sorted column (the first column when unsorted), like njhTable
 */
njhServerTable.prototype.colorRows = function(){
	var self = this;
	var colPos = self.order.length > 0 ? Math.max(0, self.columnNames.indexOf(self.order[0].col)) : 0;
	var rowData = self.datTable.rows({page: "current"}).data().toArray();
	var currentColor = "#e9e9e9";
	self.datTable.rows({page: "current"}).nodes().toArray().forEach(function(node, pos){
		if(pos > 0 && String(rowData[pos][colPos]) !== String(rowData[pos - 1][colPos])){
			currentColor = "#e9e9e9" === currentColor ? "#c9c9c9" : "#e9e9e9";
		}
		d3.select(node).style("background-color", currentColor);
	});
};

/**
 * Get the rows again from the first page, e.g. after what the table is on changed, keeping the searches and sort
 */
njhServerTable.prototype.reload = function(){
	this.datTable.draw();
};

/**
 * Remove the table, its handlers included
 */
njhServerTable.prototype.destroy = function(){
	this.datTable.destroy();
	d3.select(this.masterDivId).selectAll("*").remove();
};
