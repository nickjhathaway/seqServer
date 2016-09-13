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
		var sesUid = mainData["sessionUID"];
		var SeqViewer = new njhSeqView("#viewer", mainData);
		gifLoading.remove();
		setUpCloseSession(sesUid);
	}).catch(function(err){
  		removeAllDivGifLoading();
  		logRequestError(err);
  	});

});
