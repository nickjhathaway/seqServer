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
			var SeqViewer = new njhSeqView("#viewer",  mainData, cellWidth, cellHeight, mainData["baseColor"], false, "protein" == mainData["seqType"], mainData["sessionUID"]);
			gifLoading.remove()
			initSeqViewer(SeqViewer);
			$(window).on('beforeunload', function(){
			  	makeRequest({
			  		url: '/' + rName + '/closeSession',
			  		method: "POST",
			  		params: {"sessionUID" : mainData["sessionUID"]},
			  		headers: {"Content-Type" : "application/json"}
			  	}).then(function (datums) {
			  		console.log(JSON.parse(datums));
			  	}).catch(logRequestError);
			});
  	}).catch(logRequestError);
});
	 	