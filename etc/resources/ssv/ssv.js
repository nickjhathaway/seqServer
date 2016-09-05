$(document).ready(function(){
	setHeadTitle("Sequence Viewer");
	var locSplit = window.location.toString().split(/[\/]+/);
	var rName = locSplit[2];
	addDiv("body", "viewer");
	
	var gifLoading = prsentDivGifLoading();
	var cellWidth = 20;
	var cellHeight = 25;
	  makeRequest({
		url: '/' + rName + '/seqData',
		method: "GET"
	}).then(function (datums) {
		var mainData = JSON.parse(datums);
		var SeqViewer = new njhSeqView("#viewer",  mainData, cellWidth, cellHeight, false);
		SeqViewer.init();
		gifLoading.remove()
		$(window).on('beforeunload', function(){
		  	makeRequest({
		  		url: '/' + rName + '/closeSession',
		  		method: "POST",
		  		params: {"sessionUID" : mainData["sessionUID"]},
		  		headers: {"Content-Type" : "application/json"}
		  	}).then(function (datums) {
		  		console.log(JSON.parse(datums));
		  	}).catch(function(err){
		  		removeAllDivGifLoading();
		  		logRequestError(err);
		  	});
		});
	}).catch(function(err){
  		removeAllDivGifLoading();
  		logRequestError(err);
  	});
});
	 	